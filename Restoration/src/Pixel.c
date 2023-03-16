#include "Pixel.h"

void CopyPixel(struct pixel src, struct pixel *dest)
{
    dest->m = src.m;
    dest->n = src.n;
}

bool PixelEqual(struct pixel first, struct pixel second)
{
    bool ret_val = false;
    if ((first.m == second.m) &&
        (first.n == second.n))
    {
        ret_val = true;
    }
    else
    {
        ret_val = false;
    }
    return ret_val;
}
