#include <pthread.h>
#include <stdio.h>

#define N_MAX 1000000000LL
#define THREAD_MAX 10

void *thread_for (void *sum);

int main(void)
{
    pthread_t ids [THREAD_MAX];
    double sums [THREAD_MAX];
    int i;
    double sum = 0.0;
    for (i = 0; i < THREAD_MAX; i++)
    {
        sums [i] = (double) i;
        pthread_create (&ids[i], NULL, thread_for, (void *)&sums[i]);
    }
    for (i = THREAD_MAX - 1; i >= 0; i--)
    {
        pthread_join (ids[i], NULL);
        sum += sums [i];
    }
    printf ("sum: %.12f\n" , sum);
    return 0;
}

void *thread_for (void *sum)
{
    long long n, n_min, n_max;
    n_min = N_MAX / THREAD_MAX * *(double *)sum;
    n_max = N_MAX / THREAD_MAX * (*(double *)sum + 1) ;
    *(double *)sum = 0.0 ;
    for (n = n_max ; n > n_min ; n--)
    {
        if (n % 2 == 0)
        {
            *(double*)sum -= 1.0 / (double )n;
        }
        else
        {
            *(double *)sum += 1.0 / (double )n;
        }
    }
    return (void *)0;
}