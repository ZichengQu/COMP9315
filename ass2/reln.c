// reln.c ... functions on Relations
// part of signature indexed files
// Written by John Shepherd, March 2019

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "psig.h"
#include "bits.h"
#include "hash.h"

// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char* name, char* suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname, "%s.%s", name, suffix);
	File f = open(fname, O_RDWR | O_CREAT, 0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char* name, Count nattrs, float pF, char sigtype,
	Count tk, Count tm, Count pm, Count bm)
{
	Reln r = malloc(sizeof(RelnRep));
	RelnParams* p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
		p->sigtype = sigtype;
	p->tupsize = 28 + 7 * (nattrs - 2);
	Count available = (PAGESIZE - sizeof(Count));
	p->tupPP = available / p->tupsize;
	p->tk = tk;
	if (tm % 8 > 0) tm += 8 - (tm % 8); // round up to byte size
	p->tm = tm; p->tsigSize = tm / 8; p->tsigPP = available / (tm / 8);
	if (pm % 8 > 0) pm += 8 - (pm % 8); // round up to byte size
	p->pm = pm; p->psigSize = pm / 8; p->psigPP = available / (pm / 8);
	if (p->psigPP < 2) { free(r); return -1; }
	if (bm % 8 > 0) bm += 8 - (bm % 8); // round up to byte size
	p->bm = bm; p->bsigSize = bm / 8; p->bsigPP = available / (bm / 8);
	if (p->bsigPP < 2) { free(r); return -1; }
	r->infof = openFile(name, "info");
	r->dataf = openFile(name, "data");
	r->tsigf = openFile(name, "tsig");
	r->psigf = openFile(name, "psig");
	r->bsigf = openFile(name, "bsig");
	addPage(r->dataf); p->npages = 1; p->ntups = 0;
	addPage(r->tsigf); p->tsigNpages = 1; p->ntsigs = 0;
	addPage(r->psigf); p->psigNpages = 1; p->npsigs = 0;
	addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = 0; // replace this // 这行保留，否则第一次取nBsigPages(r) - 1的时候为-1, not 0。会报错
	// Create a file containing "pm" all-zeroes bit-strings,
	// each of which has length "bm" bits
	//TODO
	// 因为pm已知，所以bit-slice signature的个数已知。因为bit-slice signature的bm已知。
	// 因此这里直接为bit-slice创建pm个bm长的bit-slice signature。暂时全部用0表示。
	Bits bit_slice = newBits(bm); // 因为每个bit_slice都是长度为bm个bits，且全为0。因此统一创建，将其加入到bsig的page中。
	for (int bsigf_tid = 0; bsigf_tid < pm; bsigf_tid++) { //bsigf_tid是多少个bit signature的tuple的计数
		Page bsig_page = getPage(bsigFile(r), nBsigPages(r) - 1); // 获取存储bit-slice signature的第nBsigPages(r) - 1个page
		if (pageNitems(bsig_page) == maxBsigsPP(r)) { // 如果该页面的signature的个数 == 页面最大容量
			addPage(bsigFile(r)); // 为存储big-slice signature的file添加新的空白page
			r->params.bsigNpages++; // 更新page个数
			free(bsig_page);
			bsig_page = newPage(); // 创建新page
			if (bsig_page == NULL) return NO_PAGE;
		}
		putBits(bsig_page, pageNitems(bsig_page), bit_slice); // 将bit_slice signature放入该page的第pageNitems个位置
		r->params.nbsigs++; // 更新bit_slice signature的数量
		putPage(bsigFile(r), nBsigPages(r) - 1, bsig_page); // 将该page写入到file的第nBsigPages(r) - 1个位置
		// unsetAllBits(bit_slice); // 未改变bit_slice，因此无需重置
	}
	free(bit_slice);

	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char* name)
{
	char fname[MAXFILENAME];
	sprintf(fname, "%s.info", name);
	File f = open(fname, O_RDONLY);
	if (f < 0)
		return FALSE;
	else {
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char* name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name, "info");
	r->dataf = openFile(name, "data");
	r->tsigf = openFile(name, "tsig");
	r->psigf = openFile(name, "psig");
	r->bsigf = openFile(name, "bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof); close(r->dataf);
	close(r->tsigf); close(r->psigf); close(r->bsigf);
	free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Bool newPage_flag = FALSE;
	Page p;  PageID pid;
	RelnParams* rp = &(r->params);

	// add tuple to last page
	pid = rp->npages - 1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP) {
		addPage(r->dataf);
		rp->npages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
		newPage_flag = TRUE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++;  //written to disk in closeRelation()
	putPage(r->dataf, pid, p);

	// compute tuple signature and add to tsigf
	// TODO
	Bits tsig = makeTupleSig(r, t); // Tuple Signature
	Page tsig_page = getPage(tsigFile(r), nTsigPages(r) - 1); // 在tsigFile文件中，根据存储tuple signature的pageID去得到该page // PageID tsig_pid = nTsigPages(r) - 1;
	if (pageNitems(tsig_page) == maxTsigsPP(r)) {
		addPage(tsigFile(r)); // 添加一个空白page到存储tuple signature的文件中
		rp->tsigNpages++; // 更新存储tuple signature的page数
		free(tsig_page);
		tsig_page = newPage();
		if (tsig_page == NULL) return NO_PAGE;
	}
	// 一个putBits里存放一个signature(所以需要新加)，一个putPage里存放多个signature。这是为什么nTsigPages要减1的原因
	putBits(tsig_page, pageNitems(tsig_page), tsig); // 将该tuple signature放入该page的第pageNitems个位置
	rp->ntsigs++; // 更新tuple signature的数量
	putPage(tsigFile(r), nTsigPages(r) - 1, tsig_page); // 将该page放入存储tuple signature的文件的第nTsigPages(r) - 1个位置
	free(tsig);

	// compute page signature and add to psigf

	//TODO
	Bits psig = makePageSig(r, t); // Page Signature // 因为bit-slice要用，因此在bit-slice用完之后再free
	Page psig_page = getPage(psigFile(r), nPsigPages(r) - 1);
	if (newPage_flag) { //如果因为tuple插入而产生新的tuple page
		if (pageNitems(psig_page) == maxPsigsPP(r)) {
			addPage(psigFile(r)); // 添加一个空白page到存储page signature的文件中
			rp->psigNpages++; // 更新存储page signature的page数
			free(psig_page);
			psig_page = newPage();
			if (psig_page == NULL) return NO_PAGE;
		}
		putBits(psig_page, pageNitems(psig_page), psig); // 将该page signature放入该page的第pageNitems个位置
		rp->npsigs++; // 更新page signature的数量
	}
	else if (pageNitems(psig_page) == 0) { // 当第一次加入tuple的时候，会产生新的page，但不通过newPage_flag调用newpage，也无法在else中getBits
		putBits(psig_page, 0, psig); // 0 == pageNitems(psig_page)
		rp->npsigs++; // 更新page signature的数量
	}
	else { // 没有新的tuple page，只是将新的makePageSig(r, t)的psig与旧的最后一个做or操作
		Bits old_psig = newBits(psigBits(r));
		getBits(psig_page, pageNitems(psig_page) - 1, old_psig);
		orBits(old_psig, psig);
		putBits(psig_page, pageNitems(psig_page) - 1, old_psig);
		free(old_psig);
	}
	// rp->npsigs++; // 只是原位更新，没有添加新的page signature
	putPage(psigFile(r), nPsigPages(r) - 1, psig_page);
	

	// use page signature to update bit-slices
	//TODO
	int pm = psigBits(r); // page signature的长度为pm，则bit-slice共pm个
	int bit_offset = nPsigs(r) - 1; // 有多少个page signature，则bit-slice中的有效位数就为几，大于该数的bit肯定都为0

	Bits bsig = newBits(bsigBits(r)); 
	
	for(int i = 0; i < pm; i++){
		if(!bitIsSet(psig, i)){ //如果该位为0，continue，如果为1，则将bsig对应位置设为1
			continue;
		}
		int bsig_pageID = i / maxBsigsPP(r); // i的位置对应的bit-slice的pageID
		int bsigf_tid = i % maxBsigsPP(r); // i的位置对应的该Page中的第几个tuple
		Page bsigf_page = getPage(bsigFile(r), bsig_pageID); // 根据上述两个参数，获取存储该bsig的page
		getBits(bsigf_page, bsigf_tid, bsig); // 取出该bsig
		setBit(bsig, bit_offset); // 将bsig的该位设为1
		putBits(bsigf_page, bsigf_tid, bsig); // 将更新后的bsig写回该page的第bsigf_tid个位置
		// rp->nbsigs++; //不需要，因为前面new的时候，已经设置好了
		putPage(bsigFile(r), bsig_pageID, bsigf_page); // 将更新的包含该bsig的page写回到该file
		unsetAllBits(bsig); //用完了这个bsig后要清零
	}
	free(bsig);
	free(psig);

	return nPages(r) - 1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams* p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
	printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
		p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
	printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
		p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
	printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
		p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs   %s",
		p->sigtype == 'c' ? "catc" : "simc");
	if (p->sigtype == 's')
		printf("  bits/attr: %d", p->tk);
	printf("\n");
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
		p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
		p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
		p->bm, p->bsigSize, p->bsigPP);
}
