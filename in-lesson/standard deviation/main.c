#include <stdio.h>
#include <math.h>

double calcPopulationDev(int*, int);
double calcSampleDev(int*, int);
int findSum(int*, int);

int main()
{
    int n;
    printf("enter the size of the array:\n");
    scanf("%d", &n);
    int array[n];
    printf("enter the elements of the array:\n");
    for(int i = 0; i < n; ++i){
        scanf("%d", array + i);
    }
    double popDeviation = calcPopulationDev(array, n);
    double sampleDeviation = calcSampleDev(array, n);

    printf("population deviation: %lf\nsample deviation: %lf\n", popDeviation, sampleDeviation);
}
int findSum(int* array, int n){
    int sum = 0;
    for(int i = 0; i < n; ++i){
        sum += array[i];
    }
    return sum;
}
double calcPopulationDev(int* array, int n)
{
    double sum = 0;
    double result;
    double u = (double) findSum(array, n) / n;

    for(int i = 0; i < n; ++i){
        sum += (array[i] - u) * (array[i] - u);
    }
    result = sqrt(sum / n);
    return result;
}
double calcSampleDev(int* array, int n)
{
    double sum = 0;
    double result;
    double u = (double) findSum(array, n) / n;

    for(int i = 0; i < n; ++i){
        sum += (array[i] - u) * (array[i] - u);
    }
    result = sqrt(sum / (n - 1));
    return result;
}