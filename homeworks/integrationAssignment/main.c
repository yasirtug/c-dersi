#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
name: Ahmet Yasir Tug
assignment: 2
file name: main.c
*/

struct term
{
    int exp;//exponent
    int coef;//coefficient
};
// we can represent every term with these 2 variables

int calculateTermCount(char*);
char isNumber(char);
int extractTheTerms(char*, struct term*);
double calculateIntegration(struct term*, int, int, int, int, char);

/*------------------------------------------------------------------*/

// this function takes command-line arguments and prints the 
// numerical integration calculation to the stdio stream.
int main(int pc, char** parameters)
{
    char* polynomial = parameters[1];
    int startingPoint = atoi(parameters[2]);
    int endingPoint = atoi(parameters[3]);
    int intervalCount = atoi(parameters[4]);
    // we are taking third, fourth and fifth parameters, converting
    // these to integers and assigning to variables.
    char method = parameters[5][0];
    // we need the first char of sixth parameter to recognize method
    double integration;
    int termCount = calculateTermCount(polynomial);
    struct term terms[termCount];
    // we are calculating term count and creating a termCount sized
    // array. 
    if(extractTheTerms(polynomial, terms) == -1){
    // we are extracting terms into 'terms' array and controlling
    // the return value of function.
        printf("program terminated due to exception\n");
        return 0;
    }
    /*
    for(int i = 0; i < termCount; i++){
        printf("%d. terim:\n", i);
        printf("katsayi: %d\n", terms[i].coef);
        printf("us: %d\n", terms[i].exp);
    }
    */
    integration = calculateIntegration(terms, termCount, 
                startingPoint, endingPoint, intervalCount, method);
    printf("%lf\n", integration);
}

/*------------------------------------------------------------------*/

// this function takes polynomial string and returns count of terms
// in it.
int calculateTermCount(char* polynomial)
{
    int termCount = 1;
    // there will be at least one term

    int i = 1;
    // we starting cursor from 1 because if the first char is a sign 
    // char, it isn't increasing the term count
    while(polynomial[i] != 0){
        // if string continues, if i'th char does exists
        if(polynomial[i] == '-' || polynomial[i] == '+'){
            termCount++;
            // if we have a sign char, then term count must increase
        }
        i++;
        // we will look to next char on next iteration
    }
    // after this while loop, we must have term count
    return termCount;
}

/*------------------------------------------------------------------*/

// this function controls if given char parameter is an ascii digit
// or not. if it is an ascii digit, it returns 1. else, it returns 0.
char isNumber(char item)
{
    if(item < 48 || item > 57){
        //then it is not an ascii number
        return 0;
    } else{
        //then it is an ascii number
        return 1;
    }
}

/*------------------------------------------------------------------*/

// this function extracting terms to given term array by operating on
// given polynomial string. it returns 0 if it ends successfully.
// it can return some error codes too.
int extractTheTerms(char* polynomial, struct term* terms){
    int cursor = 0;// for browsing in the string
    int termCursor = 0;// we will write into this term

    char expIn = 0;// if term has an exponential sign
    char xIn = 0;// if term has an x
    char coefIn = 0; // if term has an coefficient

    int lastSign = 1; // last sign
    int lastCoef = 0; // last coefficient
    int lastExp = 0; // last exponential

    while(polynomial[cursor]){
        // in this loop, we are browsing on the polynomial string 
        // and operating for the chars.
        switch(polynomial[cursor]){
            case 'x':
                xIn = 1;
            break;

            case '^':
                expIn = 1;
            break;

            case '+':
            case '-':
            // when '+' or '-' comes, we assume the job with the
            // the previous term is done. then we save the 
            // coefficient and the exponential into the terms
            // array and prepare variables for the next term.

                if(coefIn == 0){
                    lastCoef = 1;
                }
                // if there is no coefficient, then coefficient is 1

                if(expIn == 0){
                    lastExp = 1;
                }
                // if there is no exponential, then exponential is 1

                if(xIn == 0){
                    lastExp = 0;
                }
                // if there is no 'x', then exponential is 0. 
                // in other words, the term is a fixed number.

                terms[termCursor].coef = lastCoef * lastSign;
                terms[termCursor].exp = lastExp;
                // we are writing the properties into the term

                lastSign = 44 - polynomial[cursor];
                // we are getting the sign as coefficient.
                // if we are on '-' which is 45 in ascii, then
                // lastSign will be -1.
                // if we are on '+' which is 43 in ascii, then 
                // lastSing will be +1

                lastCoef = 0;
                lastExp = 0;
                expIn = 0;
                xIn = 0;
                coefIn = 0;
                // we are setting variables to their default values
                // in order to get program work properly.

                if(cursor != 0){
                    termCursor++;
                }
                // if we are on the first char of the polynomial 
                // string and it is a sign char, we must not 
                // increase the termCount.
            break;

            default:
                if(isNumber(polynomial[cursor])){
                // if the char is an ascii number
                    if(expIn == 1){
                    // if we know the term has an exponential,
                    // then we know we are currently at
                    // digits which are after exponential symbol
                        lastExp *= 10;
                        lastExp += polynomial[cursor] - 48;
                    // we get the number from digits one by one
                    } else{
                        coefIn = 1;
                    // if we are not know if the term has exponential
                    // or not, then we must looking at the digits of
                    // the coefficient. then coefIn is true
                        lastCoef *= 10;
                        lastCoef += polynomial[cursor] - 48;
                    // we get the number from digits one by one
                    }
                }
                else{
                    // this is unexpected, we are printing a message 
                    // and returning with -1 error code.
                    printf("Unexpected char: %c\n", 
                                        polynomial[cursor]);
                    return -1;
                }
            break;
            }
        cursor++;
    }


    if(coefIn == 0){
        lastCoef = 1;
    }
    if(expIn == 0){
        lastExp = 1;
    }
    if(xIn == 0){
        lastExp = 0;
    }
    terms[termCursor].coef = lastCoef * lastSign;
    terms[termCursor].exp = lastExp;
    // we were doing these operations after every term except 
    // for the last. these are for the last term because there 
    // is no sign after this term.
    return 0;
    // zero means function is ended up successfully.
}

