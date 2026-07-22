#include "canvas.h"

#define CANVAS_AT(canvas, x, y) \
	( (canvas)->pixels[ (canvas)->width * (y) + (x) ] )

void canvas_init(Canvas *canvas, uint32_t *pixels, int width, int height)
{
	canvas->pixels = pixels;
	canvas->width = width;
	canvas->height = height;
}

bool canvas_in_bounds(Canvas *canvas, int x, int y)
{
	return 0 <= x && x < canvas->width && 0 <= y && y < canvas->height;
}

void canvas_set_pixel(Canvas *canvas, int x, int y, uint32_t color)
{
	CANVAS_AT(canvas, x, y) = color;
}

uint32_t canvas_get_pixel(Canvas *canvas, int x, int y)
{
	return CANVAS_AT(canvas, x, y);
}

void canvas_fill(Canvas *canvas, uint32_t color)
{
	for (int y = 0; y < canvas->height; y++) {
		for (int x = 0; x < canvas->width; x++) {
			CANVAS_AT(canvas, x, y) = color;
		}
	}
}

