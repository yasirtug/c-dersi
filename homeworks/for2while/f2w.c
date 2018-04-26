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
    //printf("input.c file:\n%s\n\n", code);
    char* a =(serialize_code(code), serialize_code(code), serialize_code(code), serialize_code(code));
    char* string = serialize_code(code);
    free(code);
    printf("serialized code: \n%s\n\n", string);

    struct exps expressions = extract_expressions(string);
    free(string);
    for(int i = 0; i < expressions.exp_count; ++i){
        printf("%s\n", expressions.exps[i]);
    }
    for(int i = 0; i < expressions.exp_count; ++i){
        if(is_for_loop(expressions.exps[i])){
            extract_loop(expressions.exps[i]);
        }
    }
}

/********************************************************************/

struct exps extract_expressions(char* D)
{
    char** exps = (char**)malloc(0);
    int i = -1, exp_start = 0, exp_length = 0, exps_cursor = 0, alloc_size = 0;
    char counting = 1, scope = 0, symmetric_scope = 0;
    char* exp;
    while(D[++i]){ 
        if(counting){
            exp_length++;
        }
        else{
            exp[i - exp_start] = D[i];
        }
        if(D[i] == '\'' || D[i] == '\"'){
            symmetric_scope = (symmetric_scope + 1) % 2;
        }

        if(symmetric_scope == 1)
            continue;

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
    char* A,* B,* C,* D;
    l.A = A;
    l.B = B;
    l.C = C;
    l.D = D;

    int scope = 0;
    int symmetric_scope = 0;
    char state_scope = '@';
    // '@' comes right before 'A' in ASCII

    int length_A = 0, length_B = 0, length_C = 0;
    int sum_ABC;

    int i = -1;
    while(exp[++i]){
        // A, B, C counting part
        if(exp[i] == '\"' || exp[i] == '\''){
            symmetric_scope = (symmetric_scope + 1) % 2;
        }
        if(symmetric_scope == 0){
            if(exp[i] == '('){
                scope++;
            }
            if(scope == 1){
                if(exp[i] == ';' || exp[i] == '(' || exp[i] == ')'){
                    state_scope++;
                }
            }
            if(exp[i] == ')'){
                scope--;
                if(scope == 0)
                    break;
            }
        }
        
        if(state_scope == 'A')
            length_A++;
        else if(state_scope == 'B')
            length_B++;
        else if(state_scope == 'C')
            length_C++;
    }
    // at that point, A, B and C are one more bigger that 
    // their actual lengths. that is good, because we were
    // about to allocate memory for them and we will need
    // one more space for null char which will be at the
    // end of the each string.
    A = (char*) malloc(length_A);
    B = (char*) malloc(length_B);
    C = (char*) malloc(length_C);

    // A, B, C writing part

    sum_ABC = length_A + length_B + length_C;
    // this gives actual sum + 3
    i -= sum_ABC;
    // and now this exp[i] points to paranthesis after 'for'
    for(int k = 0; k < length_A; ++k){
        i++;
        A[k] = exp[i];
    }
    for(int k = 0; k < length_B; ++k){
        i++;
        B[k] = exp[i];
    }
    for(int k = 0; k < length_C; ++k){
        i++;
        C[k] = exp[i];
    }
    A[length_A - 1] = '\0';
    B[length_B - 1] = '\0';
    C[length_C - 1] = '\0';
    printf("\nA: %s\nB: %s\nC: %s\n\n", A, B, C);

    scope = 0;
    symmetric_scope = 0;
    while(exp[++i]){
        if(exp[i] == '{')
            scope++;
        if(exp[i] == '\'' || exp[i] == '\"')
            symmetric_scope = (symmetric_scope + 1) % 2;
        if(exp[i] == ';' || exp[i] == '}'){

        }
        if(exp[i] == '}')
            scope--;
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




/*
struct loop extract_loop(char* exp)
{
    struct loop l;

    int scope = 0;
    char* A,* B,* C,* D;
    char* current;
    char state = '0';
    int state_length = 0;
    char reading = 0;
    char counting = 1;
    int cursor;
    int i = -1;
    while(exp[++i]){
        if(state == 'D'){
            break;
        }
        if(exp[i] == '('){
            scope++;
        }
        else if(exp[i] == ')'){
            scope--;
        }
        if(reading){
            if(exp[i] == ';' || (scope == 1 && exp[i] == ')')){
                reading = 0;
                counting = 1;
                current[i - cursor] = '\0';
                state++;
                state_length = 0;
                continue;
            }
            if(state == 'A'){
                A[i - cursor] = exp[i];
                current = A;
                continue;
            }
            if(state == 'B'){
                B[i - cursor] = exp[i];
                current = B;
                continue;
            }
            if(state == 'C'){
                C[i - cursor] = exp[i];
                current = C;
                continue;
            }
        }
        if(counting){
            if(exp[i] == '('){
                if(state == '0'){
                    state = 'A';
                    continue;
                }
            }
            if(exp[i] == ';' || (scope == 1 && exp[i] == ')')){
                reading = 1;
                counting = 0;
                i -= state_length + 1;
                cursor = i + 1;
                switch(state){
                    case 'A':
                        A = (char*) malloc(state_length + 1);
                        break;
                    case 'B':
                        B = (char*) malloc(state_length + 1);
                        break;
                    case 'C':
                        C = (char*) malloc(state_length + 1);
                        break;
                }
                continue;
            }
            if(state >= 'A' && state <= 'C'){
                state_length++;
                continue;
            }
        }
    }
    printf("A:%s\n",A);
    printf("B:%s\n",B);
    printf("C:%s\n",C);
    printf("Scope:%d\n",scope);
    
    return l;
}
*/