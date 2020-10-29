/**
 * @file image_connected_components.c
 * @brief Image processing library: connected components labeling
 * @author Etienne HAMELIN
 * @version 0.1
 * @date octobre 2020
 */
#include "image_connected_components.h"

/**
 * @brief Assign a unique color to each tag
 * @param tag a class tag
 * @return a color_t RGB color
 */
color_t class_color(int tag)
{
  /* multiply class id by golden angle: move along the color wheel */
  float hue = 137.507 * tag;
  /* also change saturation & value by a cycle of 9 */
  float sat = 0.33 * (3 -  tag % 3);
  float val = 0.33 * (3 - (tag / 3) % 3); 
  return (color_t){.rgb = rgb_from_hsv((hsv_t){.h = hue, .s = sat, .v = val})};
}

/**
 * @brief Find the root ancestor of a given tag
 * @param table Table of class ancestors
 * @param tag the original tag (expected > 0)
 * @return The root of the tree containing given tag
 * 
 * This recursion is guaranteed to finish since:
 * we always progress down to 0, since by construction we always have table[c] <= c
 */
int find_root(int *table, int tag)
{
  while ((tag > 0) && (table[tag] > 0) && (table[tag] < tag))
  {
    tag = table[tag];
  }
  return tag;
}

/**
 * @brief Join two classes
 * @param table equivalence table
 * @param tag1 a tag of the first class
 * @param tag2 a tag of the second class
 * @return the root tag of the now common class
 */
int join(int *table, int tag1, int tag2)
{
  /* first look for each class' root ancestor */
  tag1 = find_root(table, tag1);
  tag2 = find_root(table, tag2);
  /* now join both roots */
  int t_min = (tag1 < tag2) ? tag1 : tag2;
  int t_max = (tag1 < tag2) ? tag2 : tag1;
  table[t_max] = t_min;
  return t_min;
}


/**
 * @brief Return the minimum strictly positive of two int values
 * @param a a int
 * @param b another int
 * @return min(a,b) if both are positive; a or b if only one of them is positive; 0 if both are null
 */
int min_non_zero(int a, int b)
{
  if (a > 0 && b > 0)
  {
    return MIN(a, b);
  }
  else
  {
    return MAX(0, MAX(a, b));
  }
}

/**
 * @brief First pass: assign a temporary tag to each pixel, and store equivalences in given table
 * @param self the input image (binary)
 * @param tags the (output) image for storing pixel tags
 * @param equiv_out the table holding equivalence classes
 * @return the number of temporary tags assigned
 */
int ccl_temp_tag(
      const image_t *self,
      image_t *tags, 
      int *equiv_out)
{
  assert(self && tags && equiv_out);

  int x, y;
  bool bg_color;
  int num_tags = 0;

  DEBUG("First step: assign temporary class tag");

    /* Detect background color */
  bg_color = image_bmp_getpixel(self, 0, 0).bit;

  /* Loop over all image pixels */
  for(y = 0; y < self->height; ++y)
  {
    for (x = 0; x < self->width; ++x)
    {
      /* read current pixel color */
      bool pxl_color = image_bmp_getpixel(self, x, y).bit;
      
      /* by default, pixel tag is zero (background) */
      int tag = 0;

      if (pxl_color != bg_color) 
      {
        /* Current pixel is foreground color: give it a tag, but which one? */

        /* Read the tag (if any) of the North and West adjacent pixels */
        /* or 0, if outside image coordinate ranges */
        int tag_n = image_coord_check(tags, x, y-1) ? 
              image_gs16_getpixel(tags, x, y-1).gs16 
              : 0;
        int tag_w = image_coord_check(tags, x-1, y) ? 
              image_gs16_getpixel(tags, x-1, y).gs16
              : 0;

        /* Current pixel is the minimum non-zero of both tags */
        tag = min_non_zero(tag_n, tag_w);

        if (tag == 0)
        {
          /* adjacent pixels have not yet been tagged: create a new tag */
          ++num_tags;
          assert(num_tags < MAX_TAGS);
          tag = num_tags;
          /* new tag has no equivalence */
          equiv_out[tag] = tag;
        }
        
        if (tag_n > 0 && tag_w > 0 && tag_w != tag_n)
        {
          /* if neighbors have different tags: join them */
          join(equiv_out, tag_n, tag_w);
        }
      }
      /* store tag in the tags image structure */
      image_gs16_setpixel(tags, x, y, (color_t){.gs16 = tag});
    }
  }

  return num_tags;
}

/**
 * @brief Reduce equivalence table and renumber classes
 * @param equiv_table the input equivalence table
 * @param num_tags number of used tags in equivalence table
 * @param class_num_out (output) table of tag classes
 * @return number of connected components
 */
