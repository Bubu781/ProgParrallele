#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N_MAX 1000000000LL

int main (int argc, char **argv){
  double sum = 0.0;
  int tid, nbthreads;
  #pragma omp parallel private(tid)
  {
    long long n;
    double my_sum = 0.0;
    tid = omp_get_thread_num();
    nbthreads = omp_get_num_threads();
    for (n = N_MAX - tid; n > 0; n-= nbthreads)
    {
      if (n % 2 == 0)
        my_sum -= 1.0 / (double)n;
      else
        my_sum += 1.0 / (double)n;
    }
    sum += my_sum;
  }
  printf ("sum: %.12f\n",sum);
}
