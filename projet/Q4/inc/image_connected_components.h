#ifndef IMAGE_CONNECTED_COMPONENTS_H
#define IMAGE_CONNECTED_COMPONENTS_H
/**
 * @file image_connected_components.h
 * @brief Image processing library: connected components labeling
 * @author Etienne HAMELIN
 * @version 0.1
 * @date octobre 2020
 */

#include "image_lib.h"

#define MAX_TAGS 500000

/**
 * @brief bounding box descriptor of a connected component
 */
typedef struct 
{
  int x1, x2, y1, y2;       /*!< (x1,y1): upper-left pixel, (x2,y2): lower-right pixel of the bounding box */
  unsigned int num_pixels;  /*!< number of pixels of that connected component */
} image_connected_component_t;

color_t class_color(int class);
int find_root(int *table, int tag);
int join(int *table, int tag1, int tag2);
int min_non_zero(int a, int b);
int image_connected_components(const image_t *self, image_t *tags, image_t *color, int debug);

#endif