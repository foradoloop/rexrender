#ifndef PPM_H
#define PPM_H

#include <stdio.h>

void ppm_write(FILE *file, int width, int height, void *ctx, void (*cb)(void *ctx, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b));

#endif

