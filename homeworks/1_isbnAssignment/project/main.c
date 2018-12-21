#include <stdio.h>
#include <stdlib.h>

/*
name: Ahmet Yasir Tug
assignment: 1
file name: main.c
*/

int get_len(char const*);
int compute_d1(char const*);
char* add_digit(char const*, int);

//this function solves(if it is solvable) the problem
//for given parameter. it prints d1 and final number.
//if problem is not solvable for given parameter, it 
//prints messages to stdio stream.
int main(int paramCount, char const **parameters)
{
    char const *string, *full_number;
    int d_one;

    if (paramCount < 2){
        printf("Please run program with a parameter.\n");
        return 0;
    }
    string = parameters[1];
    d_one = compute_d1(string);
    full_number = add_digit(string, d_one);
    if(d_one < 0){
    //d_one function returns -1 or -2 for exceptions.
        printf("Program is terminated due to exception.\n");
        return 0;
    }
    else if(d_one == 10){
    //this is an exception, there is a case that we can't
    //make the number divisible by 11. in this case, d_one
    //is 10
        printf("Problem is not solvable.\n");
    } else{
        printf("D1: %d\n", d_one);
        printf("Full number: %s", full_number);
    }
    return 0;
}

/*------------------------------------------------------------------*/

//function for calculating given string length.
int get_len(char const *code)
{
    int length = 0;
    while(code[length] != 0){
    //string has not ended yet
        length++;
    }
    return length;
}

/*------------------------------------------------------------------*/

//this function calculates the d1 value for given string. 
//string must contain an integer which has 9 digits.
int compute_d1(char const *code)
{
    int sum = 0, d_one, cursor, digit, multiplier;

    if (get_len(code) != 9){
        //this function working for only 9 digit integers.
        printf("Parameter must be a 9 digit integer.\n");
        return -1;
    }

    for(cursor = 0; cursor < 9; cursor++){
        digit = code[cursor] - 48;
        //we are subtracting 48 from code[cursor] because we want
        //corresponding number of ascii char.
        if(digit < 0 || digit > 9){
            //digit must be between 0 and 9
            printf("Parameter must be an integer.\n");
            return -2;
        }
        multiplier = 10 - cursor;
        //this gives us the multiplier of the digit.
        sum += digit * multiplier;
        //after multiplying these, we get number to add to sum.
    }
    
    d_one = 11 - (sum % 11);
    //sum % 11 gives us the overflowing value.
    //if we subtract it from 11, then we will get d1.
    if(d_one == 11){
    //if sum % 11 equals to zero, there is an exception.
        d_one = 0;
    }
    return d_one;
}

/*------------------------------------------------------------------*/

//this function adds given number to the end of given string.
char* add_digit(char const *code, int number)
{
    int size = get_len(code);
    char* combined_code = malloc(size + 2);
    //we are allocating size + 2 space because we
    //need +1 space for d1 and +1 space for the null char
    //which will be at the end of the string.
    for(int i = 0; i < size; i++){
        combined_code[i] = code[i];
    }
    //we are simply copying code array into combined_code.
    combined_code[size] = (char) number + 48;
    //we are adding our number as ascii char at the
    //end of the string.
    combined_code[size + 1] = 0;
    //strings must end with a zero.
    return combined_code;
}