#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <math.h>

#define N_MAX 10000000LL
#define PI 3.14159265

int n_threads = 6;
int sine_value[3];
omp_lock_t *writers;
omp_lock_t *producers;
void sine_producers (int tid)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 40 * 0.001 * phase + tid;
        omp_set_lock(&writers[tid%3]);
        sine_value[tid%3] = (int)(amplitude * sin(x));
        omp_unset_lock(&producers[tid%3]);
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
        omp_set_lock(&producers[tid%3]);
        fprintf(file, "%d\t%d\n", nb_write, sine_value[tid%3]);
        omp_unset_lock(&writers[tid%3]);
    }
    
    fclose(file);
}

int main (int argc, char **argv)
{
    int tid;
    writers = calloc(n_threads/2, sizeof(omp_lock_t));
    producers = calloc(n_threads/2, sizeof(omp_lock_t));
    for(int i = 0; i < n_threads/2; i++){
        omp_init_lock(&writers[i]);
        omp_init_lock(&producers[i]);
        omp_set_lock(&producers[i]);
    }
    #pragma omp parallel private(tid)
    {
        tid = omp_get_thread_num();
        if (tid%2 == 0)
            sine_producers(tid);
        else
            sine_writers(tid);
    }
    
    for(int i = 0; i < n_threads/2; i++){
        omp_destroy_lock(&writers[i]);
        omp_destroy_lock(&producers[i]);
    }

    return (0);
}