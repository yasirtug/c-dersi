#include <stdio.h>
#include <stdlib.h>

#define STRING_BUFFER_SIZE 100000
#define MAX_EXPRESSION_COUNT 40
#define MAX_TAB_COUNT 10

struct exps{
    int exp_count;
    char** exps;
};

struct exps extract_expressions(char*);
char** extract_loop(char*);
char* file2string(char*);
FILE* lines2file(char**, char*, int*);
char* serialize_code(char*);
char is_for_loop(char*);
int operate(char*, char**, int, int*, int);
char* write(char*, char*);

/********************************************************************/
// this function takes a command line argument as a file location
// and reads it. after serializing it's data, it converts 'for' loops
// to 'while' loops and writes it to an output file.
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
    //printf("input.c file:%s\n\n", code);
    char* string = serialize_code(code);
    free(code);
    printf("serialized code: \n%s\n", string);

    char** lines = (char**) calloc(MAX_EXPRESSION_COUNT, sizeof(char*));
    int* tabs = (int*) calloc(MAX_EXPRESSION_COUNT, sizeof(int));

    operate(string, lines, 0, tabs, 0);

    lines2file(lines, "output.c", tabs);
}

/********************************************************************/
// this function recursively extracts expressions and saves lines.
int operate(char* string, char** lines, int line_cursor, int* tabs, int tab_count)
{
    struct exps expressions = extract_expressions(string);
    int exp_count = expressions.exp_count;
    char** exps = expressions.exps;

    char** loop;
    for(int i = 0; i < exp_count; ++i){
        if(is_for_loop(exps[i])){
            loop = extract_loop(exps[i]);
            
            tabs[line_cursor] = tab_count;
            lines[line_cursor++] = loop[0];
            
            tabs[line_cursor] = tab_count;
            lines[line_cursor++] = loop[1];
            tab_count++;
            if(loop[2][0]){
                line_cursor = operate(loop[2], lines, line_cursor, tabs, tab_count);                
            }
            else{
                i++;
                line_cursor = operate(exps[i] + 1, lines, line_cursor, tabs, tab_count);
            }
            tab_count--;
            tabs[line_cursor] = tab_count;
            lines[line_cursor++] = loop[3];
        }
        else if(exps[i][0] == '{'){
            tab_count++;
            line_cursor = operate(exps[i] + 1, lines, line_cursor, tabs, tab_count);
            tab_count--;
        }
        else{
            tabs[line_cursor] = tab_count;            
            lines[line_cursor++] = exps[i];
        }
    }
    return line_cursor;
}

/********************************************************************/
// this function extracts expressions of a serialized code and 
// returns a struct which contains expressions and expression count.
struct exps extract_expressions(char* D)
{
    char** exps = (char**)malloc(MAX_EXPRESSION_COUNT * sizeof(char*));
    int i = -1, exp_start = 0, exps_cursor = 0, l = 1, k = 0;
    char scope = 0, symmetric_scope = 0;
    char* exp = malloc(0);
    while(D[++i]){
        if(realloc(exp, ++l) == NULL){
                printf("Realloc fail.\n");
                exit(0);
        }
        exp[i - exp_start] = D[i];

        if(D[i] == '\'' || D[i] == '\"'){
            symmetric_scope = (symmetric_scope + 1) % 2;
        }
        if(symmetric_scope == 0){
            if(D[i] == '}' || D[i] == ')'){
                scope--;
            }
            if(scope == 0){
                if(D[i] == '{'){
                    exp[i - exp_start] = '\0';
                    exps[exps_cursor++] = exp;
                    exp = (char*) malloc(1);
                    exp[0] = '{';
                    l = 1;
                    exp_start = i;        
                }
                if(D[i] == '}' || D[i] == ';'){
                    exp[i - exp_start + 1] = '\0';
                    exps[exps_cursor++] = exp;
                    exp = (char*) malloc(0);
                    l = 1;
                    exp_start = i + 1;
                }
            }
            if(D[i] == '{' || D[i] == '('){
                scope++;
            }
        }
    }
    for(int i = 0; i < exps_cursor; ++i){
        printf("exp %d: %s\n", i, exps[i]);
    }
    struct exps expressions = {exps_cursor, exps};
    return expressions;
}

