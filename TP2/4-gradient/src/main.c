#include <stdio.h>
#include <stdlib.h>

#include "image_lib.h"


void test_image_gradient(const char *fname)
{
  /* Allocate image structure for input image */
  image_t *img = image_new_open(fname);
  /* Allocate image structure for output (gradient) image */
  image_t *out = image_new(img->width-2, img->height-2, IMAGE_RGB_888);
  /* Compute the gradient */
  image_gradient(img, out);
  /* Save gradient image */
  image_save_binary(out, "out/gradient.bin.ppm");
  /* Clean up heap */
  image_delete(img);
  image_delete(out);
}


int main(int argc, char **argv)
{
  printf("Started.\n");

  if (argc >= 2) 
  {
    test_image_gradient(argv[1]);
  }
  else
  {
    test_image_gradient("img/New_York_City_skyline.pgm");  
  }
    
  printf("End.\n");
  return 0;
}