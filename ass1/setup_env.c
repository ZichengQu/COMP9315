#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef struct intset* IntSet;

struct intset{
    int* set; // internal representation of the IntSet // 要用这种方式
	int length; // 记录int* set里实际存储的数字的个数
    int size; // 记录int* set的malloc的size
}intset;

IntSet initialization(){
    IntSet intset = (IntSet)malloc(sizeof(struct intset));
    if(intset == NULL){
        // printf("memory error\n");
        return NULL;
    }

    intset->size = 5; // intset.set初始化时的size位5.
    intset->set = (int*)malloc(intset->size * sizeof(int)); 
    if(intset->set == NULL){
        // printf("memory error\n");
        return NULL;
    }
    intset->length = 0; // 记录intset.set的元素个数
    return intset;
}

void trim(char* token)
{
    int i,j=0;
    for(i=0;token[i]!='\0';i++)
	{
		if(token[i]!=' ')
			token[j++]=token[i];
	}
    token[j]='\0';
}

// int* convert_intset(IntSet intset, char* input){ // 将input转换成所需的intset的类型
//     const char end = '}'; // 终止符
//     int i = 0; // 记录input字符串数组中元素的下标
//     /*
//      * 目前存在问题，input某一个元素为10的话，会拆分成1和0去读入，因此需要通过逗号来正则将input合理分开！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//      */
//     // char *token;
//     // const char delim[2] = ",";
//     // input[strlen(input)] = '\0'; // 不同的OS可能对strcpy的优化不一样, 因此最好手动在最后补齐一个 \0
//     // token = strtok(input, delim);
//     // printf("%s\n",token);

//     char* inputs = malloc(sizeof(char) * strlen(input));
//     strcpy(inputs, input);
//     const char* delim = ",";
//     char *token;
//     char* s = malloc(sizeof(char) * 100);
//     /* 获取第一个子字符串 */
//     token = strtok(inputs, delim);
//     strcat(s+strlen(s), "{");
//     /* 继续获取其他的子字符串 */
//     while( token != NULL ) {
//         char src[2];
//         for(int i=0; i<strlen(token); i++){
//             if(token[i] == '{' || token[i] == '}'){
//                 token[i] = ' ';
//             }
//         }
//         trim(token);
        
//         // snprintf(result,1,"%s%s,",token);

//         printf( "Token: %s\ninput: %s\n\n", token, input);
        
//         strcpy(src,  ",");
//         strcat(s+strlen(s), token);
//         strcat(s+strlen(s), ",");
//         printf("%s\n", s);
        
//         // int flag = 1;
//         // int num = atoi(token); // 将该字符型数字转换成整型数字
//         // for(int i=0; i<intset->length; i++){
//         //     if(intset->set[i] == num){
//         //         flag = 0;
//         //         break;
//         //     }
//         // }
//         // // if(flag){
//         // //     intset->set[intset->length++] = num;
//         // //     // printf( "%d\n", intset->set[intset->length]);
//         // //     // intset->length++;
//         // // }
//         // if(flag){ // 如果不重复
//         //     intset->set[intset->length++] = num; 

//         //     if(intset->length >= intset->size){ // 若空间分配不足，则重新分配空间
//         //         intset->size *= 2;
//         //         intset->set = realloc(intset->set,intset->size*sizeof(int));
//         //         if(intset->set==NULL)
//         //         {
//         //             // printf("memory error\n");
//         //             return NULL;
//         //         }
//         //     }
//         // }

//         token = strtok(NULL, delim);
//     }
//     s[strlen(s)-1] = '}'; // 将最后一个','替换成'}'
//     printf("%s\n", s);

//     // while(input[i] != end){ // 当input中的该字符等于终止符的时候，跳出循环
//     //     if(input[i] >= '0' && input[i] <= '9'){
//     //         int flag = 1; // 判断是否重复，1不重复，0重复
//     //         /*
//     //          * 这里写判断是否重复的代码！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
//     //          */
//     //         if(flag){ // 如果不重复
//     //             intset->set[intset->length++] = input[i] - 48; // 将该字符型数字转换成整型数字

