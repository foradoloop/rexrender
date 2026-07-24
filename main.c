#include "canvas.h"
#include "raster.h"
#include "ppm.h"
#include <stdlib.h>
#include <float.h>

// Use -D instead
//#define TOBJ_ENABLE_FILE_IO
#include "tiny_obj_c.h"

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
	return ( color & RED_CH_MASK ) >> RED_CH_SHIFT;
}

unsigned char get_green_ch(uint32_t color)
{
	return ( color & GREEN_CH_MASK ) >> GREEN_CH_SHIFT;
}

unsigned char get_blue_ch(uint32_t color)
{
	return ( color & BLUE_CH_MASK ) >> BLUE_CH_SHIFT;
}

void cb(void *ctx, int x, int y)
{
	uintptr_t *ptr = (uintptr_t *)ctx;
	Canvas *canvas = (Canvas *)ptr[0];
	uint32_t color = *(uint32_t *)ptr[1];

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

void tobj_get_vertices(tobj_scene_f *scene, tobj_index idx, float *x, float *y, float *z)
{
	*x = scene->attrib.vertices.ptr[3 * idx.vertex_index + 0];
	*y = scene->attrib.vertices.ptr[3 * idx.vertex_index + 1];
	*z = scene->attrib.vertices.ptr[3 * idx.vertex_index + 2];
}

float norm(float x, float min_x, float max_x)
{
	return (x - min_x) / (max_x - min_x);
}

float min_x = FLT_MAX;
float min_y = FLT_MAX;
float max_x = -FLT_MAX;
float max_y = -FLT_MAX;

// Orthogonal
void projection(float *sx, float *sy, float x, float y)
{
	*sx = norm(x, min_x, max_x) * WIDTH;
	*sy = HEIGHT * (1 - norm(y, min_y, max_y));
}

int main(int argc, char **argv)
{
	Canvas canvas;
	uint32_t *pixels = malloc(sizeof(uint32_t) * WIDTH * HEIGHT);

	char *obj_path = argv[1];

	canvas_init(&canvas, pixels, WIDTH, HEIGHT);

	canvas_fill(&canvas, make_rgbx(0, 0, 0));

	srand((unsigned int)(uintptr_t)(main));

	tobj_scene_f scene;
	tobj_load_config cfg = tobj_default_config();
	tobj_diag diag = {0};

	tobj_load_obj_from_file_f(&scene, obj_path, &cfg, &diag);

	for (size_t s = 0; s < scene.num_shapes; s++) {
		const tobj_mesh_f *mesh = &scene.shapes[s].mesh;
		for (size_t i = 0; i < mesh->num_indices; i++) {
			float x, y, z;

			tobj_get_vertices(&scene, mesh->indices[i], &x, &y, &z);

			if (x < min_x) min_x = x;
			if (y < min_y) min_y = y;
			if (x > max_x) max_x = x;
			if (y > max_y) max_y = y;
		}
	}

	for (size_t s = 0; s < scene.num_shapes; s++) {
		const tobj_mesh_f *mesh = &scene.shapes[s].mesh;
		for (size_t i = 0; i < mesh->num_indices; i += 3) {
			float x0, y0, z0;
			float x1, y1, z1;
			float x2, y2, z2;

			tobj_get_vertices(&scene, mesh->indices[i + 0], &x0, &y0, &z0);
			tobj_get_vertices(&scene, mesh->indices[i + 1], &x1, &y1, &z1);
			tobj_get_vertices(&scene, mesh->indices[i + 2], &x2, &y2, &z2);

			float sx0, sy0;
			float sx1, sy1;
			float sx2, sy2;
			
			projection(&sx0, &sy0, x0, y0);
			projection(&sx1, &sy1, x1, y1);
			projection(&sx2, &sy2, x2, y2);

			float s[6] = { sx0, sy0, sx1, sy1, sx2, sy2 };

			unsigned r, g, b;

			r = rand() % 256;
			g = rand() % 256;
			b = rand() % 256;

			uint32_t color = make_rgbx(r, g, b);
	
			uintptr_t ptr[2] = { (uintptr_t)&canvas, (uintptr_t)&color};

			for (int l = 0; l < 3; l++) {
				// Draw wireframe triangle
				raster_line(
					s[2 * l], s[2 * l + 1],
					s[(2 * (l + 1)) % 6], s[(2 * (l + 1) + 1) % 6],
					(void *)&ptr,
					cb
					);
			}

		}
	}

	tobj_scene_free_f(&scene);
	tobj_diag_free(&diag, NULL);

	FILE *output = fopen("output.ppm", "wb");

	ppm_write(output, WIDTH, HEIGHT, (void *)&canvas, fetcher);

	free(pixels);
	fclose(output);

	return 0;
}

