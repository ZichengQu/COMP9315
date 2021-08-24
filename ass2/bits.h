// bits.h ... interface to functions on bit-strings
// part of signature indexed files
// See bits.c for details of functions
// Written by John Shepherd, March 2019

#ifndef BITS_H
#define BITS_H 1

typedef struct _BitsRep *Bits;

#include "defs.h"
#include "page.h"

// 创建一个新bits
Bits newBits(int);
// 释放掉bits
void freeBits(Bits);
// 判断某一位是否被设置为了1
Bool bitIsSet(Bits, int);
// 判断一个bits是否是另外一个bits的子集
Bool isSubset(Bits, Bits);
// 设置某一位的bit为1
void setBit(Bits, int);
// 设置所有的bits为1
void setAllBits(Bits);
// 设置某一位的bit为0
void unsetBit(Bits, int);
// 设置所有的bits为0
void unsetAllBits(Bits);
// 做 and 操作
void andBits(Bits, Bits);
// 做 or 操作
void orBits(Bits, Bits);
// bits向左移动，int为负数时向右移动
void shiftBits(Bits, int);
// 从Page上获取一个bits （根据signatu的offset到其page上去获取它的bits）
void getBits(Page, Offset, Bits);
// 写入bits到Page上
void putBits(Page, Offset, Bits);
// 显示bits
void showBits(Bits);

#endif
