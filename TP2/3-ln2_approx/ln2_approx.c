#include <stdio.h>

#define N_MAX 1000000000LL

int main (void)
{
    double sum = 0.0;
    long long n;

    for (n = N_MAX; n > 0; n--)
    {
        if (n%2 == 0)
        {
            sum -= 1.0 / (double )n;
        }
        else
        {
            sum += 1.0 / (double ) n ;
        }
    }
    printf ("sum: %.12f\n" , sum) ;
    return 0;
}