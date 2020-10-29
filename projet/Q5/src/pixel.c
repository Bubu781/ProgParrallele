/**
 * @file pixel.c
 * @brief Pixel color type conversions utilities
 * @author Etienne HAMELIN
 * @version 0.1
 * @date october 2020
 */
#include "pixel.h"
#include "utils.h"
#include <math.h>

/**
 * @brief Create a RGB color structure from 3 8-bit levels of red, green, blue
 * @param r level of red 
 * @param g level of green
 * @param b level of blue
 * @return a rgb_t structure 
 */
rgb_t rgb_from_3u8(uint8_t r, uint8_t g, uint8_t b)
{
    rgb_t rgb = {r, g, b};
    return rgb;
}

/**
 * @brief Create a RGB color structure from 3 floating-point levels of red, green, blue
 * @param r level of red (internally constrained 0.0 to 1.0)
 * @param g level of green (internally constrained 0.0 to 1.0)
 * @param b level of blue (internally constrained 0.0 to 1.0)
 * @return a rgb_t structure 
 */
rgb_t rgb_from_3f(float r, float g, float b)
{
    return rgb_from_3u8(
        255.0 * LIMIT(r, 0.0, 1.0), 
        255.0 * LIMIT(g, 0.0, 1.0), 
        255.0 * LIMIT(b, 0.0, 1.0));
}

/* Convert color to RGB from other color spaces */

/**
 * @brief Convert a hue-saturation-value color into a RGB color
 * @param hsv hue-saturation-value color specification
 * @return rgb_t color 
 */
rgb_t rgb_from_hsv(hsv_t hsv)
{
    int i;
    float f;
    float p, q, t;

    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    if (s <= 0.0)
    {
        return rgb_from_3f(0.0, 0.0, 0.0);
    }

    while (h >= 360.) {h -= 360.;}
    while (h < 0.) {h += 360.;}

    h /= 60.;
    i = (int)h;
    f = h - i;
    p = v * (1.0 - s);
    q = v * (1.0 - s * f);
    t = v * (1.0 - s * (1.0 - f));
    
    switch(i)
    {
    case 0:
        return rgb_from_3f(v, t, p);
        break;
    case 1:
        return rgb_from_3f(q, v, p);
        break;
    case 2:
        return rgb_from_3f(p, v, t);
        break;
    case 3:
        return rgb_from_3f(p, q, v);
        break;
    case 4:
        return rgb_from_3f(t, p, v);
        break;
    default:
        return rgb_from_3f(v, p, q);
        break;
    }
}

/**
 * @brief Convert a RGB color into a hue-saturation-value description
 * @param rgb a RGB color
 * @return hue-saturation-value color 
 */
hsv_t hsv_from_rgb(rgb_t rgb)
{
    /* Inspired from: 
    https://www.tutorialspoint.com/c-program-to-change-rgb-color-model-to-hsv-color-model
     */
    float h, s, v;
    float r = rgb.r / 255.0;
    float g = rgb.g / 255.0;
    float b = rgb.b / 255.0;

    float cmin = MIN(MIN(r, g), b);
    float cmax = MAX(MAX(r, g), b);
    float diff = cmax - cmin;
    
    if (cmax == cmin)
    {
        h = 0;
    }  
    else if (cmax == r)
    {
        h = fmod((60 * ((g - b) / diff) + 360), 360.0);
    } 
    else if (cmax == g)
    {
        h = fmod((60 * ((b - r) / diff) + 120), 360.0);
    }
    else if (cmax == b)
    {
        h = fmod((60 * ((r - g) / diff) + 240), 360.0);
    }

    if (cmax == 0)
    {
        s = 0;
    }
    else
    {
        s = (diff / cmax);
    }
    v = cmax;
    return (hsv_t){.h = h, .s = s, .v = v};
}