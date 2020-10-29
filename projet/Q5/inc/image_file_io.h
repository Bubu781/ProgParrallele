#ifndef IMAGE_FILE_IO_H
#define IMAGE_FILE_IO_H
/**
 * @file image_file_io.h
 * @brief Basic image processing library: read/write NetBPM files
 * @author Etienne HAMELIN
 * @version 0.1
 * @date october 2020
 */

#include <image.h>

image_t *image_new_open(const char *fname);
int image_save_ascii(const image_t *self, const char *fname);
int image_save_binary(const image_t *self, const char *fname);
int image_save(const image_t *self, const char *fname, int binary_encoding);

#endif