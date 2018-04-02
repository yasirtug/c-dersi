#include <stdio.h>
int getLen(char* str){
    int length = 0;
    while(str[length]){
        length++;
    }
    return length;
}
char* sort(char* str){
    char mem;
    int length = getLen(str);
    char devam = 0;
    for(int i = 1; i < length; ++i){
        for(int k = i; k > -2; --k){
            if(str[k] > str[i]){
                devam = 1;
            }
            if((str[k] < str[i] || k == -1) && devam == 1){
                devam = 0;
                printf("i:%dk:%d\n",i,k);
                mem = str[i];
                for(int j = i; j > k; --j){
                    str[j] = str[j - 1];
                }
                str[k + 1] = mem;
            }
        }
    }
    return str;
}
int main(int argc, char** args){
    int mem,length;
    printf("%s", sort(args[1]));
}