//     //             if(intset->length >= intset->size){ // 若空间分配不足，则重新分配空间
//     //                 intset->size *= 2;
//     //                 intset->set = realloc(intset->set,intset->size*sizeof(int));
//     //                 if(intset->set==NULL)
//     //                 {
//     //                     // printf("memory error\n");
//     //                     return NULL;
//     //                 }
//     //             }
//     //         }
//     //     }
//     //     i++;
//     // }
//     // if(intset->length > 0){
//     //     intset->set[intset->length] = 111;
//     // }
//     // return 0;
// }
char* convert_intset(char* input){ // 将input转换成所需的intset的类型

    char* inputs = (char*)malloc(sizeof(char) * (strlen(input)+1));
    
    const char* delim = ",";
    char *token;
    // char* s = (char*)palloc(VARHDRSZ + sizeof(char) * (strlen(input)+1));
    strcpy(inputs, input);
    /* 获取第一个子字符串 */
    token = strtok(inputs, delim);
    strcpy(inputs, "{");
    // strcat(inputs+strlen(inputs), "{");
    /* 继续获取其他的子字符串 */
    while( token != NULL ) {
        for(int i=0; i<strlen(token); i++){
            if(token[i] == '{' || token[i] == '}'){
                token[i] = ' ';
            }
        }
        trim(token);

        printf( "Token: %s\ninput: %s\n\n", token, inputs);

        strcat(inputs+strlen(inputs), token);
        strcat(inputs+strlen(inputs), ",");
        printf("%s\n", inputs);


        token = strtok(NULL, delim);
    }
    inputs[strlen(inputs)-1] = '}'; // 将最后一个','替换成'}'
    printf("%s\n", inputs);
    return inputs;
}

// void convert_intset(IntSet intset, char* str){ // 将input转换成所需的intset的类型
// 	const char* delim;
// 	char* input;
// 	char *token;

// 	delim = ","; //const char* delim = ",";
//     // input = palloc(VARHDRSZ + sizeof(char) * strlen(str));
//     input = malloc(sizeof(char) * strlen(input));
//     strcpy(input, str);

    
//     /* 获取第一个子字符串 */
//     token = strtok(input, delim);
    
//     /* 继续获取其他的子字符串 */
//     while( token != NULL ) {
// 		// int flag;
// 		// flag = 1;
        
//         for(int i=0; i<strlen(token); i++){
//             if(token[i] == '{' || token[i] == '}'){
//                 token[i] = ' ';
//             }
//         }
//         trim(token);
//         printf("Token: %s\n", token);
        
//         // for(int i=0; i<intset->length; i++){
//         //     if(intset->set[i] == token){
//         //         flag = 0;
//         //         break;
//         //     }
//         // }
//         // if(flag){ // 如果不重复
//         //     intset->set[intset->length++] = token; 
//         // }
//         token = strtok(NULL, delim);
//     }
// }

IntSet IntSet_in(char* input){
    IntSet intset = initialization();
    convert_intset(input);

    // strcpy(intset->set, set);
    // intset->set[strlen(set)] = '\0'; // 不同的OS可能对strcpy的优化不一样, 因此最好手动在最后补齐一个 \0

    return intset;
}

char* itoa(int num,char* str,int radix)
{
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
 
    char temp;//临时变量，交换两个值时用到
    for(j=k;j<=(i-1)/2;j++)//头尾一一对称交换，i其实就是字符串的长度，索引最大值比长度少1
    {
        temp=str[j];//头部赋值给临时变量
        str[j]=str[i-1+k-j];//尾部赋值给头部
        str[i-1+k-j]=temp;//将临时变量的值(其实就是之前的头部值)赋给尾部
    }
 
    return str;//返回转换后的字符串
 
}

int main(void){

    // int aaa[] = {1,2,3,4,5,666,7,8,9};
    // char* s = malloc(sizeof(char) * 100);
    // for(int i=0; i<9; i++){
    //     // printf("%d\n", aaa[i]);
    //     // itoa(999, s, 10);
    //     printf("len: %ld\n",strlen(s));
    //     printf("%s\n", itoa(aaa[i],(s+strlen(s)),10));
        
    //     // printf("QQQQQ: %s\n", s+i);
    //     char src[2];
    //     strcpy(src,  ",");
    //     // strcpy(dest, "This is destination");
    //     strcat(s+strlen(s)-1, src);
    //     // printf("WWWWWWW: %s\n", s+2*i);
    // }
    // printf("len total: %ld\n",strlen(s));
    // printf("这个字符: %s\n", s+strlen(s)-1);
    // s[strlen(s)-1] = '}'; // 将最后一个','替换成'}'
    // printf("%s\n", s);

    IntSet intset = NULL;
    char* input = "{9 , 10, 8, 9, 10, 3, 7, 6,5,4,3,2,1}"; //"{10, 9, 8, 7, 6,5,4,3,2,1}";

    intset = IntSet_in(input);

    printf("Total len: %d\n", intset->length);
    for(int i=0; i<intset->length; i++){
        printf("length: %d, value: %d\n", i, intset->set[i]);
    }
    
    return 0;
}

// int main(void){
//     int number = 0;
//     while (1) {
//         printf("请输入一个数字："); //Debug运行卡到这里时可能会报错，尽量不要在这里打断点。
//         if (scanf("%d", &number) == 1) {
//             printf("your input is %d\n", number);
//         }
//         if (number == 100) {
//             break;
//         }
//     }

//     return 0;
// }