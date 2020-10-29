/**
 * @file image.c
 * @brief Basic image processing library: image container types and pixel accessors
 * @author Etienne HAMELIN
 * @version 0.1
 * @date october 2020
 */

#include "image.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>

/**
 * @fn image_t *image_new(int width, int height, int channels, int depth)
 * @brief Image constructor; creates an empty space for image pixels.
 * @param width image width
 * @param height image height
 * @param type image type (pixel format)
 * @return Handle of a new image object, NULL if creation fails.
 * Don't forget to call object destructor when you're finished using this image object: image_delete(image_handle);
 */
image_t *image_new(int width, int height, image_type_t type)
{
    image_t *self;
    void *mem;
    int bytes;

    assert(0 < width && width < 100000);
    assert(0 < height && height < 100000);

    switch(type)
    {
    case IMAGE_BITMAP:
        bytes = (width * height + 7) / 8;
    case IMAGE_GRAYSCALE_8:
        bytes = width * height * 1;
        break;
    case IMAGE_GRAYSCALE_16:
        bytes = width * height * 2;
        break;
    case IMAGE_GRAYSCALE_FL:
        bytes = width * height * sizeof(float);
        break;
    case IMAGE_RGB_888:
        bytes = width * height * 3;
        break;
    default:
        DIE("Image type %d not supported", type);
    }

    /* allocate pixel data space as a continuous memory block */
    mem = calloc(1, bytes);
    if (!mem)
    {
        DEBUG("Failed to allocate memory for image pixels");
        return NULL;
    }

    /* use constructor from memory */
    self = image_new_from_mem(width, height, type, mem);
    return self;
}

/**
 * @fn image_new_from_mem(int width, int height, size_t bpp, uint8_t *mem)
 * @brief Image constructor; creates an image container, with pixel data from mem pointer.
 * @param width image width
 * @param height image height
 * @param type image type (pixel format)
 * @param mem pointer to pixel data in memory
 * @return Handle of a new image object, NULL if creation fails.
 * 
 * Make sure that mem points to pixel data, of proper length and format
 */
image_t *image_new_from_mem(int width, int height, image_type_t type, void *mem)
{
    assert(0 < width && width < 100000);
    assert(0 < height && height < 100000);
    assert(mem);
    image_t *self;

    /* create image container */
    self = calloc(1, sizeof(image_t));
    if (!self)
    {
        DEBUG("Failed to allocate memory for image");
        return NULL;
    }

    self->width = width;
    self->height = height;
    self->type = type;

    /* assign data buffer */
    self->data = mem;

    /* assign get/set pixel member functions */
    switch(self->type)
    {
    case IMAGE_BITMAP:
        self->getpixel = &image_bmp_getpixel;
        self->setpixel = &image_bmp_setpixel;
        break;
    case IMAGE_GRAYSCALE_8:
        self->getpixel = &image_gs8_getpixel;
        self->setpixel = &image_gs8_setpixel;
        break;
    case IMAGE_GRAYSCALE_16:
        self->getpixel = &image_gs16_getpixel;
        self->setpixel = &image_gs16_setpixel;
        break;
    case IMAGE_GRAYSCALE_FL:
        self->getpixel = &image_gsfl_getpixel;
        self->setpixel = &image_gsfl_setpixel;
        break;        
    case IMAGE_RGB_888:
        self->getpixel = &image_rgb_getpixel;
        self->setpixel = &image_rgb_setpixel;
        break;        
    default:
        DIE("Not implemented yet");
    }

    return self;
}

/**
 * @fn void image_delete(image_t *self)
 * @brief object destructor. Deallocates pixel data and image object.
 * Please make sure that you don't keep dangling pointers on image data.
 */
void image_delete(image_t *self) 
{
    DEBUG("Deleting image @%p", (void*)self);
 //   image_print_details(self);
    free(self->data);
    self->data = NULL;
    free(self);
}

/**
 * @brief Tool for debugging
 */
void image_print_details(const image_t *self)
{
    printf("Image @%p: width %d x height %d, %s format\n",
        (void *)self, self->width, self->height, 
        (self->type == IMAGE_BITMAP) ? "bitmap" :
        (self->type == IMAGE_GRAYSCALE_8) ? "8bit grayscale" :
        (self->type == IMAGE_GRAYSCALE_16) ? "16bit grayscale" :
        (self->type == IMAGE_GRAYSCALE_FL) ? "floating-point grayscale" :        
        (self->type == IMAGE_RGB_888) ? "3x8bit RGB color" : "unknown");
}


void image_print_ascii(const image_t *self)
{
    int disp_x, disp_y;
    int img_x, img_y;
    color_t c;
    char disp_c;    
    int disp_w = MIN(40, self->width);
    int disp_h = disp_w * self->height / self->width;
    printf("+");
    for (disp_x = 0; disp_x < disp_w; disp_x++)
    {
        printf("-");
    }
    printf("+\n");
    for (disp_y = 0; disp_y < disp_h; disp_y++)
    {
        img_y = disp_y * self->height / disp_h;
        printf("|");
        for (disp_x = 0; disp_x < disp_w; disp_x++)
        {
            img_x = disp_x * self->width / disp_w;
            c = self->getpixel(self, img_x, img_y);
            switch (self->type)
            {
            case IMAGE_BITMAP:
                disp_c = c.bit ? '#' : ' ';
                break;
            case IMAGE_GRAYSCALE_8:
                disp_c = " -+#" [c.gs8 >> 6];
                break;
            case IMAGE_GRAYSCALE_16:
                disp_c = " -+#" [c.gs16 >> 14];
                break;
            case IMAGE_GRAYSCALE_FL:
                disp_c = " -+#" [(int)(c.fl * 4.0)];
            default:
                disp_c = " -+#" [((c.rgb.r + c.rgb.g + c.rgb.b) / 3) >> 6];
                break;
            }
            printf("%c", disp_c);
        }
        printf("|\n");
    }
    printf("+");
    for (disp_x = 0; disp_x < disp_w; disp_x++)
    {
        printf("-");
    }
    printf("+\n");
}

