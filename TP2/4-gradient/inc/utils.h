#ifndef UTILS_H
#define UTILS_H

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#define DEBUG(fmt...) do { \
        fprintf(stderr, "[%s(%s):%d] : ", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, fmt); \
        fprintf(stderr, "\n"); \
        fflush(stderr); \
    } while(0)

#define DIE(fmt...) do {  \
        DEBUG(fmt); \
        exit(1); \
    } while(0)

#define TRACE() do { \
        DEBUG(" "); \
    } while (0)

#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)<(b)) ? (b) : (a))
#define MAP(x, x0, x1, y0, y1) (y0 + ((y1 - y0) * (x - x0) + (x1 - x0)/2) / (x1 - x0))
#define LIMIT(x, x0, x1) (MIN(MAX(x, x0), x1))
#define ABS(a) (((a)>=0)?(a):(-(a)))

float fmap(float x, float x0, float x1, float y0, float y1);

#endif