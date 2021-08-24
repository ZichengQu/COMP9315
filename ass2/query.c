// query.c ... query scan functions
// part of signature indexed files
// Manage creating and using Query objects
// Written by John Shepherd, March 2019

#include "query.h"

#include "bits.h"
#include "bsig.h"
#include "defs.h"
#include "psig.h"
#include "reln.h"
#include "tsig.h"
#include "tuple.h"

// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q) {
    if (*q == '\0') return 0;
    char *c;
    int nattr = 1;
    for (c = q; *c != '\0'; c++)
        if (*c == ',') nattr++;
    return (nattr == nAttrs(r));
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q, char sigs) {
    Query new = malloc(sizeof(QueryRep));
    assert(new != NULL);
    if (!checkQuery(r, q)) return NULL;
    new->rel = r;
    new->qstring = q;
    new->nsigs = new->nsigpages = 0;
    new->ntuples = new->ntuppages = new->nfalse = 0;
    new->pages = newBits(nPages(r)); // 若pages的某一个bit为1，则该bit的page是potential mathc
    switch (sigs) {
        case 't':
            findPagesUsingTupSigs(new);
            break;
        case 'p':
            findPagesUsingPageSigs(new);
            break;
        case 'b':
            findPagesUsingBitSlices(new);
            break;
        default:
            setAllBits(new->pages); // 所有的pages都是potential match，因为要linear scan所有page
            break;
    }
    new->curpage = 0;
    return new;
}

// scan through selected pages (q->pages)
// search for matching tuples and show each
// accumulate query stats

void scanAndDisplayMatchingTuples(Query q) {
    assert(q != NULL);
    // TODO (Done)
    Reln reln = q->rel; // 获取表
    for (q->curpage = 0; q->curpage < nPages(reln); q->curpage++) { // 从第0个page开始获取
		if(!bitIsSet(q->pages, q->curpage)){ // FALSE->continue, TRUE继续执行
            continue;
        }
        Page page = getPage(dataFile(reln), q->curpage); // 获取当前page
        Bool nfalse_flag = TRUE; // 用来验证是否是false match。若page中一个match的都没有，则该flag位TRUE
        for(q->curtup = 0; q->curtup < pageNitems(page); q->curtup++){ // 该page中有多少个tuple，遍历
            Tuple tuple = getTupleFromPage(reln,page,q->curtup);
            if(tupleMatch(reln, tuple, q->qstring)){
                showTuple(reln, tuple); // 显示符合条件的tuple
                nfalse_flag = FALSE;
            }
        }
        
        q->ntuples += pageNitems(page); // 更新读了多少个tuple
        q->ntuppages++; // 更新读了多少个data page
        if(nfalse_flag){ // 上面的tupleMatch未执行
            q->nfalse++; // 更新不匹配个数
        }
    }
}

// print statistics on query

void queryStats(Query q) {
    printf("# sig pages read:    %d\n", q->nsigpages);
    printf("# signatures read:   %d\n", q->nsigs);
    printf("# data pages read:   %d\n", q->ntuppages);
    printf("# tuples examined:   %d\n", q->ntuples);
    printf("# false match pages: %d\n", q->nfalse);
}

// clean up a QueryRep object and associated data

void closeQuery(Query q) {
    free(q->pages);
    free(q);
}
