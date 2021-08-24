// psig.c ... functions on page signatures (psig's)
// part of signature indexed files
// Written by John Shepherd, March 2019

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"

// Make a codeword for a attribute
Bits codeword_psig(char *attr_value, int cur_attr, Reln r) {
    char sigType = sigType(r);
    int m = psigBits(r); // 获取page signature的长度m
    int k = codeBits(r); // 获取page signature的为1的个数k
    int attr_num = nAttrs(r); // 获取该relation里的attr个数

    int nbits = 0;            // count of set bits
    Bits cword = newBits(m);  // assuming m <= 32 bits
    int m_backup = m; // 为了sigType为CATC
    int len = strlen(attr_value);
    Word hash = hash_any(attr_value, len);
    srandom(hash); // 固定随机种子
    if (sigType == 'c') {
        if (!cur_attr) {  // 第一个属性的m
            m = m / attr_num + m % attr_num;
        } else { // 其它属性的m
            m = m / attr_num;
        }
        k = m / (2 * maxTupsPP(r));
    }
    while (nbits < k) { // 当被设置为1的个数小于k时
        int i = rand() % m; // 随机生成一个数(这里的随机种子已被固定)
        if (bitIsSet(cword, i) == 0) { // 如果该位为0，则设置为1
            setBit(cword,i);
            nbits++;
        }
    }
    if (sigType == 'c') { // 如果sigType为CATC的话，则将第cur_attr个属性的shiftBits
        shiftBits(cword,cur_attr == 0? 0: (m_backup / attr_num + m_backup % attr_num) +m * (cur_attr -1));  // 若是第一个属性，则移动0位。假如是第三个属性，则移动第一个属性的长度，加第二个属性的长度。第一个属性长度可能与其他属性长度不一致。
    }
	
    return cword;
}

Bits makePageSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);
	// TODO
    int m = psigBits(r); // 获取page signature的长度m
    // int k = codeBits(r); // 获取page signature的为1的个数k

    Bits Psig = newBits(m);

    int attr_num = nAttrs(r); // 获取该relation里的attr个数
    char** attr = tupleVals(r, t); // 将tuple里的内容转换成attr数组

    for (int i = 0; i < attr_num; i++) {
        if(strcmp(attr[i],"?")==0){ // 如果是"?"，则代表着query中的"?"，则不做处理
            continue;
        }
        Bits CW = codeword_psig(attr[i], i, r); // 为该属性生成page signature
		
        orBits(Psig, CW);

        free(CW);
    }
	
    return Psig;
}

void findPagesUsingPageSigs(Query q)
{
	assert(q != NULL);
	//TODO
    Bits QuerySig = makePageSig(q->rel, q->qstring); // 获取query的tuple signature
    Bits psig = newBits(psigBits(q->rel)); // 用来存储每一个page signature

    for (q->curpage = 0; q->curpage < nPsigPages(q->rel); q->curpage++) { // 从第0个page指示器开始筛选
        Page page = getPage(psigFile(q->rel), q->curpage); //获取该query对应的page signature文件的第某个page
        for(q->curtup=0; q->curtup < pageNitems(page); q->curtup++){ // 遍历该page中的所有的page signatures
            getBits(page, q->curtup, psig); // 获取该page中的某一个page signature
            if(isSubset(QuerySig, psig)){ // 如果满足
                setBit(q->pages, (q->curpage * maxPsigsPP(q->rel) + q->curtup));
            }
        }
        q->nsigs += pageNitems(page); // 读取signature的个数
        q->nsigpages++; // 读取的signature的page数
        unsetAllBits(psig); // 重置psig所有位为0
	}
    free(psig);
    free(QuerySig);
}