/*------------------------------------------------------------------*/

// this function calculates integration for a term array with given
// parameters. it can calculate numerical integration with 4 methods.
// it returns result of calculation.
double calculateIntegration(struct term* terms, int termCount, 
        int start, int end, int intervalCount, char method){

    double integration = 0; // result of the integration
    double h; // length of the interval
    double a; // starting point
    double b; // ending point
    double fa = 0; // f(a)
    double fb = 0; // f(b)
    double fab = 0; // f((a + b) / 2.0)
    switch(method){
        // we will do the process for given method. the method
        // will be given to us by only a char.
        // you can look these methods from that link:
        // pages.cs.wisc.edu/~amos/412/lecture-notes/lecture18.pdf

        // RECTANGLE
        case 'R': 
        case 'r':
            h = (double) (end - start) / intervalCount;
            a = start;
            for(int i = 0; i < intervalCount; ++i){
                //for every interval

                for(int ti = 0; ti < termCount; ++ti){
                    fa += terms[ti].coef * pow(a, terms[ti].exp);
                    // we are calculating for every term
                }
                integration += fa * h;
                fa = 0;
                a += h;
                // we will calculate for a + h on the next step
            }
        break;
        
        // MIDPOINT
        case 'M':
        case 'm': 
            h = (double) (end - start) / intervalCount;
            a = start + h / 2;
            // this method differs from rectangle method with
            // calculating the integration at the mid point between 
            // a and b. we achive this by assigning the mid point of
            // the first interval to the a. then a + h * i will be
            // the mid point of i'th interval.
            for(int i = 0; i < intervalCount; ++i){

                for(int ti = 0; ti < termCount; ++ti){
                    fa += terms[ti].coef * pow(a, terms[ti].exp);
                }
                integration += fa * h;
                fa = 0;
                a += h;
            }
        break;


        // TRAPEZOID
        case 'T':
        case 't':
            h = (double) (end - start) / intervalCount;
            b = start + h;
            // on trapezoid rule, we need both starting points and 
            // ending points of the intervals. but we don't need to 
            // calculate both points every time. at each interval,
            // we will calculate for the ending points and use the
            // previous ending point calculation as starting point
            // calculation. so we will calculate only for b, which 
            // is ending point at each interval.
            for(int ti = 0; ti < termCount; ++ti){
                fa += terms[ti].coef * pow(b - h, terms[ti].exp);
            }
            // we must calculate f(a) for the first interval, once.
            for(int i = 0; i < intervalCount; ++i){
                for(int ti = 0; ti < termCount; ++ti){
                    fb += terms[ti].coef * pow(b, terms[ti].exp);
                }
                integration += ((fa + fb) / 2) * h;
                fa = fb;
                // f(b) of this interval will be f(a) for the next
                // interval.
                fb = 0;
                b += h;
            }
        break;

        // SIMPSON'S
        case 'S':
        case 's':
            h = (double) (end - start) / intervalCount;
            b = start + h;
            for(int ti = 0; ti < termCount; ++ti){
                fa += terms[ti].coef * pow(b - h, terms[ti].exp);
            }
            for(int i = 0; i < intervalCount; ++i){
                for(int ti = 0; ti < termCount; ++ti){
                    fb += terms[ti].coef * pow(b, terms[ti].exp);
                    fab += terms[ti].coef * pow((b - h / 2.0), 
                                                    terms[ti].exp);
                    // calculation of this method is just likes to 
                    // trapezoid rule, but needs an extra calculation 
                    // for mid point of every interval.
                }
                integration += (fa + 4 * fab + fb) * (h / 6.0);       
                fa = fb;
                fb = 0;
                fab = 0;
                b += h;
            }
        break;

        default:
        // if the method is none of them, function returns with 
        // an error code.
        return -1;
    }
    return integration;
}
