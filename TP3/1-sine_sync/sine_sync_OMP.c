#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int sine_value = 0;
omp_lock_t locker1;
omp_lock_t locker2;
void sine_producer (int tid)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    int val;
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 0.04 * phase;
        val = (int)(amplitude * sin(x));
        omp_set_lock(&locker1);
        sine_value = val;
        omp_unset_lock(&locker2);
    }
}

void sine_writer (int tid)
{
    int nb_write = 0;
    FILE *file = NULL;
    file = fopen ("sine.txt" , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {
        omp_set_lock(&locker2);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        omp_unset_lock(&locker1);
    }
    
    fclose(file);
    
}

int main (int argc, char **argv)
{
    int n_threads;
    int rc;
    void *thread_return;
    int tid;
    n_threads = 2;
    omp_init_lock(&locker2);
    omp_init_lock(&locker1);
    omp_set_lock(&locker2);
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        if(tid==0) sine_writer(tid);
        if(tid==1) sine_producer(tid);
    }
    omp_destroy_lock(&locker1);
    omp_destroy_lock(&locker2);
    return (0);
}
