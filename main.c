#include "canvas.h"
#include "raster.h"
#include "ppm.h"
#include <stdlib.h>

const int WIDTH = 1280;
const int HEIGHT = 720;

// RGBX format
const uint32_t RED_CH_MASK = 0xFF000000;
const uint32_t GREEN_CH_MASK = 0x00FF0000;
const uint32_t BLUE_CH_MASK = 0x0000FF00;

const int RED_CH_SHIFT = 24;
const int GREEN_CH_SHIFT = 16;
const int BLUE_CH_SHIFT = 8;

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

void cb(void *ctx, int x, int y, int u0, int u1, int u2, int det)
{
	uintptr_t *ptr = (uintptr_t *)ctx;
	Canvas *canvas = (Canvas *)ptr[0];
	uint32_t *gradient = (uint32_t *)ptr[1];

	float alpha = (float)u0 / (float)det;
	float beta = (float)u1 / (float)det;
	float gamma = (float)u2 / (float)det;

	unsigned char r0 = get_red_ch(gradient[0]);
	unsigned char r1 = get_red_ch(gradient[1]);
	unsigned char r2 = get_red_ch(gradient[2]);

	unsigned char g0 = get_green_ch(gradient[0]);
	unsigned char g1 = get_green_ch(gradient[1]);
	unsigned char g2 = get_green_ch(gradient[2]);

	unsigned char b0 = get_blue_ch(gradient[0]);
	unsigned char b1 = get_blue_ch(gradient[1]);
	unsigned char b2 = get_blue_ch(gradient[2]);

	unsigned char r = alpha * r0 + beta * r1 + gamma * r2;
	unsigned char g = alpha * g0 + beta * g1 + gamma * g2;
	unsigned char b = alpha * b0 + beta * b1 + gamma * b2;

	uint32_t color = make_rgbx(r, g, b);

	canvas_set_pixel(canvas, x, y, color);
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

	int x0, y0;
	int x1, y1;
	int x2, y2;

	uint32_t gradient[3];

	x0 = 0.50f * WIDTH; y0 = 0.25f * HEIGHT;
	x1 = 0.25f * WIDTH; y1 = 0.75f * HEIGHT;
	x2 = 0.75f * WIDTH; y2 = 0.75f * HEIGHT;

	gradient[0] = make_rgbx(255, 0, 0);
	gradient[1] = make_rgbx(0, 255, 0);
	gradient[2] = make_rgbx(0, 0, 255);

	uintptr_t ptr[2] = { (uintptr_t)&canvas, (uintptr_t)&gradient };

	raster_triangle(
		x0, y0,
		x1, y1,
		x2, y2,
		(void *)&ptr,
		cb
		);

	FILE *output = fopen("output.ppm", "wb");

	ppm_write(output, WIDTH, HEIGHT, (void *)&canvas, fetcher);

	free(pixels);
	fclose(output);

	return 0;
}

