#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#define N_MAX 10000000LL
#define PI 3.14159265

typedef struct {
  int thread_id;
} thread_args_t;

int sine_value[3];

void *sine_producers (void *thread_arg)
{
    thread_args_t *my_args = (thread_args_t *)(thread_arg);
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 40 * 0.001 * phase + my_args->thread_id;
        sine_value[my_args->thread_id%3] = (int)(amplitude * sin(x));
    }
    
    return NULL;
}

void *sine_writers (void *thread_arg)
{
    thread_args_t *my_args = (thread_args_t *)(thread_arg);
    int nb_write = 0;
    FILE *file = NULL;
    char filename[20] = {'\0'};
    
    sprintf(filename, "sine_%d.txt", my_args->thread_id);
    file = fopen (filename , "w");
    if (file == NULL)
    {
        printf("ERROR while opening file\n");
    }
    
    for (nb_write = 0; nb_write < N_MAX; ++nb_write)
    {
        fprintf(file, "%d\t%d\n", nb_write, sine_value[my_args->thread_id%3]);
    }
    
    fclose(file);
    
    return NULL;
}

int main (int argc, char **argv)
{
    int n_threads;
    int rc;
    pthread_t *my_threads;
    thread_args_t *my_args;
    void *thread_return;
    
    n_threads = 6;
    
    my_threads = calloc(n_threads, sizeof(pthread_t));
    my_args = calloc(n_threads, sizeof(thread_args_t));
    
    for (int i = 0; i < n_threads; i++)
    {
        my_args[i].thread_id = i;
        if (i%2 == 0)
            pthread_create(&my_threads[i], NULL, sine_producers, (void *)&my_args[i]);
        else
            pthread_create(&my_threads[0], NULL, sine_writers, (void *)&my_args[i]);
    }
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    free (my_threads);

    return (0);
}