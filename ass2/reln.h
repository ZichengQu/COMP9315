// reln.h ... interface to functions on Relations
// part of signature indexed files
// See reln.c for details on Reln type and functions
// Written by John Shepherd, March 2019

#ifndef RELN_H
#define RELN_H 1

#include "defs.h"

// Relation parameters

typedef struct _RelnParams {
    // dynamic parameters
	Count  npages;     // number of data pages
	Count  ntups;      // number of tuples
	Count  tsigNpages; // number of tsig pages
	Count  ntsigs;     // number of tuple signatures (tsigs)
	Count  psigNpages; // number of psig pages
	Count  npsigs;     // number of page signatures (psigs)
	Count  bsigNpages; // number of bsig pages
	Count  nbsigs;     // number of bit-sliced sigs (bsigs)
    // fixed parameters (set at relation creation time)
	Count  nattrs;     // number of attributes
	char   sigtype;    // CATC == 'c', SIMC == 's'
	float  pF;         // false match probability
	Count  tupsize;    // # bytes in tuples (all same size)
	Count  tupPP;      // max tuples per page
	Count  tk;         // bits set per attribute
	Count  tm;         // width of tuple signature (#bits)
	Count  tsigSize;   // # bytes in tuple signature
	Count  tsigPP;     // max tuple signatures per page
	Count  pm;         // width of page signature (#bits)
	Count  psigSize;   // # bytes in page signature
	Count  psigPP;     // max tuple signatures per page
	Count  bm;         // width of bit-slice (=maxpages)
	Count  bsigSize;   // # bytes in bit-slice
	Count  bsigPP;     // max bit-slices per page
} RelnParams;
	
// Open relation = parameters + open files

typedef struct _RelnRep {
	RelnParams params; // relation parameters
	File  infof;  // handle on info file
	File  dataf;  // handle on data file
	File  tsigf;  // handle on tuple signature file
	File  psigf;  // handle on page signature file
	File  bsigf;  // handle on bit-sliced signature file
} RelnRep;

typedef struct _RelnRep *Reln;

#include "tuple.h"
#include "page.h"

Status newRelation(char *name, Count nattrs, float pF, char sigtype,
				   Count tk, Count tm, Count pm, Count bm);
Reln openRelation(char *name);
void closeRelation(Reln r);
Bool existsRelation(char *name);
PageID addToRelation(Reln r, Tuple t);
void relationStats(Reln r);

// Convenience marcos
// 获取Reln有多少列名
#define nAttrs(REL)      (REL)->params.nattrs
// 获取tuple的大小
#define tupSize(REL)     (REL)->params.tupsize
// 获取signature的签名类型：c：CATA，s: SIMC
#define sigType(REL)     (REL)->params.sigtype

// 获取当前的Reln上有多少个Page
#define nPages(REL)      (REL)->params.npages
// 获取当前的Reln上有多少个Tuple
#define nTuples(REL)     (REL)->params.ntups
// 获取每个Page上最多有多少个Page
#define maxTupsPP(REL)   (REL)->params.tupPP

// 获取当前的Reln上的tuple signature有多少个Page
#define nTsigPages(REL)  (REL)->params.tsigNpages
// 获取当前的Reln上的tuple signature有多少个Tuple
#define nTsigs(REL)      (REL)->params.ntsigs
// 获取当前的Reln上的tuple signature的Page上最多可以有多少Tuple
#define maxTsigsPP(REL)  (REL)->params.tsigPP

// 获取当前的Reln上的page signature有多少个Page
#define nPsigPages(REL)  (REL)->params.psigNpages
// 获取当前的Reln上的page signature有多少个Tuple
#define nPsigs(REL)      (REL)->params.npsigs
// 获取当前的Reln上的page signature的Page上最多可以有多少Tuple
#define maxPsigsPP(REL)  (REL)->params.psigPP

// 获取当前的Reln上的bit-sliced signature有多少个Page
#define nBsigPages(REL)  (REL)->params.bsigNpages
// 获取当前的Reln上的bit-sliced signature有多少个Tuple
#define nBsigs(REL)      (REL)->params.nbsigs
// 获取当前的Reln上的bit-sliced signature的Page上最多可以有多少Tuple
#define maxBsigsPP(REL)  (REL)->params.bsigPP

// 用来做code words的 m 和 k 的长度
#define codeBits(REL)    (REL)->params.tk
#define tsigBits(REL)    (REL)->params.tm
#define psigBits(REL)    (REL)->params.pm
#define bsigBits(REL)    (REL)->params.bm

// Reln对应的相关数据文件
#define dataFile(REL)    (REL)->dataf
#define tsigFile(REL)    (REL)->tsigf
#define psigFile(REL)    (REL)->psigf
#define bsigFile(REL)    (REL)->bsigf

#endif
