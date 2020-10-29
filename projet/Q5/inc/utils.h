#ifndef UTILS_H
#define UTILS_H
/**
 * @file utils.h
 * @brief A few utilities for debug
 * @author Etienne HAMELIN
 * @version 0.1
 * @date october 2020
 */

/**
 * Output messages on the stderr stream, with file & line number to ease tracing.
 * Compile with the -DNDEBUG flag to disable debugging output.
 */
#ifdef NDEBUG
#define DEBUG(fmt...) 
#else
#define DEBUG(fmt...) do { \
        fprintf(stderr, "[%s(%s):%d] : ", __FILE__, __FUNCTION__, __LINE__); \
        fprintf(stderr, fmt); \
        fprintf(stderr, "\n"); \
        fflush(stderr); \
    } while(0)
#endif

/**
 * Output some message, then quit
 */
#define DIE(fmt...) do {  \
        DEBUG(fmt); \
        exit(1); \
    } while(0)

/**
 * Just trace that we've run through this file/line
 */
#define TRACE() do { \
        DEBUG(" "); \
    } while (0)

/**
 * A few useful macros. Use with care.
 */
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#define MAX(a,b) (((a)<(b)) ? (b) : (a))
#define MAP(x, x0, x1, y0, y1) (y0 + ((y1 - y0) * (x - x0) + (x1 - x0)/2) / (x1 - x0))
#define LIMIT(x, x0, x1) (MIN(MAX(x, x0), x1))
#define ABS(a) (((a)>=0)?(a):(-(a)))

#endif