#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define N_MAX 10000000LL

int sine_value = 0;    

// Création du sémaphore
sem_t semaphore1;
sem_t semaphore2;

void *sine_producer (void *thread_arg)
{
    int phase = 0;
    int amplitude = 10000;
    int val;
    for (phase = 0; phase < N_MAX; ++phase)
    {

        val = (int)(amplitude * sin(0.040 * phase));
        sem_wait(&semaphore1);
        sine_value = val;
        sem_post(&semaphore2);
    }
    return NULL;
}

void *sine_writer (void *thread_arg)
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
        sem_wait(&semaphore2);
        fprintf(file, "%d\t%d\n", nb_write, sine_value);
		sem_post(&semaphore1);
    }    
    fclose(file);
    return NULL;
}

int main (int argc, char **argv)
{
    int n_threads;
    int rc;
    pthread_t *my_threads;
    void *thread_return;

	// Initialisation du sémaphore
    sem_init(&semaphore1, PTHREAD_PROCESS_SHARED, 1);
    sem_init(&semaphore2, PTHREAD_PROCESS_SHARED, 0);
    
    n_threads = 2;
    my_threads = calloc(n_threads, sizeof(pthread_t));
    
    pthread_create(&my_threads[0], NULL, sine_writer, NULL);
    pthread_create(&my_threads[1], NULL, sine_producer, NULL);
    
    for (int i = 0; i < n_threads; i++)
    {
        rc = pthread_join(my_threads[i], &thread_return);
        if (rc < 0)
            printf("pthread_join ERROR !!! (%d)\n", rc);
    }
    
    //RELACHATION DES DONNEES 
    free (my_threads);
	sem_destroy(&semaphore1);
	sem_destroy(&semaphore2);

    return (0);
}
