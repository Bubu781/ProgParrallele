
#ifndef PIXEL_H
#define PIXEL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * \brief alias for 8-bit grayscale pixel value
 */
typedef uint8_t gs8_t;

/**
 * \brief alias for 16-bit grayscale pixel value
 */
typedef uint16_t gs16_t;

/** 
 * \struct rgb_t
 * \brief 3-channel (RGB), 256-level, pixel color value
 */
typedef struct 
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_t;

/**
 * \brief a container for all known color formats
 */
typedef union 
{
    bool bit;
    gs8_t gs8;
    gs16_t gs16;
    rgb_t rgb;
    uint32_t u32;
    float fl;
} color_t;


typedef struct 
{
    float h; //< hue, 0.0..360.0
    float s; //< saturation, 0.0 - 1.0
    float v; //< value, 0.0 - 1.0
} hsv_t;

/* convert pixel values between formats */
gs8_t gs8_from_gs16(gs16_t g16);
gs8_t gs8_from_rgb(rgb_t rgb);
gs8_t gs8_from_f(float gray);
gs16_t gs16_from_gs8(gs8_t g8);
gs16_t gs16_from_rgb(rgb_t rgb);
gs16_t gs16_from_f(float gray);
rgb_t rgb_from_3u8(uint8_t r, uint8_t g, uint8_t b);
gs8_t gs8_from_3f(float r, float g, float b);
rgb_t rgb_from_gs8(gs8_t g);
rgb_t rgb_from_gs16(gs16_t g);
color_t color_from_gs8(gs8_t g);
color_t color_from_gs16(gs16_t g);
color_t color_from_rgb(rgb_t rgb);
/* color space conversions */
rgb_t rgb_from_hsv(hsv_t hsv);
hsv_t hsv_from_rgb(rgb_t rgb);

#endif
