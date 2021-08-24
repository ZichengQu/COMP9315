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

#include "postgres.h"

#include "fmgr.h"

PG_MODULE_MAGIC;

typedef struct intset *IntSet;

struct intset
{
	int length; // 记录int* set里实际存储的数字的个数
	int size;	// 记录int* set的palloc的size
	char set[FLEXIBLE_ARRAY_MEMBER];	// internal representation of the IntSet // 要用这种方式
} intset;

IntSet initialization(void);
void trim(char *token);
void convert_intset(IntSet intset, char* input);
char* itoa(int num,char* str,int radix);


IntSet initialization()
{
	IntSet intset = (IntSet)palloc(VARHDRSZ + sizeof(struct intset));
	if (intset == NULL)
	{
		// printf("memory error\n");
		return NULL;
	}

	// intset->size = 5; // intset.set初始化时的size位5.
	// intset->set = (int *)palloc(intset->size * sizeof(int));
	// if (intset->set == NULL)
	// {
	// 	// printf("memory error\n");
	// 	return NULL;
	// }
	// intset->length = 0; // 记录intset.set的元素个数
	return intset;
}

void trim(char *token)
{
	int i, j = 0;
	for (i = 0; token[i] != '\0'; i++)
	{
		if (token[i] != ' ')
			token[j++] = token[i];
	}
	token[j] = '\0';
}

void convert_intset(IntSet intset, char* input){ // 将input转换成所需的intset的类型
	const char* delim;
	char* inputs;
	char *token;

	delim = ","; //const char* delim = ",";
    // const char end = '}'; // 终止符
    //int i = 0; // 记录input字符串数组中元素的下标
    /*
     * 目前存在问题，input某一个元素为10的话，会拆分成1和0去读入，因此需要通过逗号来正则将input合理分开！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
     */
    // char *token;
    // const char delim[2] = ",";
    // input[strlen(input)] = '\0'; // 不同的OS可能对strcpy的优化不一样, 因此最好手动在最后补齐一个 \0
    // token = strtok(input, delim);
    // printf("%s\n",token);

    inputs = palloc(VARHDRSZ + sizeof(char) * strlen(input));
    strcpy(inputs, input);
    
    
    
    /* 获取第一个子字符串 */
    token = strtok(inputs, delim);
    
    /* 继续获取其他的子字符串 */
    while( token != NULL ) {
		int num;
		int flag;
		flag = 1;
        
        for(int i=0; i<strlen(token); i++){
            if(token[i] == '{' || token[i] == '}'){
                token[i] = ' ';
            }
        }
        trim(token);
        // printf( "%s\n", token );
        
        num = atoi(token);
        for(int i=0; i<intset->length; i++){
            if(intset->set[i] == num){
                flag = 0;
                break;
            }
        }
        if(flag){ // 如果不重复
            intset->set[intset->length++] = num; 

            // if(intset->length >= intset->size){ // 若空间分配不足，则重新分配空间
            //     intset->size *= 2;
            //     intset->set = repalloc(intset->set,intset->size*sizeof(int));
            //     if(intset->set==NULL)
            //     {
            //         // printf("memory error\n");
            //         return;
            //     }
            // }
        }
        token = strtok(NULL, delim);
    }
}

char* itoa(int num,char* str,int radix)
{
	char temp;//临时变量，交换两个值时用到
    char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";//索引表
    unsigned unum;//存放要转换的整数的绝对值,转换的整数可能是负数
    int i=0,j,k;//i用来指示设置字符串相应位，转换之后i其实就是字符串的长度；转换后顺序是逆序的，有正负的情况，k用来指示调整顺序的开始位置;j用来指示调整顺序时的交换。
 
    //获取要转换的整数的绝对值
    if(radix==10&&num<0)//要转换成十进制数并且是负数
    {
        unum=(unsigned)-num;//将num的绝对值赋给unum
        str[i++]='-';//在字符串最前面设置为'-'号，并且索引加1
    }
    else unum=(unsigned)num;//若是num为正，直接赋值给unum
 
    //转换部分，注意转换后是逆序的
    do
    {
        str[i++]=index[unum%(unsigned)radix];//取unum的最后一位，并设置为str对应位，指示索引加1
        unum/=radix;//unum去掉最后一位
 
    }while(unum);//直至unum为0退出循环
 
    str[i]='\0';//在字符串最后添加'\0'字符，c语言字符串以'\0'结束。
 
    //将顺序调整过来
    if(str[0]=='-') k=1;//如果是负数，符号不用调整，从符号后面开始调整
    else k=0;//不是负数，全部都要调整
 
    //char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }
 
    return str;//返回转换后的字符串
 
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/
PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS){
	char *str = PG_GETARG_CSTRING(0);
	char *input = (char*)palloc(strlen(str)+1);
	
	// IntSet intset = initialization();
	IntSet intset = (IntSet)palloc(VARHDRSZ + sizeof(struct intset));
	snprintf(input, strlen(str)+1, "%s", str);
	//convert_intset(intset, input);
	snprintf(intset->set,strlen(input)+1,"%s",input);
	PG_RETURN_POINTER(intset); // 老师：A pointer of value wrapped in a datum，这个会造成问题，需要自己去看macros for making sure that you use the right one.老师的week3的exercise的slides里讲了这点的原因。
}

PG_FUNCTION_INFO_V1(intset_out);

Datum
	intset_out(PG_FUNCTION_ARGS)
{
	IntSet intset = (IntSet)PG_GETARG_POINTER(0);//(IntSet *)PG_GETARG_POINTER(0);
	//char* result = palloc(VARHDRSZ + sizeof(char) * (intset->length*2 + 2 - 1)); //乘以2是因为数组和逗号一对，加2是因为两个{}，减一是因为最后一个数组不需要逗号
	int len=VARSIZE_ANY_EXHDR(intset);
	char* result = (char*)palloc(VARHDRSZ + sizeof(char) * len);
    snprintf(result, len, "%s", intset->set);
	// for(int i=0; i<intset->length; i++){
	// 	char src[2];
	// 	itoa(intset->set[intset->length],(result+strlen(result)),10);
    //     // printf("%s\n", itoa(intset->set[intset->length],(result+strlen(result)),10));       
    //     // printf("QQQQQ: %s\n", s+i);
    //     // char src[2];
    //     strcpy(src,  ",");
    //     // strcpy(dest, "This is destination");
    //     strcat(result+strlen(result)-1, src);
    //     // printf("WWWWWWW: %s\n", s+2*i);
    // }
    // // printf("len total: %ld\n",strlen(s));
    // // printf("这个字符: %s\n", s+strlen(s)-1);
    // result[strlen(result)-1] = '}'; // 将最后一个','替换成'}'
    // // printf("%s\n", s);

	



	// char *result;


	// result = psprintf("(%g,%g)", IntSet->x, IntSet->y);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * New Operators
 *
 * A practical IntSet datatype would provide much more than this, of course.
 *****************************************************************************/

// PG_FUNCTION_INFO_V1(IntSet_add);

// Datum
// 	IntSet_add(PG_FUNCTION_ARGS)
// {
// 	IntSet *a = (IntSet *)PG_GETARG_POINTER(0);
// 	IntSet *b = (IntSet *)PG_GETARG_POINTER(1);
// 	IntSet *result;

// 	result = (IntSet *)palloc(sizeof(IntSet));
// 	result->x = a->x + b->x;
// 	result->y = a->y + b->y;
// 	PG_RETURN_POINTER(result);
// }