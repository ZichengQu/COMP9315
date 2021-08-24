/*
 * src/tutorial/IntSet.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "postgres.h"

#include "fmgr.h"

PG_MODULE_MAGIC;

typedef struct intset *IntSet;

struct intset{
    int len;
    uint32 set[FLEXIBLE_ARRAY_MEMBER]; // Non-negative integers (natural numbers), less than INT_MAX.
} intset;

int is_valid(char *inputs);
void str_processing(char *str);
int compare(const void *num_1, const void *num_2);
int *convert_set(char *str);

// Determine whether the input IntSet is valid.
int is_valid(char *str){
    regex_t re;
    int cflags = REG_EXTENDED;
    const char *pattern = "^[ ]*\\{[ ]*[0-9]+[ ]*([ ]*,[ ]*[0-9]+[ ]*)*\\}[ ]*$|\\{[ ]*\\}";
    regcomp(&re, pattern, cflags);
    if (regexec(&re, str, 0, NULL, 0) == REG_NOMATCH){
        return 0;
    }
    regfree(&re);
    return 1;
}

// Remove space and {} for this input.
void str_processing(char *str){
    int i, j = 0;
    for (i = 0; str[i] != '\0'; i++){
        if (str[i] != ' ' && str[i] != '{' && str[i] != '}'){
            str[j++] = str[i]; // Remove space and {} for this input.
        }
    }
    str[j] = '\0'; // Label the index j as the end of this str.
}

// Compare two numbers, used in qsort and bsearch.
int compare(const void *num_1, const void *num_2){
    return *(const int *)num_1 - *(const int *)num_2;
}

// Convert the numbers in str into integers, and store them in the set (array) with deduplicated, then sort this set.
int *convert_set(char *str){
    int *set;          // An set (array) to hold the deduplicated, sorted numbers.
    int set_index = 1; // Index 0 is reserved for the length of this set, the values in the str are stored from index 1.
    int set_size = 1;  // The set_size must be 1 at least since the index 0 is always used.

    const char *delim = ",";
    char *token = strtok(str, delim); // Only the comma and numbers remained after str_processing.

    set = (int *)malloc(sizeof(int) * set_size); // At least 1 sizeof(int) is used for index 0 in the set.

    while (token != NULL){
        int flag = 1;
        token[strlen(token)] = '\0';

        for (int i = 1; i < set_index; i++){ // Index 0 is reserved, so start from 1.
            if (set[i] == atoi(token)){ // If it has been repeated.
                flag = 0;
                break;
            }
        }
        if (flag){ // If not repeated, add this number into the set.
            if (set_index >= set_size){ // Check whether allocated memory is enough.
                set_size *= 2;
                set = (int *)realloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = atoi(token);
        }
        token = strtok(NULL, delim);
    }
    set[0] = set_index; // Index 0 is for the length of this set, including the index 0 itself.
    qsort((set + 1), set[0] - 1, sizeof(int), compare); // Sort from index 1, the length should be subtracted by 1 for index 0.
    return set;
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/
PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS){
    char *str = PG_GETARG_CSTRING(0);
    IntSet intset;
    int *set;

    if (is_valid(str) == 0){ // If not valid, then report the error and terminate here.
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                 errmsg("invalid input syntax for type %s: \"%s\"",
                        "IntSet", str)));
    }

    str_processing(str);    // Remove the space and {}
    set = convert_set(str); // Returns a set (array) with values being deduplicated and sorted.

    intset = (IntSet)palloc(VARHDRSZ + (sizeof(int) * set[0]));
    SET_VARSIZE(intset, VARHDRSZ + (sizeof(int) * set[0]));

    for (int i = 0; i < set[0]; i++){
        intset->set[i] = set[i];
        // elog(NOTICE, "intset->set[%d] = %d\n", i, intset->set[i]);
    }
    free(set);

    PG_RETURN_POINTER(intset);
}

PG_FUNCTION_INFO_V1(intset_out);

Datum
intset_out(PG_FUNCTION_ARGS){
    IntSet intset = (IntSet)PG_GETARG_POINTER(0);
    int result_len = intset->set[0]; // Initial result length.
    int max_input_len = 10;          // INT_MAX can be represented within 10 bits.
    char *result = (char *)palloc(VARHDRSZ + sizeof(char) * result_len);
    strcpy(result, "\0");
    strcat(result + strlen(result), "{");

    for (int i = 1; i < intset->set[0]; i++){
        if (strlen(result) + max_input_len >= result_len){ // Check whether allocated memory is enough.
            result_len *= 2;
            result = (char *)repalloc(result, sizeof(char) * result_len);
        }
        strcat(result, psprintf("%d", intset->set[i])); // Append the integer into char* result.
        strcat(result, ",");                            // Add a comma.
    }
    if (result[strlen(result) - 1] == ','){
        result[strlen(result) - 1] = '}'; // Replace the comma with } if the very end of this result is comma.
    }
    else{
        if (strlen(result) + 1 >= result_len){
            result = (char *)repalloc(result, sizeof(char) * ++result_len); // Only append one }, so ++result_len is enough.
        }
        strcat(result, "}"); // Append } if the raw input is kind of like {}
    }

    strcat(result, "\0");

    PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * New Operators
 *
 * A practical IntSet datatype would provide much more than this, of course.
 *****************************************************************************/