int ccl_reduce_equivalences(
      int *equiv_table, 
      int num_tags, 
      int *class_num_out)
{
  int num_classes = 0;

  for (int t = 1; t <= num_tags; ++t)
  {
    if (equiv_table[t] == t)
    {
      /* t is the root of an equivalence class: assign a new class identifier */
      class_num_out[t] = ++num_classes;
    }
    else
    {
      /* t is not a root: renumber with the root tag of its class.
      Note that find_root(table, t) <= t (by construction), so that class_num[find_root(table, t)] is already set. */
      class_num_out[t] = class_num_out[find_root(equiv_table, t)];
    }
  }

  return num_classes;
}

/**
 * @brief Replace temporary tags by class number (connected component number)
 * @param tags image containing temporary tags (modified in place)
 * @param class_num table that associates tags to number
 */
void ccl_retag(image_t *tags, int *class_num)
{
  int x, y, t;
  /* Replace temporay class tags by their renumbered class root */
  for (y = 0; y < tags->height; ++y)
  {
    for (x = 0; x < tags->width; ++x)
    {
      /* initial pixel tag */
      t = image_gs16_getpixel(tags, x, y).gs16;
      if (t != 0) 
      {
        /* get connected component number from tag */
        t = class_num[t];
        image_gs16_setpixel(tags, x, y, (color_t){.gs16 = t});
      }
    }
  }
}

/**
 * @brief Analyze connected components
 * @param tags an image containing pixel (renumbered) tags
 * @param con_cmp 
 * @param num_classes
 */
void ccl_analyze(
      const image_t *tags, 
      image_connected_component_t *con_cmp, 
      int num_classes)
{
  int x, y, t;

  for (t = 0; t < num_classes; ++t)
  {
    con_cmp[t] = (image_connected_component_t){
          .num_pixels = 0,
          .x1 = INT32_MAX,
          .y1 = INT32_MAX,
          .x2 = 0,
          .y2 = 0
        };
  }

  for (y = 0; y < tags->height; ++y)
  {
    for (x = 0; x < tags->width; ++x)
    {
      t = image_gs16_getpixel(tags, x, y).gs16;
      if (t > 0)
      {
        assert(t <= num_classes);
        /* compute size and bounding box of connected component */
        con_cmp[t-1].num_pixels++;
        con_cmp[t-1].x1 = MIN(con_cmp[t-1].x1, x);
        con_cmp[t-1].y1 = MIN(con_cmp[t-1].y1, y);
        con_cmp[t-1].x2 = MAX(con_cmp[t-1].x2, x);
        con_cmp[t-1].y2 = MAX(con_cmp[t-1].y2, y);
      }
    }
  }
}

/**
 * @brief Draw a colored representation of connected components
 * @param tags the reduced & renumbered pixel tags
 * @param color the (output) image structure
 */
void ccl_draw_colors(const image_t *tags, image_t *color)
{
  int x, y, t;
  assert(tags && color);
  for (y = 0; y < tags->height; ++y)
  {
    for (x = 0; x < tags->width; ++x)
    {
      t = image_gs16_getpixel(tags, x, y).gs16;
      if (t != 0)
      {
        image_rgb_setpixel(color, x, y, class_color(t-1));
      }
    }
  }
}

/**
 * @brief Draw a bounding box around each connected component
 * @param color the (output) image structure
 * @param con_cmp the table of identified connected components
 */
void ccl_draw_bounding_boxes(image_t *color, 
    image_connected_component_t *con_cmp, 
    int num_cc)
{
  int x, y, t;
  for (t = 0; t < num_cc; ++t)
  {
    int x1 = con_cmp[t].x1; 
    int y1 = con_cmp[t].y1;
    int x2 = con_cmp[t].x2; 
    int y2 = con_cmp[t].y2;
    color_t c = class_color(t);

    for (x = x1; x <= x2; ++x)
    {
      if ((x + y1) % 2) image_rgb_setpixel(color, x, y1, c);
      if ((x + y2) % 2) image_rgb_setpixel(color, x, y2, c);
    }
    for (y = y1; y <= y2; ++y)
    {
      if ((x1 + y) % 2) image_rgb_setpixel(color, x1, y, c);
      if ((x2 + y) % 2) image_rgb_setpixel(color, x2, y, c);
    }
  }
}

/**
 * @brief Draw a legend of colors used
 * @param color the (output) image structure
 * @param num_colors the number of user colors
 */
void ccl_draw_legend(image_t *color, int num_colors)
{
  int x, y, t;
  color_t c;
  for (y = 0; y < color->height; ++y)
  {
    t = (y * num_colors) / color->height;
    c = class_color(t);
    for (x = 0; x < 20; ++x)
    {
      image_rgb_setpixel(color, x, y, c);
    }
  }
}