/********************************************************************/
// this function extracts loop into lines and returns them.
// here is contract of it's return type:
// return[0] -> "A;"
// return[1] -> "while(B)"
// return[2] -> "D" // as a block code {} or as just one line.
// return[3] -> "C;"

char** extract_loop(char* exp)
{
    char** loop = (char**) malloc(6 * sizeof(char*));
    char* A,* B,* C,* D;

    int scope = 0;
    int symmetric_scope = 0;
    char state_scope = '@';
    // '@' comes right before 'A' in ASCII

    int length_A = 0, length_B = 0, length_C = 0, length_D = 1;
    int sum_ABC;

    int i = -1;
    while(exp[++i]){
        // A, B, C counting part

        if(state_scope == 'A')
            length_A++;
        else if(state_scope == 'B')
            length_B++;
        else if(state_scope == 'C')
            length_C++;

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

    // below is D counting and writing part
    scope = 0;
    symmetric_scope = 0;
    while(exp[++i]){
        length_D++;
    }
    D = (char*) malloc(length_D);
    i -= length_D;

    for(int k = 0; k < length_D; ++k){
        i++;
        D[k] = exp[i];
    }
    A[length_A - 1] = '\0';
    B[length_B - 1] = '\0';
    C[length_C - 1] = '\0';
    D[length_D - 1] = '\0';

    char* temp_line;
    char* cursor;

    temp_line = (char*) malloc(length_A + 1);
    cursor = write(temp_line, A);
    write(cursor, ";");
    free(A);
    loop[0] = temp_line;
    
    temp_line = (char*) malloc(length_B + 7);
    cursor = write(temp_line, "while(");
    cursor = write(cursor, B);
    write(cursor, ")");
    free(B);
    loop[1] = temp_line;

    loop[2] = D;

    temp_line = (char*) malloc(length_C + 1);
    cursor = write(temp_line, C);
    write(cursor, ";");
    loop[3] = temp_line;

    //printf("\nA: %s\nB: %s\nC: %s\nD: %s\n", l.A, l.B, l.C, l.D);
    return loop;
}

/********************************************************************/
// this function copies chars from a char array into another char 
// array one by one and returns last copied address of the string.
char* write(char* string, char* source)
{
    int i = 0;
    do{
        string[i] = source[i];        
    } while(source[i++]);

    return string + i - 1;
}

/********************************************************************/
// this function extracts file content into a char array and 
// returns it.
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
    fclose(file);
    return string;
}

/********************************************************************/
// this function write lines into a file.
FILE* lines2file(char** lines, char* file_name, int* tabs)
{
    FILE* file = fopen(file_name, "w+");
    int i = -1;

    char* tab = (char*) malloc(MAX_TAB_COUNT * sizeof(char) + 1);
    char* temp_tab = tab;
    while(lines[++i]){
        tab = temp_tab;
        tab[0] = '\0';
        for(int k = 0; k < tabs[i]; ++k){
            tab = write(tab, "    ");
        }
        fprintf(file, "%s%s\n", temp_tab, lines[i]);
        printf("%s%s\n", temp_tab, lines[i]);
    }
    fclose(file);
}

/********************************************************************/
// this function serializes code. it converts all '\n's to spaces and
// deletes all unnecessary spaces.
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
// this function checks if an expression is a for loop or not.
// if an expression starts with "for(" or "for (", it returns
// 1. else, it returns 0.
char is_for_loop(char* exp)
{
    if(exp[0] == 'f' && exp[1] == 'o' && exp[2] == 'r' && 
                (exp[3] == '(' || (exp[3] == ' ' && exp[4] == '('))){
        return 1;
    }
    return 0;
}