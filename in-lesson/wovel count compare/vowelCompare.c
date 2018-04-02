#include <stdio.h>
int getLen(char* str){
    int length = 0;
    while(str[length] != 0)
    {
        length++;
    }
    return length;
}
int findVowelCount(char* str){
    int length = getLen(str);
    int vowelCount = 0;
    for(int i = 0; i < length; ++i){
        switch(str[i]){
            case 'a':
            case 'u':
            case 'e':
            case 'o':
            case 'i':
            case 'A':
            case 'U':
            case 'E':
            case 'O':
            case 'I':
            vowelCount++;
            break;
        }
    }
    return vowelCount;
}
int main(int argc, char** args){
    char *first = args[1];
    char *second = args[2];
    printf("vowel count of first: %d\nvowel count of second: %d\n",
    findVowelCount(first),findVowelCount(second));
    int vowelCountOfFirst = findVowelCount(first);
    int vowelCountOfSecond = findVowelCount(second);

    if(vowelCountOfFirst > vowelCountOfSecond){
        printf("%s\n", first);
    }
    else if(vowelCountOfSecond > vowelCountOfFirst){
        printf("%s\n", second);
    }
    else{
        printf("%s","both have the same vowel count\n");
    }
}