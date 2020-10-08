#ifndef IMAGE_FILE_IO_H
#define IMAGE_FILE_IO_H

#include <image.h>

/**
 * Read from / save to PNM format files.
 * 
 * Recall the following definitions:
 * 
 * Extension    Name        ASCII   Binary
 * pbm  Portable Bitmap     P1      P4
 * pgm  Portable Graymap    P2      P5
 * ppm  Portable Pixmap     P3      P6
 * 
 */ 

image_t *image_new_open(const char *fname);
int image_save_ascii(const image_t *self, const char *fname);
int image_save_binary(const image_t *self, const char *fname);

#endif