/**
 * @fn image_coord_check(image_t *self, int x, int y)
 * @brief check whether (x,y) coordinates are within self's dimensions (0..width-1, 0..height-1)
 * @param self handle to image object
 * @param x abscissa
 * @param y ordinate
 * @return true iff coordinates are in range
 */
uint8_t image_coord_check(const image_t *self, int x, int y)
{
    return (0 <= x) && (0 <= y) &&
        (x < self->width) && (y < self->height);
}

/**
 * @brief set a pixel in a BITMAP-type image
 * @param x abscissa
 * @param y ordinate
 * @param c color container
 */
void image_bmp_setpixel(image_t *self, int x, int y, color_t c)
{
    assert(image_coord_check(self, x, y));
    long int byte_idx = (int)((self->width + 7)/8) * y + x / 8;
    int bit_idx = 7 - (x % 8);
    if (c.bit)
    {
        self->data[byte_idx] |= 1 << bit_idx;
    }
    else
    {
        self->data[byte_idx] &= 0xFF ^ (1 << bit_idx);
    }
}

/** 
 * @brief Get the color of a certain pixel in a BITMAP-type image
 * @param x abscissa
 * @param y ordinate
 * @return color container
 */ 
color_t image_bmp_getpixel(const image_t *self, int x, int y)
{
    color_t res;
    assert(image_coord_check(self, x, y));
    long int byte_idx = (int)((self->width + 7)/8) * y + x / 8;
    int bit_idx = 7 - (x % 8);
    res.bit = self->data[byte_idx] & (1 << bit_idx);
    return res;
}

/**
 * @brief set a pixel in a 8-bit grayscale image
 * @param x abscissa
 * @param y ordinate
 * @param c color container
 */
void image_gs8_setpixel(image_t *self, int x, int y, color_t c)
{
    assert(image_coord_check(self, x, y));
    self->data[y * self->width + x] = c.gs8;
}

/** 
 * @brief Get the color of a certain pixel in a 8-bit grayscale image
 * @param x abscissa
 * @param y ordinate
 * @return color container
 */ 
color_t image_gs8_getpixel(const image_t *self, int x, int y)
{
    color_t res;
    assert(image_coord_check(self, x, y));
    res.gs8 = self->data[y * self->width + x];
    return res;
}

/**
 * @brief set a pixel in a 16bit grayscale image
 * @param x abscissa
 * @param y ordinate
 * @param c color container
 */
void image_gs16_setpixel(image_t *self, int x, int y, color_t c)
{
    assert(image_coord_check(self, x, y));
    memcpy(&(self->data[2 * (y * self->width + x)]), &(c.gs16), sizeof(gs16_t));
}

/** 
 * @brief Get the color of a certain pixel in a 16bit grayscale image
 * @param x abscissa
 * @param y ordinate
 * @return color container
 */ 
color_t image_gs16_getpixel(const image_t *self, int x, int y)
{
    color_t res;
    assert(image_coord_check(self, x, y));
    memcpy(&(res.gs16), &(self->data[2 * (y * self->width + x)]), sizeof(gs16_t));
    return res;
}

/**
 * @brief set a pixel in a floating-point grayscale image
 * @param x abscissa
 * @param y ordinate
 * @param c color container
 */
void image_gsfl_setpixel(image_t *self, int x, int y, color_t c)
{
    assert(image_coord_check(self, x, y));
    memcpy(&(self->data[sizeof(float) * (y * self->width + x)]), &(c.fl), sizeof(float));
}

/** 
 * @brief Get the color of a certain pixel in a floating-point grayscale image
 * @param x abscissa
 * @param y ordinate
 * @return color container
 */ 
color_t image_gsfl_getpixel(const image_t *self, int x, int y)
{
    color_t res;
    assert(image_coord_check(self, x, y));
    memcpy(&(res.fl), &(self->data[sizeof(float) * (y * self->width + x)]), sizeof(float));
    return res;
}

/**
 * @brief Set a pixel in a RGB color image
 * @param x abscissa
 * @param y ordinate
 * @param c color container
 */
void image_rgb_setpixel(image_t *self, int x, int y, color_t c)
{
    assert(image_coord_check(self, x, y));
    memcpy(&(self->data[sizeof(rgb_t) * (y * self->width + x)]), &c.rgb, sizeof(rgb_t));
}

/** 
 * @brief Get the color of a certain pixel in a RGB color image
 * @param x abscissa
 * @param y ordinate
 * @return color container
 */ 
color_t image_rgb_getpixel(const image_t *self, int x, int y)
{
    color_t res;
    assert(image_coord_check(self, x, y));
    memcpy(&res.rgb, &(self->data[sizeof(rgb_t) * (y * self->width + x)]), sizeof(rgb_t));
    return res;
}
