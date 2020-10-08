#include "pixel.h"
#include "utils.h"

/* Convert color types to Grayscale 8 */

gs8_t gs8_from_gs16(gs16_t g16)
{
    return g16 / 256;
}

gs8_t gs8_from_rgb(rgb_t rgb)
{
    return (rgb.r + rgb.g + rgb.b) / 3;
}

gs8_t gs8_from_f(float gray)
{
    return 255.0 * LIMIT(gray, 0.0, 1.0);
}


/* Convert color types to Grayscale 16 */

gs16_t gs16_from_gs8(gs8_t g8)
{
    return g8 * 256;
}

gs16_t gs16_from_rgb(rgb_t rgb)
{
    /* 85 ~= 65536 / (3 * 256) */
    return (rgb.r + rgb.g + rgb.b) * 85;
}

gs16_t gs16_from_f(float gray)
{
    return 65535.0 * LIMIT(gray, 0.0, 1.0);
}


/* Convert color types to RGB */

rgb_t rgb_from_3u8(uint8_t r, uint8_t g, uint8_t b)
{
    rgb_t rgb = {r, g, b};
    return rgb;
}

rgb_t rgb_from_3f(float r, float g, float b)
{
    return rgb_from_3u8(
        255.0 * LIMIT(r, 0.0, 1.0), 
        255.0 * LIMIT(g, 0.0, 1.0), 
        255.0 * LIMIT(b, 0.0, 1.0));
}

rgb_t rgb_from_gs8(gs8_t g)
{
    return rgb_from_3u8(g, g, g);
}

rgb_t rgb_from_gs16(gs16_t g)
{
    g /= 256;
    return rgb_from_3u8(g, g, g);
}


/* Convert to generic color container */

color_t color_from_gs8(gs8_t g)
{
    color_t c; 
    c.gs8 = g;
    return c;
}

color_t color_from_gs16(gs16_t g)
{
    color_t c;
    c.gs16 = g;
    return c;
}

color_t color_from_rgb(rgb_t rgb)
{
    color_t c;
    c.rgb = rgb;
    return c;
}


/* Convert color to RGB from other color spaces */
rgb_t rgb_from_hsv(hsv_t hsv)
{
    rgb_t rgb;
    int i;
    float f;
    float p, q, t;

    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    if (s <= 0.0)
    {
        return rgb_from_3u8(0, 0, 0);
    }

    while (h > 360.) {h -= 360.;}
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

hsv_t hsv_from_rgb(rgb_t rgb)
{
    /* https://www.tutorialspoint.com/c-program-to-change-rgb-color-model-to-hsv-color-model */
    
    hsv_t res;
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