PG_FUNCTION_INFO_V1(contain);

Datum
contain(PG_FUNCTION_ARGS){
    int *flag;
    int result;
    int i = PG_GETARG_INT32(0);
    IntSet S = (IntSet)PG_GETARG_POINTER(1);

    // Binary Search to check whether integer i is in IntSet S or not.
    flag = (int *)bsearch(&i, (S->set + 1), S->set[0] - 1, sizeof(int), compare); // Index 0 is reserved, so the length should be substracted by 1 for index 0.

    result = flag == NULL ? 0 : 1;

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(cardinality);

Datum
cardinality(PG_FUNCTION_ARGS){
    IntSet S = (IntSet)PG_GETARG_POINTER(0);

    PG_RETURN_INT32(S->set[0] - 1);
}

PG_FUNCTION_INFO_V1(superset);

Datum
superset(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);

    int *flag;
    int result = 1;

    for (int i = 1; i < B->set[0]; i++){
        flag = (int *)bsearch(&B->set[i], (A->set + 1), A->set[0] - 1, sizeof(int), compare);
        if (flag == NULL){ // Return false if existing at least elements in B, but not in A.
            result = 0;
            break;
        }
    }

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(subset);

Datum
subset(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);

    int *flag;
    int result = 1;

    for (int i = 1; i < A->set[0]; i++){
        flag = (int *)bsearch(&A->set[i], (B->set + 1), B->set[0] - 1, sizeof(int), compare);
        if (flag == NULL){ // Return false if existing at least elements in A, but not in B.
            result = 0;
            break;
        }
    }

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intset_equal);

Datum
intset_equal(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);
    int result = 1;

    for (int i = 0; i < A->set[0]; i++){
        if (A->set[i] != B->set[i]){ // Each element in both set should be equal to each other, or return false.
            result = 0;
            break;
        }
    }

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(not_equal);

Datum
not_equal(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);

    int result = 0;

    for (int i = 0; i < A->set[0]; i++){
        if (A->set[i] != B->set[i]){ // If any element is not equal to each other in corresponding index, then return true.
            result = 1;
            break;
        }
    }

    PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intersection);

Datum
intersection(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);
    IntSet result;

    int *set;          // Store the intersection elements.
    int set_index = 1; // Index 0 is reserved for the length of this set, other values (if exists) start from index 1.
    int set_size = 1;  // Index 0 is always used for the length, so the size must be at least 1.
    int index_A = 1;
    int index_B = 1;

    set = (int *)palloc(VARHDRSZ + sizeof(int) * set_size); // Allocate memory for this set, at least needs memory space for the index 0.

    while (index_A < A->set[0] && index_B < B->set[0]){ // Both A.set and B.set are sorted from index 1.
        if (A->set[index_A] < B->set[index_B]){
            index_A++;
        }
        else if (A->set[index_A] > B->set[index_B]){
            index_B++;
        }
        else{ // Two elements are equal to each other, then add to the set (result).
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = A->set[index_A];
            index_A++;
            index_B++;
        }
    }

    set[0] = set_index;

    result = (IntSet)palloc(VARHDRSZ + (sizeof(int) * set[0]));
    SET_VARSIZE(result, VARHDRSZ + (sizeof(int) * set[0]));

    for (int i = 0; i < set[0]; i++){
        result->set[i] = set[i];
    }

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(intset_union);