/**
 * @brief Identify connected components in given image
 * @param self the input image (should be a binary black & white image, i.e. self->type = IMAGE_BITMAP)
 * @param tags an image structure for holding the connected components tags (should be a 16-bit grayscale image), or NULL
 * @param color an output image structure for holding a color visualization of connected components, or NULL
 * @param con_cmp_out pointer to a image_connected_component_t[] table that will be written by this procedure, for returning the detected connected components, or NULL
 * @param debug 1 if we shall print out equivalence classes, generate intermediate files, etc. Good for debug, not for performance.
 * @return the number of classes detected 
 */
int image_connected_components(
      const image_t *self, 
      image_t *tags, 
      image_t *color, 
      int debug)
{
  int *equiv_table;
  int num_tags = 0;

  int *class_num;
  int num_cc;
  int t;

  /* ~~~~~~~~~~ Verify input arguments, initialize tables ~~~~~~~~~~ */
  assert(self && 
        (self->type == IMAGE_BITMAP));

  assert(tags && 
        (tags->type == IMAGE_GRAYSCALE_16) &&
        (tags->width >= self->width) && 
        (tags->height >= self->height));

  assert(color &&
        (color->type == IMAGE_RGB_888) &&
        (color->width >= self->width) && 
        (color->height >= self->height));

  /* Allocate the equivalence table */
  equiv_table = calloc(MAX_TAGS, sizeof(int));
  assert(equiv_table);
  
  /* ~~~~~~~~~~ First step: assign temporary class tags ~~~~~~~~~~ */
  num_tags = ccl_temp_tag(self, tags, equiv_table);
#ifndef NDEBUG
  /* save temporay tags as "tags.pgm" */
  image_save_ascii(tags, "tags.pgm");
  DEBUG("Tags equivalences table:");
  for (t = 0; t <= num_tags; ++t)
  {
    DEBUG("T[%02d] = %02d", t, equiv_table[t]);
  }
#endif
  
  /* ~~~~~~~~~~ Second step: reduce equivalence classes and renumber ~~~~~~~~~~ */
  DEBUG("Now reduce tag equivalence classes, and renumber those classes");
  /**
   * Reduce tag equivalence classes,
   * Renumber classes, so that they are numbered 0 .. num_classes-1
   * Recall: a class root ancestor A is charcterized by table[A] = A.
   */
  class_num = calloc(num_tags+1, sizeof(int));
  assert(class_num);
  num_cc = ccl_reduce_equivalences(equiv_table, num_tags, class_num);

  if (debug)
  {
    DEBUG("Tags renumbering:");
    for (t = 0; t <= num_tags; ++t)
    {
      DEBUG("N[%02d] = %02d", t, class_num[t]);
    }
  }

  /* ~~~~~~~~~~ Third step: replace temp tags by connected component number ~~~~~~~~~~ */
  DEBUG("Re-tag");
  ccl_retag(tags, class_num);

  image_save_ascii(tags, "classes.pgm");

  /* ~~~~~~~~~~ Fourth step: generate useful outputs ~~~~~~~~~~ */
  DEBUG("Analyze connected components");
  /* allocate & initialize connected components output structure */
  image_connected_component_t *con_cmp = calloc(num_cc, sizeof(image_connected_component_t));
  assert(con_cmp);
  ccl_analyze(tags, con_cmp, num_cc);

  /* Write outputs */ 
  for (t = 0; t < num_cc; ++t)
  {
    printf("Connected component  #%03d: bounding box (%04d,%04d),(%04d,%04d), %06d pixels\n",
        t, 
        con_cmp[t].x1, con_cmp[t].y1,
        con_cmp[t].x2, con_cmp[t].y2,
        con_cmp[t].num_pixels);
  }

  DEBUG("Draw color output");
  /* draw connected components as a color image */
  ccl_draw_colors(tags, color);
  
  if (color->width > 100)
  {
    /* draw connected components bounding boxes */
    ccl_draw_bounding_boxes(color, con_cmp, num_cc);
    /* draw a legend on the left */
    ccl_draw_legend(color, num_cc);
  }

  if (debug)
  {
    /* use ASCII format for readability */
    image_save_ascii(color, "color.ppm");
  }
  else
  {
    /* use BIN format for large images: optimize for speed */
    image_save_binary(color, "color.ppm");
  }    

  /* liberate allocated memory */
  free(equiv_table);
  free(class_num);

  /* note: caller is responsible for liberating the tags and color images */
  DEBUG("End of connected components labeling");
  return num_cc;
}




