#include <stdio.h>
#include <stdlib.h>

#define STRING_BUFFER_SIZE 100000
#define MAX_EXPRESSION_COUNT 40
#define LOOP_EXPRESSION_SIZE 40

struct loop{
    char* A;
    char* B;
    char* C;
    char* D;
};

struct exps{
    int exp_count;
    char** exps;
};

struct exps extract_expressions(char*);
struct loop extract_loop(char*);
char* file2string(char*);
char* serialize_code(char*);
char is_for_loop(char*);

/********************************************************************/

int main(int argc, char** args)
{
    if(args[1] == NULL){
        printf("Please pass a file location to the program.\n");
        return 0;
    }
    char* code = file2string(args[1]);
    if(code == NULL){
        free(code);
        return 0;
    }
    printf("input.c file:\n%s\n\n", code);
    char* a =(serialize_code(code), serialize_code(code), serialize_code(code), serialize_code(code));
    char* string = serialize_code(code);
    free(code);
    printf("serialized code: \n%s\n\n", string);

    struct exps expressions = extract_expressions(string);
    free(string);
    for(int i = 0; i < expressions.exp_count; ++i){
        printf("%s\n", expressions.exps[i]);
    }
}

/********************************************************************/

struct exps extract_expressions(char* D)
{
    char** exps = (char**)malloc(0);
    int i = -1, exp_start = 0, exp_length = 0, exps_cursor = 0, alloc_size = 0;
    char counting = 1, scope = 0;
    char* exp;
    while(D[++i]){ 
        if(counting){
            exp_length++;
        }
        else{
            exp[i - exp_start] = D[i];
        }
        if(D[i] == '{' || D[i] == '('){
            scope++;
        }
        if(D[i] == '}' || D[i] == ')'){
            scope--;
        }
        if(scope == 0){
            if(D[i] == '}' || D[i] == ';'){
                if(counting){
                    counting = 0;
                    exp = (char*)malloc(exp_length + 1);
                    i -= exp_length;
                }
                else{
                    counting = 1;
                    exp[i - exp_start + 1] = 0;
                    exp_start = i + 1;
                    exps = (char**)realloc(exps, alloc_size + 8);
                    if(exps == NULL){
                        printf("Reallocate failed.");
                        exit(0);// probably leak happened
                    }
                    alloc_size += 8;
                    exps[exps_cursor++] = exp;
                    exp_length = 0;
                }
            }
        }
    }
    struct exps expressions = {exps_cursor, exps};
    return expressions;
}

/********************************************************************/

struct loop extract_loop(char* exp)
{
    struct loop l;
    int i = -1, p_scope = 0;// parantheses scope
    int length = 0;
    char scope = 0;
    while(exp[++i]){
        if(exp[i] == '('){
            if(scope == 0){
                scope = 'A';
            }

        }
    }
    return l;
}

/********************************************************************/

char* file2string(char* location)
{
    FILE* file = fopen(location,"r");
    char* string = (char*) malloc(STRING_BUFFER_SIZE);
    if(file == NULL){
       printf("The file is not accessible.");
       return NULL;
    }
    int i = 0;
    while(i < STRING_BUFFER_SIZE){
        if(fscanf(file, "%c", string + i) == EOF){
            string[i] = 0;
            break;
        }
        i++;
    }
    return string;
}

/********************************************************************/

char* serialize_code(char* code)
{
    char* string = (char*) calloc(STRING_BUFFER_SIZE, 1);
    int i = -1, cursor = 0;
    char last_char = ' ';
    while(code[++i]){
        if(code[i] == '\n'){
            code[i] = ' ';            
        }
        switch(last_char){
            case ' ':
            case ';':
            case '{':
            case '}':
            case '(':
            case ')':
                if(code[i] == ' ')
                    continue;
        }
        last_char = code[i];
        string[cursor] = code[i];
        cursor++;
    }
    string[i] = 0;
    return string;
}

/********************************************************************/

char is_for_loop(char* exp)
{
    if(exp[0] == 'f' && exp[1] == 'o' && exp[2] == 'r' && 
                (exp[3] == '(' || (exp[3] == ' ' && exp[4] == '('))){
        return 1;
    }
    return 0;
}