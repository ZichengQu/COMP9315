// query.h ... interface to query scan functions
// part of signature indexed files
// See query.c for details of Query type and functions
// Written by John Shepherd, March 2019

#ifndef QUERY_H
#define QUERY_H 1

#include "reln.h"
#include "tuple.h"
#include "bits.h"

// A suggestion ... you can change however you like

typedef struct _QueryRep {
	// static info
	Reln    rel;       // 要查询的那个表，need to remember Relation info
	char   *qstring;   // query的那个语句，query string
	//dynamic info 这些信息要实时的去改
	Bits    pages;     // 检索到的page有多少个，list of pages to examine
	PageID  curpage;   // 当前的pageID，current page in scan
	Count   curtup;    // 当前的tupleID，current tuple within page
	// statistics info 这些信息要实时的去改
	Count   nsigs;     // how many signatures read
	Count   nsigpages; // how many signature pages read
	Count   ntuples;   // how many tuples examined
	Count   ntuppages; // how many data pages read
	Count   nfalse;    // how many pages had no matching tuples
} QueryRep;

typedef struct _QueryRep *Query;

// 根据查询语句构造一个查询对象的结构体
Query startQuery(Reln, char *, char);
// 扫描并显示查询到的结果tuple
void  scanAndDisplayMatchingTuples(Query);
// 显示查询扫描的状态
void  queryStats(Query);
// 关闭查询
void  closeQuery(Query);

#endif
