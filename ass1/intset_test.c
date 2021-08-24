#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <stdbool.h>
#include <regex.h>

typedef struct intset *IntSet;

struct intset
{
    int *set;   // internal representation of the IntSet // 要用这种方式
    int length; // 记录int* set里实际存储的数字的个数
    int size;   // 记录int* set的malloc的size
} intset;

IntSet initialization()
{
    IntSet intset = (IntSet)malloc(sizeof(struct intset));
    if (intset == NULL)
    {
        // printf("memory error\n");
        return NULL;
    }

    intset->size = 5; // intset.set初始化时的size位5.
    intset->set = (int *)malloc(intset->size * sizeof(int));
    if (intset->set == NULL)
    {
        // printf("memory error\n");
        return NULL;
    }
    intset->length = 0; // 记录intset.set的元素个数
    return intset;
}

void trim(char *inputs)
{
    int i, j = 0;
    for (i = 0; inputs[i] != '\0'; i++)
    {
        if (inputs[i] != ' ')
            inputs[j++] = inputs[i];
    }
    inputs[j] = '\0';
}

char *convert_intset(char *input)
{ // 将input转换成所需的intset的类型

    // char* inputs = (char*)palloc(VARHDRSZ + sizeof(char) * (strlen(input)+1));
    char *inputs = (char *)malloc(sizeof(char) * (strlen(input) + 1));

    // const char* delim = ",";
    char *token;
    // char* result = (char*)palloc(VARHDRSZ + sizeof(char) * (strlen(inputs)+1));
    char *result = (char *)malloc(sizeof(char) * (strlen(inputs) + 1));
    strcpy(inputs, input);
    /* 获取第一个子字符串 */
    token = strtok(inputs, ",");
    strcat(result + strlen(result), "{");
    printf("Token: %s\ninput: %s\n\n", token, inputs);

    /* 继续获取其他的子字符串 */
    while (token != NULL)
    {
        for (int i = 0; i < strlen(token); i++)
        {
            if (token[i] == '{' || token[i] == '}')
            {
                token[i] = ' ';
            }
        }
        trim(token);

        printf("Token: %s\ninput: %s\n\n", token, inputs);

        strcat(result + strlen(result), token);
        strcat(result + strlen(result), ",");
        printf("%s\n", result);

        token = strtok(NULL, ",");
    }
    result[strlen(result) - 1] = '}'; // 将最后一个','替换成'}'
    printf("%s\n", result);
    return result;
}

// void trim(char* inputs) // 待删除
// {
//     int i,j=0;
//     for(i=0;inputs[i]!='\0';i++)
// 	{
// 		if(inputs[i]!=' ')
// 			inputs[j++]=inputs[i];
// 	}
//     inputs[j]='\0';
// }

void abstract(char *inputs)
{
    int i, j = 0;
    char *element = strtok(inputs, ",");

    // ========================================================
    // token = strtok(inputs, ",");
    // strcat(result+strlen(result), "{");
    // printf( "Token: %s\ninput: %s\n\n", token, inputs);

    // /* 继续获取其他的子字符串 */
    // while( token != NULL ) {
    //     for(int i=0; i<strlen(token); i++){
    //         if(token[i] == '{' || token[i] == '}'){
    //             token[i] = ' ';
    //         }
    //     }
    //     trim(token);

    //     printf( "Token: %s\ninput: %s\n\n", token, inputs);

    //     strcat(result+strlen(result), token);
    //     strcat(result+strlen(result), ",");
    //     printf("%s\n", result);

    //     token = strtok(NULL, ",");
    // }
    // result[strlen(result)-1] = '}'; // 将最后一个','替换成'}'
    // printf("%s\n", result);
    // return result;
}

// bool is_valid(char* inputs){
//     // trim(inputs);
//     printf("trim之后: %s\nstrlen: %ld\n", inputs, strlen(inputs));
//     // char * re = "^(?!\\{((?:[0-9]+,[0-9]+)*|\\d+)\\}).*";
//     // char * re = "^\\s*{\\s*\\d\\s*(\\s*,\\s*\\d+\\s*)*}|{\\s*}$";
//     char* re = "[0-9]+";

// 	if(!regexMatch(inputs, re)) {
// 		return false;
// 	}
// 	return true;
// }

void make_set(char *input)
{
    char *result = (char *)malloc(sizeof(char) * (strlen(input) + 1));
    // strchr()
    // for
}

// function used to compile the regrex and execute it
static int regexMatch(char *str, const char *regexPattern)
{
    regex_t regex;
    int match = false;
    // compile the regex
    if (regcomp(&regex, regexPattern, REG_EXTENDED))
    {
        return false;
    }
    // execute the regex
    if (regexec(&regex, str, 0, NULL, 0) == 0)
    {
        match = true;
    }
    // free the regex
    regfree(&regex);
    return match;
}

int is_valid(char *inputs)
{
    // trim(inputs);
    // printf("trim之后: %s\nstrlen: %ld\n", inputs, strlen(inputs));
//  char  *pattern = "^[ ]*\\{[ ]*[0-9]+([ ]*,[ ]*[0-9]+[ ]*)*\\}[ ]*$|\\{[ ]*\\}";
    const char *re = "^[ ]*\\{[ ]*[0-9]+([ ]*,[ ]*[0-9]+[ ]*)*\\}[ ]*$|\\{[ ]*\\}";//$";//\\}[ ]*$";		// 正则表达式
    printf("该字符串: %s\n", inputs);
    if (!regexMatch(inputs, re)){
        return 0;
    }
    return 1;
}

int main(void){
    // IntSet intset = NULL;
    char *input = "{0}";

// '{ }'
// '{2,3,1}'
// '{6,6,6,6,6,6}'
// '{10, 9, 8, 7, 6,5,4,3,2,1}'
// '{1, 999, 13, 666, 5}'
// '{    1  ,  3  ,  5 , 7,9 }'
// '{1, 01, 001, 0001}'  (same as '{1}')
// '{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}'
// '{1,2,3,4,5,6,7,8,9,10,(and then all numbers to),9999,10000}'
// '  {1,5,7,9}'
// '{2,4,6, 8  }  '
// '{0}'
    char *inputs = (char *)malloc(sizeof(char) * (strlen(input) + 1));
    strcpy(inputs, input);
    if (is_valid(inputs) == 0){
        printf("匹配不成功\n");
    }else{
        printf("匹配成功\n");
    }

    
    return 0;

    // intset = IntSet_in(input);

    // printf("Total len: %d\n", intset->length);
    // for(int i=0; i<intset->length; i++){
    //     printf("length: %d, value: %d\n", i, intset->set[i]);
    // }

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