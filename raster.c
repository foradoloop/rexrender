#include "raster.h"
#include <stdlib.h>

#define SIGN(x) ( ( (x) > 0 ) - ( (x) < 0 ) )

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

