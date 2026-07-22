#include "canvas.h"
#include "raster.h"
#include "ppm.h"
#include <stdlib.h>

const int WIDTH = 1024;
const int HEIGHT = 768;

// RGBX format
const uint32_t RED_CH_MASK = 0xFF000000;
const uint32_t GREEN_CH_MASK = 0x00FF0000;
const uint32_t BLUE_CH_MASK = 0x0000FF00;

const int RED_CH_SHIFT = 24;
const int GREEN_CH_SHIFT = 16;
const int BLUE_CH_SHIFT = 8;

void cb(void *ctx, int x, int y)
{
	uintptr_t *ptr = (uintptr_t *)ctx;
	Canvas *canvas = (Canvas *)ptr[0];
	uint32_t color = *((uint32_t *)ptr[1]);

	canvas_set_pixel(canvas, x, y, color);
}

uint32_t make_rgbx(unsigned char r, unsigned char g, unsigned char b)
{
	return ( r << RED_CH_SHIFT | g << GREEN_CH_SHIFT | b << BLUE_CH_SHIFT | 0xFF );
}

unsigned char get_red_ch(uint32_t color)
{
	return (color & RED_CH_MASK) >> RED_CH_SHIFT;
}

unsigned char get_green_ch(uint32_t color)
{
	return (color & GREEN_CH_MASK) >> GREEN_CH_SHIFT;
}

unsigned char get_blue_ch(uint32_t color)
{
	return (color & BLUE_CH_MASK) >> BLUE_CH_SHIFT;
}

void fetcher(void *ctx, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b)
{
	Canvas *canvas = (Canvas *)ctx;
	uint32_t color = canvas_get_pixel(canvas, x, y);

	*r = get_red_ch(color);
	*g = get_green_ch(color);
	*b = get_blue_ch(color);
}

int main(void)
{
	Canvas canvas;
	uint32_t *pixels = malloc(sizeof(uint32_t) * WIDTH * HEIGHT);

	canvas_init(&canvas, pixels, WIDTH, HEIGHT);

	srand((unsigned int)(uintptr_t)(main));

	for (int i = 0; i < (1 << 20); i++) {
		int x0, y0;
		int x1, y1;

		unsigned char r, g, b;

		x0 = rand() % WIDTH; y0 = rand() % HEIGHT;
		x1 = rand() % WIDTH; y1 = rand() % HEIGHT;

		r = rand() % 256;
		g = rand() % 256;
		b = rand() % 256;

		uint32_t color = make_rgbx(r, g, b);

		uintptr_t ptr[2] = { (uintptr_t)&canvas, (uintptr_t)&color };

		raster_line(
			x0, y0,
			x1, y1,
			(void *)&ptr,
			cb
			);
	}

	FILE *output = fopen("output.ppm", "wb");

	ppm_write(output, WIDTH, HEIGHT, (void *)&canvas, fetcher);

	free(pixels);
	fclose(output);

	return 0;
}

