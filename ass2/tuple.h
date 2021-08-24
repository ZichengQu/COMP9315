// tuple.h ... interface to functions on Tuples
// part of signature indexed files
// A Tuple is just a '\0'-terminated C string
// Consists of "val_1,val_2,val_3,...,val_n"
// See tuple.c for details on functions
// Written by John Shepherd, March 2019

#ifndef TUPLE_H
#define TUPLE_H 1

typedef char *Tuple;

#include "reln.h"
#include "page.h"

// 主要的作用是从标准输入stdin里读入一条tuple/record
Tuple readTuple(Reln r, FILE *f);
// 将tuple的内容转换成一个字符串的二位数组，里面存的是所有的属性
// array[0] = "test"
// array[1] = "1"
char **tupleVals(Reln r, Tuple t);
// 释放掉属性换算过来的二维数组
void freeVals(char **vals, int nattrs);
// 判断两个tuple是否相等
// 注意：这两个tuple可以包含？，如果匹配到？的地方会跳过
Bool tupleMatch(Reln r, Tuple t1, Tuple t2);
// 将一个tuple的数据添加到page上
// 注意：是将tuple添加到page上，并不是将tuple添加到对应的文件上
//      如果需要将tuple写入文件，需要调用putPage方法
Status addTupleToPage(Reln r, Page p, Tuple t);
// 根据 i （position）来一个条tuple 
Tuple getTupleFromPage(Reln r, Page p, int i);
// 输出一个tuple
void showTuple(Reln r, Tuple t);

#endif
