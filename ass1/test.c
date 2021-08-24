#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct intset* IntSet;

struct intset{
    int* set; // internal representation of the IntSet // 要用这种方式
	int length; // 记录int* set里实际存储的数字的个数
}intset;

IntSet initialization(){
    IntSet intset = (IntSet)malloc(sizeof(struct intset));
    if(intset == NULL){
        // printf("memory error\n");
        return NULL;
    }

    intset->set = (int*)malloc(100 * sizeof(int)); 
    if(intset->set == NULL){
        // printf("memory error\n");
        return NULL;
    }
    intset->length = 0; // 记录intset.set的元素个数
    return intset;
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
//     char *family_name1=NULL;
//     char *given_name1=NULL;
//     family_name1=strtok_r(input,",",&given_name1);
//     // token = strtok_r(input, delim);
//     printf("%s\n%s\n", family_name1, given_name1);
    
//     /* 继续获取其他的子字符串 */
//     while( token != NULL ) {
// 		int flag;
// 		flag = 1;
        
//         // for(int i=0; i<strlen(token); i++){
//         //     if(token[i] == '{' || token[i] == '}'){
//         //         token[i] = ' ';
//         //     }
//         // }
//         // trim(token);
//         printf("Token: %s\n", token);
        
//         for(int i=0; i<intset->length; i++){
//             if(intset->set[i] == atoi(token)){
//                 flag = 0;
//                 break;
//             }
//         }
//         if(flag){ // 如果不重复
//             intset->set[intset->length++] = atoi(token); 
//         }
//         token = strtok(NULL, delim);
//     }
// }

void trim(char *input)
{
    int i, j = 0;
    for (i = 0; input[i] != '\0'; i++)
    {
        if (input[i] != ' ' && input[i] != '{' && input[i] != '}')
            input[j++] = input[i];
    }
    input[j] = '\0';
}

void convert_intset(IntSet intset, char *str)
{ // 将input转换成所需的intset的类型
    const char *delim;
    char *input;
    char *token;

    delim = ","; //const char* delim = ",";
    // input = palloc(VARHDRSZ + sizeof(char) * strlen(str));
    // input = (char *)palloc(VARHDRSZ + sizeof(char) * (strlen(str) + 1));
    input = malloc(sizeof(char) * strlen(input));
    // token = (char*)palloc(VARHDRSZ + sizeof(char) * (strlen(str)+1));
    // strcpy(input, str);
    memcpy(input, str, (strlen(str) + 1));
    // input = psprintf("%s", str);
    /* 获取第一个子字符串 */
    token = strtok(input, delim);
    // token = psprintf("%s", strtok(input, delim));

    /* 继续获取其他的子字符串 */
    while (token != NULL)
    {
        int flag = 1;
        token[strlen(token)] = '\0';
        printf("%s\n",token);
        for (int i = 0; i < intset->length; i++)
        {
            if (intset->set[i] == atoi(token))
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        { // 如果不重复
            intset->set[intset->length++] = atoi(token);
            // sscanf(token, "%d", &intset->set[intset->length++]);
            // sscanf(atoi(token), "%d", &intset->set[intset->length++]);
        }
        token = strtok(NULL, delim);
    }
}
 
int main(void)
{
    char* input = "{1,01,001,0001 ,2,3,2}";
    // char* input = "{}";
    char* inputs = malloc(sizeof(char) * strlen(input));
    strcpy(inputs, input);
    trim(inputs);
    printf("%s\n",inputs);
	IntSet intset = initialization();
    convert_intset(intset, inputs);
    for(int i=0; i< intset->length; i++){
        printf("%d, %d\n", i, intset->set[i]);
    }
 
	return 0;
}