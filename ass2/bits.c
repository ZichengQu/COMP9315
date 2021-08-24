// bits.c ... functions on bit-strings
// part of signature indexed files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, March 2019

#include "bits.h"

#include <assert.h>

#include "defs.h"
#include "page.h"

typedef struct _BitsRep {
    Count nbits;        // how many bits
    Count nbytes;       // how many bytes in array
    Byte bitstring[1];  // array of bytes to hold bits
                        // actual array size is nbytes
} BitsRep;

// create a new Bits object

Bits newBits(int nbits) {
    Count nbytes = iceil(nbits, 8);
    Bits new = malloc(2 * sizeof(Count) + nbytes);
    new->nbits = nbits;
    new->nbytes = nbytes;
    memset(&(new->bitstring[0]), 0, nbytes);
    return new;
}

// release memory associated with a Bits object

void freeBits(Bits b) {
    // TODO (Done)
    // free(b->bitstring); // memset不分配空间，类似于strcpy
    free(b);
}

// check if the bit at position is 1

Bool bitIsSet(Bits b, int position) {
    assert(b != NULL);
    assert(0 <= position && position < b->nbits);
    // TODO (Done)
    Byte mask = 1 << (position % 8); // position%8 是某一个b->bitstring里的第几个bit(bit是从右向左数)
    return b->bitstring[position / 8] & mask; // position/8 是第几个b->bitstring
}

// check whether one Bits b1 is a subset of Bits b2
Bool isSubset(Bits b1, Bits b2) {
    assert(b1 != NULL && b2 != NULL);
    assert(b1->nbytes == b2->nbytes); // 根据其它模块要求，isSubset的前提，是两个b的nbytes一致
    // TODO (Done)
    int flag = TRUE;
    if (b1->nbits > b2->nbits) {
        flag = FALSE; // 如果b1的nbits大于b2的nbits，则b1肯定不是b2的子集
    }
    for (int i = 0; i < b1->nbytes; i++) { // 按每个bit去遍历
        if (b1->bitstring[i] == 0) { // b1->bitstring[i] == 0 等价于 b1->bitstring[i] == 00000000 (0<<7)
            continue; // 如果是0，则00000000是任何 8bits 的子集，跳过
        } else if (b1->bitstring[i] != (b1->bitstring[i] & b2->bitstring[i])) { // 如果b1的该bitstring与b2的相交，其结果与b1原本的bitstring不等，则证明其不是子集
            flag = FALSE;
            break;
        }
    }
    return flag;  // remove this
}

// set the bit at position to 1

void setBit(Bits b, int position) {
    assert(b != NULL);
    assert(0 <= position && position < b->nbits);
    // TODO  (Done)
    Byte mask = 1 << (position % 8); // 将 1 左移 (position % 8) 位
    b->bitstring[position / 8] |= mask; // 将第 (position / 8) 个bitstring的第 (position % 8) 个bit赋值为 1
}

// set all bits to 1

void setAllBits(Bits b) {
    assert(b != NULL);
    // TODO (Done)
    for (int i = b->nbytes - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            Byte mask = (1 << j);
            b->bitstring[i] |= mask; // 将每一个 b->bitstring 的每一位都设为 1
        }
    }
}

// set the bit at position to 0

void unsetBit(Bits b, int position) {
    assert(b != NULL);
    assert(0 <= position && position < b->nbits);
    // TODO (Done)
    Byte mask = 1 << (position % 8); // 将 1 左移 (position % 8) 位，1的右侧补0
    mask = ~(mask); // 取反，1的bit变为0，0的bit变为1
    b->bitstring[position / 8] &= mask; // 将第 (position / 8) 个 b->bitstring 的第 (position % 8) 个 bit 设为 0
}

// set all bits to 0

void unsetAllBits(Bits b) {
    assert(b != NULL);
    // TODO  (Done)
    for (int i = b->nbytes - 1; i >= 0; i--) {
        b->bitstring[i] &= 0; // 每个 b->bitstring 都设为0
    }
}

// bitwise AND ... b1 = b1 & b2

void andBits(Bits b1, Bits b2) {
    assert(b1 != NULL && b2 != NULL);
    assert(b1->nbytes == b2->nbytes);
    // TODO (Done)
    for (int i = b1->nbytes - 1; i >= 0; i--) {
        b1->bitstring[i] &= b2->bitstring[i]; // 将每个 b1->bitstring 与 b2->bitstring 做逻辑与运算
    }
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2) {
    assert(b1 != NULL && b2 != NULL);
    assert(b1->nbytes == b2->nbytes);
    // TODO (Done)
    for (int i = b1->nbytes - 1; i >= 0; i--) {
        b1->bitstring[i] |= b2->bitstring[i]; // 将每个 b1->bitstring 与 b2->bitstring 做逻辑或运算
    }
}

// left-shift ... b1 = b1 << n
// negative n gives right shift

void shiftBits(Bits b, int n) {
    // TODO (Done)
    char temp[b->nbytes];
    memcpy(temp, b->bitstring, b->nbytes); // 将b->bitstring复制到temp中
    unsetAllBits(b); // 将b的bitstring 都设为0

    for (int i = b->nbytes - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            Byte mask = (1 << j);
            if (temp[i] & mask) {
                int position = (i * 8 + j) + n; // 若原始的bits的该位为1,则取其原始位置，并加上左移n位后，得到新position
                if (0 <= position && position < b->nbits) { // 判断是否超出范围
                    setBit(b, position); // 将b->bitstring的第position个bit设为1
                }
            }
        }
    }

}

// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b) {
    // TODO (Done)
    assert(p != NULL && pos < pageNitems(p) && b != NULL);
    Byte* temp = addrInPage(p, pos, b->nbytes); // 获取存储在该Page中的Offset的位置的地址
    memcpy(b->bitstring, temp, b->nbytes);
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b) {
    // TODO (Done)
    // assert(p != NULL && pos <= pageNitems(p) && b != NULL);
    Byte* temp = addrInPage(p, pos, b->nbytes); // 获取存储在该Page中的Offset的位置的地址
    memcpy(temp, b->bitstring, b->nbytes); // 向该地址写入b->bitstring的数据
    if (pos == pageNitems(p)) { // 有些情况，比如原数据修改的等，不需要addOneItem。当pos==pageNitems时表示pos为该Page上一个新的items(因为该page上的item编号从0到pageNitems-1)
        addOneItem(p);
    }
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b) {
    assert(b != NULL);
    for (int i = b->nbytes - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            Byte mask = (1 << j);
            if (b->bitstring[i] & mask)
                putchar('1');
            else
                putchar('0');
        }
    }
}
