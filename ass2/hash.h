// hash.h ... interface to hash function
// part of signature indexed files
// Hash function from PostgreSQL
// Edited by John Shepherd, March 2019
// DO NOT CHANGE this file

#ifndef HASH_H
#define HASH_H 1

#include "bits.h"

// 多少位的hash，在codeword时需要用
Word hash_any(char *, int);

#endif
