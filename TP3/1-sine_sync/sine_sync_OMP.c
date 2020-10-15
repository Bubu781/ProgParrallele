#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int sine_value = 0;
int actual = 0;
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
        while(actual == tid); 
        sine_value = val;
        actual = tid;
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
        while(actual == tid);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        actual = tid;
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
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        if(tid==0) sine_writer(tid);
        if(tid==1) sine_producer(tid);
    }

    return (0);
}
