#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <ctype.h>

#include "image_file_io.h"
#include "utils.h"


/**
 * \fn image_new_open(const char *fname)
 * \brief Image constructor; creates an image object from a PGM/PPM file.
 * \param fname path of image file
 * \return Handle of a new image object, NULL in case of failure.
 */
image_t *image_new_open(const char *fname) 
{
    FILE *fp;
    int width, height, depth, channels;
    size_t read, expected;
    image_t *self;
    uint8_t chr;
    image_type_t type;
    uint8_t pnm_format;
    uint8_t ascii_encoding;

    fp = fopen(fname, "rb");
    if (!fp) 
    {
        DEBUG("Failed to open file `%s`", fname);
        return NULL;
    }
    
    if (getc(fp) != 'P') 
    {
        DEBUG("Not a Netpbm file");
        return NULL;
    }

    pnm_format = getc(fp) - '0';
    if (pnm_format <= 0 || pnm_format > 6)
    {
        DIE("Not a Netpbm file");
    }

    /* read until end of line */
    while(getc(fp) != '\n') 
        continue;
    /* skip comments */
    while (getc(fp) == '#')
    {
        while (getc(fp) != '\n');
    }

    fseek(fp, -1, SEEK_CUR); // seems necessary in some cases...
    fscanf(fp,"%d", &width);
    fscanf(fp,"%d", &height);
    fscanf(fp,"%d", &depth);

    // basic plausibility checks
    assert(0 < width && width < 10000);
    assert(0 < height && height < 10000);
    assert(depth < 65536);

    ascii_encoding = 0;
    switch(pnm_format)
    {
    case 1: // P1 = black/white, 1 bit ; ascii encoding
        ascii_encoding = 1;
    case 4: // P4 = black/white, 1 bit ; binary encoding
        type = IMAGE_BITMAP;
        break;
    case 2: // P2 = 0-255 or 0-65535 grayascale ; ascii
        ascii_encoding = 1;
    case 5: // P5 = 0-255 or 0-65535 grayascale ; binary
        type = (depth < 256) ? IMAGE_GRAYSCALE_8 : IMAGE_GRAYSCALE_16;
        break;
    case 3: // P3 = 0-255 x 3 channels (R, G, B) color ; ascii
        ascii_encoding = 1;
    case 6: // P6 = 0-255 x 3 channels (R, G, B) color ; binary
        type = IMAGE_RGB_888;
        break;
    default:
        DIE("Format not supported");
    }

    self = image_new(width, height, type);
    if (!self) 
    {
        DEBUG("Image container failed");
        return NULL;
    }

    if (ascii_encoding) 
    {
        //DEBUG("Reading ASCII-encoded pixel data");
        FOR_YX(self, y, x)
        {
            int rc;
            uint32_t v, r, g, b;
            color_t c;

            switch(self->type)
            {
            case IMAGE_BITMAP:
                rc = fscanf(fp, "%d", &v);
                self->setpixel(self, x, y, (color_t){.bit = LIMIT(v, 0, 1)});
                break;
            case IMAGE_GRAYSCALE_8:
                rc = fscanf(fp, "%d", &v);
                self->setpixel(self, x, y, (color_t){.gs8 = LIMIT(v, 0, 255)});
                break;
            case IMAGE_GRAYSCALE_16:
                rc = fscanf(fp, "%d", &v);
                self->setpixel(self, x, y, (color_t){.gs16 = LIMIT(v, 0, 65535)});
                break;
            case IMAGE_RGB_888:
                rc = fscanf(fp, "%d%d%d", &r, &g, &b);
                printf("%d %d %d\n", r, g, b);
                self->setpixel(self, x, y, (color_t){.rgb.r = LIMIT(r, 0, 255), .rgb.g = LIMIT(g, 0, 255), .rgb.b = LIMIT(b, 0, 255)});
                break;
            default:
                DIE("Not supported");
            }
        }
    }
    else 
    {
        //DEBUG("Reading binary-encoded pixel data");
        long int nbytes;
        switch(self->type)
        {
        case IMAGE_BITMAP:
            nbytes = (self->width * self->height + 7) / 8;
            break;
        case IMAGE_GRAYSCALE_8:
            nbytes = self->width * self->height;
            break;
        case IMAGE_GRAYSCALE_16:
            nbytes = self->width * self->height * 2;
            break;
        case IMAGE_RGB_888:
            nbytes = self->width * self->height * 3;
            break;
        default:
            DIE("Unsupported");
        }
         
        int read = fread(self->data, 1, nbytes, fp);
        if (read < nbytes)
        {
            DIE("Expected %d bytes, could read only %d bytes.\n", nbytes, read);
        }
    }

    DEBUG("Read file `%s` : ", fname);
    image_print_details(self);
    printf("\n");
    return self;
}



