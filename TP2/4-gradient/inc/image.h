#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pixel.h"

/*
        switch(self->type)
        {
        case IMAGE_BITMAP:
            break;
        case IMAGE_GRAYSCALE_8:
            break;
        case IMAGE_GRAYSCALE_16:
            break;
        case IMAGE_GRAYSCALE_FL:
            break;
        case IMAGE_RGB_888:
            break;
        default:
            DIE("Unsupported");
        }
*/


typedef enum 
{
    IMAGE_BITMAP, /* 1bit, i.e. 2-level black&white image */
    IMAGE_GRAYSCALE_8, /* 8bit, i.e. 256-levels grayscale */
    IMAGE_GRAYSCALE_16, /* 16bit, i.e. 65636-levels grayscale */
    IMAGE_GRAYSCALE_FL, /* floating-point grayscale */
    IMAGE_RGB_888 /* 3-channel (RGB), 256-levels per channel, color image */
} image_type_t;

#define FOR_X(self, x_index) \
    for (int (x_index) = 0; (x_index) < self->width; ++(x_index))

#define FOR_Y(self, y_index) \
    for (int (y_index) = 0; (y_index) < self->height; ++(y_index))

#define FOR_YX(self, y_index, x_index) \
    FOR_Y(self, y_index) \
        FOR_X(self, x_index) 


/** 
 * \struct image_t
 * \brief image object ; this class holds an image metadata and pixels
 */
typedef struct image_s
{
    image_type_t type;
    int width;
    int height;
    uint8_t *data;
    void (*setpixel)(struct image_s *self, int x, int y, color_t c);
    color_t (*getpixel)(struct image_s *self, int x, int y);
} image_t;

// constructors
image_t *image_new(int width, int height, image_type_t type);
image_t *image_new_from_mem(int width, int height, image_type_t type, void *mem);

// destructors
void image_delete(image_t *self);

// output to console
void image_print_details(const image_t *self);
void image_print_ascii(const image_t *self);

// methods
uint8_t image_coord_check(const image_t *self, int x, int y);

void image_bmp_setpixel(const image_t *self, int x, int y, color_t c);
color_t image_bmp_getpixel(const image_t *self, int x, int y);

void image_gs8_setpixel(const image_t *self, int x, int y, color_t c);
color_t image_gs8_getpixel(const image_t *self, int x, int y);

void image_gs16_setpixel(const image_t *self, int x, int y, color_t c);
color_t image_gs16_getpixel(const image_t *self, int x, int y);

void image_gsfl_setpixel(const image_t *self, int x, int y, color_t c);
color_t image_gsfl_getpixel(const image_t *self, int x, int y);

void image_rgb_setpixel(const image_t *self, int x, int y, color_t c);
color_t image_rgb_getpixel(const image_t *self, int x, int y);

void image_clear(const image_t *self);

#endif