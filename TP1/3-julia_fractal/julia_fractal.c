#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 

#define X_SIZE 4096
#define Y_SIZE 4096
#define IT_MAX 255
typedef struct {
  char *raster;
  double cx;
  double cy;
  int number;
  int nb_th;
}t_my_args;
// Prototypes
void* compute_set ( void* args);
long compute_point (double zx, double zy, double cx, double cy);
int read_seed (double *cx, double *cy);
void write_pgm( char *raster, char *name);

int main (int argc, char *argv[])
{
    char name [256];
    int nbthreads;
    double cx, cy;
    t_my_args* args;
    t_my_args cpy;
    pthread_t* threads;
    sscanf(argv[1], "%d", &nbthreads);
    cpy.nb_th = nbthreads;
    args = calloc(nbthreads, sizeof(t_my_args));
    threads = calloc(nbthreads, sizeof(pthread_t));
    while (read_seed(&cx, &cy))
    {
        cpy.cx = cx, cpy.cy = cy;
        cpy.raster = (char *) malloc (Y_SIZE * X_SIZE);
        for(int i = 0; i < nbthreads; i++){
            args[i] = cpy;
            args[i].number = i;
            pthread_create(&threads[i], NULL,compute_set, (void *)&args[i]);
        }
        for(int i = 0; i < nbthreads; i++)
            pthread_join(threads[i], NULL);
        sprintf (name, "julia_%f_%f.pgm", cpy.cx, cpy.cy);
        write_pgm(cpy.raster, name);
        free(cpy.raster);
    }
    return 0;
}
void* compute_set(void* args)
{
    t_my_args *my_args = (t_my_args*) args;
    char *raster = my_args->raster;
    double cx = my_args->cx, cy = my_args->cy;
    long x, y;
    double zx, zy;
    for (y = my_args->number; y < Y_SIZE; y+=my_args->nb_th)
    {
        zy = 4.0 * (double) y / (double) (Y_SIZE - 1) - 2.0;
        for (x = 0; x < X_SIZE; x++)
        {
            zx = 4.0 * (double) x / (double) (X_SIZE - 1) - 2.0;
            raster[y * X_SIZE + x ] = compute_point ( zx, zy, cx, cy);
        }
    }
    my_args->raster = raster;
    return NULL;
}

long compute_point (double zx, double zy, double cx, double cy)
{
    double zx_temp, zy_temp;
    long it = 0;
    while ((it < IT_MAX) && ((zx * zx) + (zy * zy) < 4.0))
    {
        zx_temp = zx * zx - zy * zy + cx;
        zy_temp = 2 * zx * zy + cy;
        zx = zx_temp;
        zy = zy_temp;
        it++;
    }
    return it;
}

int read_seed (double *cx, double *cy)
{
    if (scanf ("%lf %lf\n", cx, cy) == EOF)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void write_pgm( char *raster, char *name)
{
    FILE *fp;
    fp = fopen (name, "wb");
    fprintf (fp , "P5 %d %d %d\n", X_SIZE, Y_SIZE, IT_MAX);
    fwrite (raster , 1 , X_SIZE * Y_SIZE, fp);
    fclose (fp);
}