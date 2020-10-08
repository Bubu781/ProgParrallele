#include <pthread.h>
#include <stdio.h>

#define N_MAX 1000000000LL
#define THREAD_MAX 10

void *thread_for (void *sum);

int main(void)
{
    pthread_t ids [THREAD_MAX];
    double sums [THREAD_MAX * 64];
    int i;
    double sum = 0.0;
    for (i = 0; i < THREAD_MAX; i++)
    {
        sums [i * 64] = (double) i;
        pthread_create (&ids[i], NULL, thread_for, (void *)&sums[i * 64]);
    }
    for (i = THREAD_MAX - 1; i >= 0; i--)
    {
        pthread_join (ids[i], NULL);
        sum += sums [i * 64];
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

/*
[?02/?10/?2020 15:25]  HAMELIN Etienne:  
si toutes les cases utilisées sont dans la même ligne de cache, alors on va faire des cache invalidate à chaque écriture
lorsque C0 écrit sur la ligne (dans son cache L1), ça invalide la ligne dans le L1 de C1, C2, C3 ; donc quand C1 ira lire le même bloc, cache miss !
à chaque fois, ça fait un miss dans le L1 et L2, et hit dans le L3 qui est partagé par les coeurs
 
[?02/?10/?2020 15:26]  HAMELIN Etienne:  
alros que si les variables sont séparées d'au moins 128 octets, on est sûrs qu'ils sont dans des lignes de cache différentes
et les accès de l'un n'invalident pas les accès pour l'autre coeur
et tout le monde peut travailler dans son L1*
 
*/