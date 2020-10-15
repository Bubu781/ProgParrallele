#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // usleep

#ifndef N
    #define N 10000000
#endif

#define PADDING 64

int main(int argc, char **argv) 
{
    volatile int a[PADDING];
    volatile int b[PADDING];
    volatile int x[PADDING];
    volatile int y[PADDING];

    int results[4] = {0, 0, 0, 0};
    

    for (int i = 0; i < N; i++)
    {
        a[0] = 0;
        b[0] = 0;
        x[0] = 0;
        y[0] = 0;

        #pragma omp parallel shared(a, b, x, y) num_threads(2)
        {
        /* Start parallel region */
            #pragma omp sections
            {
                #pragma omp section
                {
                    a[0] = 1;
                    x[0] = b[0];
                }
                #pragma omp section
                {
                    b[0] = 1;
                    y[0] = a[0];
                }
            }

      } /* end of parallel section */

      ++results[x[0] + 2*y[0]];

    }

    for (int vx = 0; vx < 2; vx++) {
        for (int vy = 0; vy < 2; vy++) {
            printf("(x = %d, y = %d) : %9d times (%.3f%%)\n", vx, vy, results[vx + 2*vy], (100.0d*results[vx + 2*vy])/N);
        }
    }
        
}