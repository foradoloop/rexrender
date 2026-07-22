#ifndef RASTER_H
#define RASTER_H

void raster_line(
		int x0, int y0,
		int x1, int y1,
		void *ctx,
		void (*cb)(void *ctx, int x, int y)
		);

#endif

