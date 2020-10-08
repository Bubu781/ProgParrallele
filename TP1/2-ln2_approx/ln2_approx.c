#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#define N_MAX 1000000000LL

typedef struct {
  double sum;
  int nb_th;
  int numero;
}t_my_args;

int n_thread;

void* loop(void*thread_args){
  t_my_args* my_args = (t_my_args*) thread_args;
  double sum = 0;
  for(long long n = N_MAX - my_args->numero; n > 0; n -= my_args->nb_th){
    if (n % 2 == 0)
      sum -= 1.0 / (double)n;
    else
      sum += 1.0 / (double)n;
  }
  my_args->sum = sum;
  return NULL;
}

int main (int argc, char *argv[])
{
  t_my_args* my_args;
  int nbthreads;
  pthread_t *threads;
  void* thread_return;
  double sum = 0;

  sscanf(argv[1], "%d", &nbthreads);
  my_args = calloc(nbthreads, sizeof(t_my_args));
  threads = calloc(nbthreads, sizeof(pthread_t));

  for(int i = 0; i < nbthreads; i++){
    my_args[i].sum = 0.0;
    my_args[i].nb_th = nbthreads;
    my_args[i].numero = i;
    pthread_create(&threads[i], NULL,loop, (void *)&my_args[i]);
  }

  for(int i = 0; i < nbthreads; i++)
    pthread_join(threads[i], &thread_return);
  
  for(int i = 0; i < nbthreads; i++)
    sum += my_args[i].sum;
  
  printf ("sum: %.12f\n",sum);
  return 0;
}