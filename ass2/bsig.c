// bsig.c ... functions on Tuple Signatures (bsig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "bsig.h"

void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	Bits QuerySig = makePageSig(q->rel, q->qstring); // 获取query的page signature
	setAllBits(q->pages); // 将所有位都设为1

    Bits bsig = newBits(bsigBits(q->rel)); // 用来存储每一个从bit-slice signature
    
    int bsig_pageID = -1;
    for (q->curtup = 0; q->curtup < psigBits(q->rel); q->curtup++) { // psigBits是page signature的长度，同时也是bit-slice signature的个数
        if(!bitIsSet(QuerySig, q->curtup)){ // 如果该位是0，则continue，是1才继续执行
		continue;
	}
	if(bsig_pageID != q->curtup / maxBsigsPP(q->rel)){ // 如果当前记录的pageID与实际不符
            q->nsigpages++; // 读取的signature的page数
            bsig_pageID =  q->curtup / maxBsigsPP(q->rel); // 更新bit-slice的pageID
        }
	int bsigf_sid = q->curtup % maxBsigsPP(q->rel); // 更新bit-slice的某一个Page的第几个signature
	Page bsigf_page = getPage(bsigFile(q->rel), bsig_pageID); // 取得该bit-slice signature所在的page

	getBits(bsigf_page, bsigf_sid, bsig); // 取得该bit-slice signature
        for(int i=0; i< nPsigs(q->rel); i++){ // 当前Page signature有nPsigs个tuple (signatures)，则bit-slice signature的长度就应该是多少。这个和初始化的bm不一致。bm要大于等于nPsigs
            if(!bitIsSet(bsig, i)){ // bit-slice signature的哪位为0
                unsetBit(q->pages, i); // 就将query->page的哪位设为0
            }
        }
        q->nsigs++; // 读取signature的个数
        unsetAllBits(bsig); // 重置bsig所有位为0
    }
    free(bsig);
    free(QuerySig);
}
