#include "raster.h"
#include <stdlib.h>
#include <stdbool.h>

#define SIGN(x) ( ( (x) > 0 ) - ( (x) < 0 ) )

#define MIN(a, b) ( (a) < (b) ? (a) : (b) )
#define MAX(a, b) ( (a) > (b) ? (a) : (b) )

void raster_line(
		int x0, int y0,
		int x1, int y1,
		void *ctx,
		void (*cb)(void *ctx, int x, int y)
		)
{
	int *major_coord, *minor_coord;
	int major_delta, minor_delta;
	int major_step, minor_step;

	int x, y;
	int diff_x, diff_y;
	int dx, dy;

	x = x0;
	y = y0;

	diff_x = x1 - x0;
	diff_y = y1 - y0;

	dx = abs(diff_x);
	dy = abs(diff_y);

	if (dx > dy) {
		major_coord = &x;
		minor_coord = &y;
		major_delta = dx;
		minor_delta = dy;
		major_step = SIGN(diff_x);
		minor_step = SIGN(diff_y);
	} else {
		major_coord = &y;
		minor_coord = &x;
		major_delta = dy;
		minor_delta = dx;
		major_step = SIGN(diff_y);
		minor_step = SIGN(diff_x);
	}

	int e = 2 * minor_delta - major_delta;

	for (int i = 0; i <= major_delta; i++) {
		cb(ctx, x, y);

		if (e >= 0) {
			e -= 2 * major_delta;
			(*minor_coord) += minor_step;
		}

		e += 2 * minor_delta;
		(*major_coord) += major_step;
	}
}

int signed_double_area(
		int x0, int y0,
		int x1, int y1,
		int x2, int y2
		)
{
	return ( x0 - x2 ) * ( y1 - y2 ) - ( x1 - x2 ) * ( y0 - y2 );
}

void raster_triangle(
		int x0, int y0,
		int x1, int y1,
		int x2, int y2,
		void *ctx,
		void (*cb)(void *ctx, int x, int y, int u0, int u1, int u2, int det)
		)
{
	int min_x = MIN(MIN(x0, x1), x2);
	int min_y = MIN(MIN(y0, y1), y2);
	int max_x = MAX(MAX(x0, x1), x2);
	int max_y = MAX(MAX(y0, y1), y2);

	int det = signed_double_area(x0, y0, x1, y1, x2, y2);

	for (int y = min_y; y <= max_y; y++) {
		for (int x = min_x; x <= max_x; x++) {
			int u0 = signed_double_area(x, y, x1, y1, x2, y2);
			int u1 = signed_double_area(x, y, x2, y2, x0, y0);
			int u2 = signed_double_area(x, y, x0, y0, x1, y1);

			bool in =
				( SIGN(u0) == SIGN(det) || u0 == 0 ) &&
				( SIGN(u1) == SIGN(det) || u1 == 0 ) &&
				( SIGN(u2) == SIGN(det) || u2 == 0 );

			if (in) {
				cb(ctx, x, y, u0, u1, u2, det);
			}
		}
	}
}

