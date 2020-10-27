#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#define N_MAX 10000000LL
#define PI 3.14159265

omp_lock_t *writers;
omp_lock_t *producers;
int sine_value = 0;
int n_threads = 4;
void sine_producer ()
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 40 * 0.001 * phase;
        for(int i = 0; i < n_threads - 1; i++)
            omp_set_lock(&writers[i]);
        sine_value = (int)(amplitude * sin(x));
        for(int i = 0; i < n_threads - 1; i++)
            omp_unset_lock(&producers[i]);
    }
    
}

void sine_writers (int tid)
{
    int nb_write = 0;
    FILE *file = NULL;
    char filename[20] = {'\0'};
    
    sprintf(filename, "sine_%d.txt", tid);
    file = fopen (filename , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {
        omp_set_lock(&producers[tid]);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        omp_unset_lock(&writers[tid]);
    }
    
    fclose(file);
}

int main (int argc, char **argv)
{
    int tid;
    writers = calloc(n_threads - 1, sizeof(omp_lock_t));
    producers = calloc(n_threads - 1, sizeof(omp_lock_t));
    for(int i = 0; i < n_threads - 1; i++){
        omp_init_lock(&writers[i]);
        omp_init_lock(&producers[i]);
        omp_set_lock(&producers[i]);
    }
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        if (tid == (n_threads - 1))
            sine_producer();
        else if(tid < n_threads)
            sine_writers(tid);
    }

    for(int i = 0; i < n_threads - 1; i++){
        omp_destroy_lock(&writers[i]);
        omp_destroy_lock(&producers[i]);
    }

    return (0);
}