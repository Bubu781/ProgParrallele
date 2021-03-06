#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 10000000LL
#define PI 3.14159265

typedef struct {
  int thread_id;
} thread_args_t;

int sine_value = 0;

// Création du tableau de sémaphore
sem_t semaphore_write[3];
sem_t semaphore_produce[3];

void *sine_producer (void *thread_arg)
{
    int phase = 0;
    int amplitude = 10000;
    float x = 0;
    
    for (phase = 0; phase < N_MAX; ++phase)
    {
        x = 40 * 0.001 * phase;
        for(int i = 0; i < 3 ; i++)
            sem_wait(&semaphore_write[i]);
        sine_value = (int)(amplitude * sin(x));
        for(int i = 0; i < 3 ; i++)
            sem_post(&semaphore_produce[i]);
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
        sem_wait(&semaphore_produce[my_args->thread_id]);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
        sem_post(&semaphore_write[my_args->thread_id]);
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

    // Initialisation des sémaphore
    for (int i = 0 ; i < 3 ; i++)
    {
        sem_init(&semaphore_write[i], PTHREAD_PROCESS_SHARED, 0);
        sem_init(&semaphore_produce[i], PTHREAD_PROCESS_SHARED, 1);
    }
    
    n_threads = 4;
    
    my_threads = calloc(n_threads, sizeof(pthread_t));
    my_args = calloc(n_threads, sizeof(thread_args_t));
    
    for (int i = 0; i < n_threads; i++)
    {
        my_args[i].thread_id = i;
        if (i == (n_threads - 1))
            pthread_create(&my_threads[i], NULL, sine_producer, (void *)&my_args[i]);
        else
            pthread_create(&my_threads[i], NULL, sine_writers, (void *)&my_args[i]);
    }
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    for (int i = 0 ; i < 3 ; i++)
    {	
        sem_destroy(&semaphore_write[i]);
	    sem_destroy(&semaphore_produce[i]);
    }
    free (my_threads);

    return (0);
}