
#ifndef PIXEL_H
#define PIXEL_H
/**
 * @file pixel.h
 * @brief Pixel color type conversions utilities
 * @author Etienne HAMELIN
 * @version 0.1
 * @date october 2020
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief alias for 8-bit grayscale pixel value
 */
typedef uint8_t gs8_t;

/**
 * @brief alias for 16-bit grayscale pixel value
 */
typedef uint16_t gs16_t;

/** 
 * @struct rgb_t
 * @brief 3-channel (red, green, blue), 256-level, pixel color value
 */
typedef struct 
{
    uint8_t r;  /*!< level of red */
    uint8_t g;  /*!< level of green */
    uint8_t b;  /*!< level of blue */
} rgb_t;

/**
 * @union color_t
 * @brief a container for all color formats
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


/**
 * @struct hsv_t
 * @brief a structure for hue-saturation-value color specification
 */
typedef struct 
{
    float h; //!< hue, 0.0..360.0
    float s; //!< saturation, 0.0 - 1.0
    float v; //!< value, 0.0 - 1.0
} hsv_t;


/* convert pixel values between formats */
gs8_t gs8_from_3f(float r, float g, float b);
/* color space conversions */
rgb_t rgb_from_hsv(hsv_t hsv);
hsv_t hsv_from_rgb(rgb_t rgb);

#endif
