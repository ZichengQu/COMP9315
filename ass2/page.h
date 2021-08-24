// page.h ... interface to functions on Pages
// part of signature indexed files
// See pages.c for descriptions of Page type and functions
// Written by John Shepherd, March 2019

#ifndef PAGE_H
#define PAGE_H 1

typedef struct _PageRep *Page;

#include "defs.h"
#include "reln.h"
#include "tuple.h"

// 创建一个page
Page newPage();
// 增加一个page到file上
void addPage(File);
// 根据pageID获取一个page
Page getPage(File, PageID);
// 根据pageID写入一个page
Status putPage(File, PageID, Page);
// 定位到page的指定位置。getPage 和 putPage需要用到
Byte *addrInPage(Page, int, int);
// 获取page上的tuple个数
Count pageNitems(Page);
// page增加一条记录
void  addOneItem(Page);

#endif
