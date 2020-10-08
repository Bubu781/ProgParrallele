#include <stdio.h>
#include <pthread.h>

long actual = 0;
/*thread function definition*/
void *threadFunction(void* args)
{
    while(actual != (long) args);
    printf("Thread %ld\n", (long) args);
    actual++;
    
    return NULL;
}

int main(int argc, char **argv)
{
    // Creating thread ids
    pthread_t thread_id[20];
    int ret = 0;
    long loop = 0;
    
    for (loop = 0; loop < 10; ++loop)
    {
        // Creating threads
        ret = pthread_create(&thread_id[loop],NULL, &threadFunction, (void *)loop);
        if (ret)
        {
            printf("Error during thread creation !\n");
            return (-1);
        }
    }
    
    // Waiting end of every threads
    for (loop = 0; loop < 10; ++loop)
        pthread_join (thread_id[loop], NULL);
    
    return 0;
}
