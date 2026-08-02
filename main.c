#include "canvas.h"
#include "raster.h"
#include "vec.h"
#include "mat.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

const Vec3 cube_vertices[] = {
	{ 0.5f, 0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f },
	{ 0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
};

const int cube_edges[][2] = {
	{ 0, 1 }, { 0, 3 }, { 0, 4 },
	{ 1, 2 }, { 1, 5 },
	{ 2, 3 }, { 2, 6 },
	{ 3, 7 },
	{ 4, 5 }, { 4, 7 },
	{ 5, 6 },
	{ 6, 7 }
};

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

	if (canvas_in_bounds(canvas, x, y)) {
		canvas_set_pixel(canvas, x, y, color);
	}
}

float norm(float val, float min_val, float max_val)
{
	return (val - min_val) / (max_val - min_val);
}

Vec4 projection(Vec4 vec)
{
	float z = vec.z; 
	Vec4 ret = { vec.x / z, vec.y / z, vec.z / z, vec.w / z };

	return ret;
}

Vec4 viewport(Vec4 vec)
{
	Vec4 ret = {
		.x = norm(vec.x, -1.0f, 1.0f),
		.y = norm(vec.y, -1.0f, 1.0f),
		.z = vec.z,
		.w = vec.w
	};

	ret.x *= WIDTH;
	ret.y = (1.0f - ret.y) * HEIGHT;

	return ret;
}

Vec4 vec3_to_vec4(Vec3 v, float w)
{
	Vec4 vec = { v.x, v.y, v.z, w };

	return vec;
}

Vec4 mat4_mul_vec4(Mat4 m, Vec4 v)
{
	Vec4 ret = {
		.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w,
		.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w,
		.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w,
		.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w
	};

	return ret;
}

uint32_t rand_color()
{
	unsigned char r, g, b;

	r = rand() % 256;
	g = rand() % 256;
	b = rand() % 256;

	return make_rgbx(r, g, b);
}

const char *TITLE = "Cube";
const float FPS = 60.0f;

const float pi = 3.141593f;

#define ARRAY_SIZE(ptr) ( sizeof(ptr) / sizeof(*ptr) )

int main(int argc, char **argv)
{
	SDL_Window *window;
	SDL_Surface *window_surface;
	SDL_Surface *surface;

	Canvas canvas;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			TITLE,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WIDTH,
			HEIGHT,
			SDL_WINDOW_SHOWN
			);

	window_surface = SDL_GetWindowSurface(window);

	surface = SDL_CreateRGBSurfaceWithFormat(
			0,
			WIDTH,
			HEIGHT,
			32,
			SDL_PIXELFORMAT_RGBX8888
			);

	canvas_init(&canvas, (uint32_t *)surface->pixels, WIDTH, HEIGHT);

	canvas_fill(&canvas, make_rgbx(0, 0, 0));

	srand((unsigned int)(uintptr_t)main);

	bool running = true;	
	SDL_Event e;
	Uint32 last_tick = 0;
	float angle = 0.0f;

	uint32_t color = rand_color();

	size_t num_vertices = ARRAY_SIZE(cube_vertices);
	size_t num_edges = ARRAY_SIZE(cube_edges);

	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		if (SDL_GetTicks() - last_tick < (1.0f / FPS * 1000.0f)) {
			continue;
		}

		last_tick = SDL_GetTicks();

		Vec4 world[num_vertices];

		// Putting the cube a little away to fit in [-1, 1] and avoid division by 0
		Mat4 model = mat4_mul(mat4_translate(0.0f, 0.0f, 2.0f), mat4_rotate_y(angle));

		for (int v = 0; v < num_vertices; v++) {
			world[v] = mat4_mul_vec4(model, vec3_to_vec4(cube_vertices[v], 1.0f));
			world[v] = projection(world[v]);
			world[v] = viewport(world[v]);
		}

		for (int e = 0; e < num_edges; e++) { 
			const int *edge = cube_edges[e];
			uintptr_t ptr[2] = { (uintptr_t)&canvas, (uintptr_t)&color };

			Vec4 v0 = world[edge[0]];
			Vec4 v1 = world[edge[1]];

			raster_line(
				v0.x, v0.y,
				v1.x, v1.y,
				(void *)&ptr,
				cb
				);
		}

		// One full rotation around y axis each 5 seconds
		angle += 2 * pi / FPS / 5.0f;

		SDL_BlitSurface(surface, 0, window_surface, 0);
		SDL_UpdateWindowSurface(window);

		canvas_fill(&canvas, make_rgbx(0, 0, 0));
	}

	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