/** 
 * \fn int image_save(const image_t *self, const char *fname)
 * \brief saves image data to file
 * \param self image object
 * \param fname path to file
 * \return 0 if success, -1 if failure
 */
int image_save_ascii(const image_t *self, const char *fname)
{
    return image_save(self, fname, 0);
}

int image_save_binary(const image_t *self, const char *fname)
{
    return image_save(self, fname, 1);
}

int image_save(const image_t *self, const char *fname, int binary_encoding)
{
    FILE *fp;
    int colors = 0;
    int depth;
    int format;
    
    fp = fopen(fname, "wb");
    if (!fp)
    {
        DIE("Could not open file %s", fname);
    }
    assert(fp);
    assert(self);
    assert(self->data);

    DEBUG("Saving image @%p to file %s",
        (void *)self, fname);
    image_print_details(self);

    switch(self->type)
    {
    case IMAGE_BITMAP:
        format = 1;
        depth = 1;
        break;
    case IMAGE_GRAYSCALE_8:
        format = 2;
        depth = 255;
        break;
    case IMAGE_GRAYSCALE_16:
        format = 2;
        depth = 65535;
        break;
    case IMAGE_GRAYSCALE_FL:
        format = 2; 
        depth = 255;
        DEBUG("Warning: floating-point grayscale values will be converted to 0...%d range", depth);
        break;
    case IMAGE_RGB_888:
        format = 3;
        depth = 255;
        break;
    default:
        DIE("Unsupported");
    }

    if (binary_encoding)
    {
        format += 3;
    }

    fprintf(fp, "P%d\n%d %d\n%d\n",
            format,
            self->width, self->height,
            depth);

    if (binary_encoding)
    {
        long int nbytes;
        switch(self->type)
        {
        case IMAGE_BITMAP:
            nbytes = (self->height * self->width + 7) / 8;
            break;
        case IMAGE_GRAYSCALE_8:
            nbytes = (self->height * self->width);
            break;
        case IMAGE_GRAYSCALE_16:
            nbytes = (self->height * self->width) * 2;
            break;
        case IMAGE_RGB_888:
            nbytes = (self->height * self->width) * 3;
            break;

        case IMAGE_GRAYSCALE_FL:
        default:
            DIE("Unsupported");
            break;
        }
        fwrite(self->data, nbytes, 1, fp);
    }
    else
    {
        FOR_YX(self, y, x)
        {
            color_t c;
            c = self->getpixel(self, x, y);

            switch(self->type)
            {
            case IMAGE_BITMAP:
                fprintf(fp, "%d ", c.bit);
                break;
            case IMAGE_GRAYSCALE_8:
                fprintf(fp, "%d ", c.gs8);
                break;
            case IMAGE_GRAYSCALE_16:
                fprintf(fp, "%d ", c.gs16);
                break;
            case IMAGE_GRAYSCALE_FL:
                fprintf(fp, "%d ", (uint16_t)(depth * LIMIT(c.fl, 0.0, 1.0)));
                break;
            case IMAGE_RGB_888:
                fprintf(fp, "%d %d %d ", c.rgb.r, c.rgb.g, c.rgb.b);
                break;
            }

            if (x == self->width - 1)
            {
                fprintf(fp, "\n");
            }
        }
        fprintf(fp, "\n# end of file\n");        
    }

    fclose(fp);
    return 0;
}

