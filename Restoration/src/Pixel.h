#ifndef PIXEL_H
#define PIXEL_H
#include <stdbool.h>

struct pixel {
int m,n; /* m=row, n=col */
};

void CopyPixel(struct pixel src, struct pixel *dest);

bool PixelEqual(struct pixel first, struct pixel second);
#endif // PIXEL_H