Datum
intset_union(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);
    IntSet result;

    int *set;
    int set_index = 1;
    int set_size = 1;
    int index_A = 1;
    int index_B = 1;

    set = (int *)palloc(VARHDRSZ + sizeof(int) * set_size);

    while (index_A < A->set[0] && index_B < B->set[0]){
        if (A->set[index_A] < B->set[index_B]){
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = A->set[index_A++];
        }
        else if (A->set[index_A] > B->set[index_B]){
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = B->set[index_B++];
        }
        else{ 
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = A->set[index_A];
            index_A++;
            index_B++;
        }
    }
    while (index_A < A->set[0]){
        // Add;
        if (set_index >= set_size){
            set_size *= 2;
            set = (int *)repalloc(set, sizeof(int) * set_size);
        }
        set[set_index++] = A->set[index_A++];
    }
    while (index_B < B->set[0]){
        // Add;
        if (set_index >= set_size){
            set_size *= 2;
            set = (int *)repalloc(set, sizeof(int) * set_size);
        }
        set[set_index++] = B->set[index_B++];
    }
    set[0] = set_index;

    result = (IntSet)palloc(VARHDRSZ + (sizeof(int) * set[0]));
    SET_VARSIZE(result, VARHDRSZ + (sizeof(int) * set[0]));

    for (int i = 0; i < set[0]; i++){
        result->set[i] = set[i];
    }

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(disjunction);

Datum
disjunction(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);
    IntSet result;

    int *set;
    int set_index = 1;
    int set_size = 1;
    int index_A = 1;
    int index_B = 1;

    set = (int *)palloc(VARHDRSZ + sizeof(int) * set_size);

    while (index_A < A->set[0] && index_B < B->set[0]){
        if (A->set[index_A] < B->set[index_B]){
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = A->set[index_A++];
        }
        else if (A->set[index_A] > B->set[index_B]){
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = B->set[index_B++];
        }
        else{
            index_A++;
            index_B++;
        }
    }
    while (index_A < A->set[0]){
        // Add;
        if (set_index >= set_size){
            set_size *= 2;

            set = (int *)repalloc(set, sizeof(int) * set_size);
        }
        set[set_index++] = A->set[index_A++];
    }
    while (index_B < B->set[0]){
        // Add;
        if (set_index >= set_size){
            set_size *= 2;
            set = (int *)repalloc(set, sizeof(int) * set_size);
        }
        set[set_index++] = B->set[index_B++];
    }

    set[0] = set_index;

    result = (IntSet)palloc(VARHDRSZ + (sizeof(int) * set[0]));
    SET_VARSIZE(result, VARHDRSZ + (sizeof(int) * set[0]));

    for (int i = 0; i < set[0]; i++){
        result->set[i] = set[i];
    }

    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(difference);

Datum
difference(PG_FUNCTION_ARGS){
    IntSet A = (IntSet)PG_GETARG_POINTER(0);
    IntSet B = (IntSet)PG_GETARG_POINTER(1);
    IntSet result;

    int *set;
    int set_index = 1;
    int set_size = 1;
    int index_A = 1;
    int index_B = 1;

    set = (int *)palloc(VARHDRSZ + sizeof(int) * set_size);

    while (index_A < A->set[0] && index_B < B->set[0]){
        if (A->set[index_A] < B->set[index_B]){
            // Add;
            if (set_index >= set_size){
                set_size *= 2;
                set = (int *)repalloc(set, sizeof(int) * set_size);
            }
            set[set_index++] = A->set[index_A++];
        }
        else if (A->set[index_A] > B->set[index_B]){
            index_B++;
        }
        else{
            index_A++;
            index_B++;
        }
    }
    while (index_A < A->set[0]){
        // Add;
        if (set_index >= set_size){
            set_size *= 2;
            set = (int *)repalloc(set, sizeof(int) * set_size);
        }
        set[set_index++] = A->set[index_A++];
    }
    set[0] = set_index;

    result = (IntSet)palloc(VARHDRSZ + (sizeof(int) * set[0]));
    SET_VARSIZE(result, VARHDRSZ + (sizeof(int) * set[0]));

    for (int i = 0; i < set[0]; i++){
        result->set[i] = set[i];
    }

    PG_RETURN_POINTER(result);
}
