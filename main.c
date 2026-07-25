#include "canvas.h"
#include "raster.h"
#include <math.h>
#include <SDL2/SDL.h>

typedef struct vec3f {
	float x, y, z;
} Vec3f;

Vec3f make_vec3f(float x, float y, float z)
{
	Vec3f ret = { x, y , z};

	return ret;
}

Vec3f vec3f_translate(Vec3f vec, Vec3f delta)
{
	Vec3f ret = {
		vec.x + delta.x,
		vec.y + delta.y,
		vec.z + delta.z
	};

	return ret;
}

Vec3f vec3f_rotate_by_y_axis(Vec3f vec, float angle)
{
	float x = vec.x;
	float y = vec.y;
	float z = vec.z;
	float c = cosf(angle);
	float s = sinf(angle);

	Vec3f ret = {
		x * c + z * s,
		y,
		x * (-s) + z * c
	};

	return ret;
}

Vec3f vec3f_mul_scalar(Vec3f vec, float scalar)
{
	Vec3f ret = {
		vec.x * scalar,
		vec.y * scalar,
		vec.z * scalar
	};

	return ret;
}

Vec3f vec3f_div_scalar(Vec3f vec, float scalar)
{
	return vec3f_mul_scalar(vec, 1.0f / scalar);
}

const Vec3f cube[] = {
	{ 0.5f, 0.5f, 0.5f },
	{ -0.5f, 0.5f, 0.5f },
	{ -0.5f, -0.5f, 0.5f },
	{ 0.5f, -0.5f, 0.5f },

	{ 0.5f, 0.5f, -0.5f },
	{ -0.5f, 0.5f, -0.5f },
	{ -0.5f, -0.5f, -0.5f },
	{ 0.5f, -0.5f, -0.5f },
};

const int lines_indices[][3] = {
	{ 1, 3, 4 },
	{ 0, 2, 5 },
	{ 1, 3, 6 },
	{ 2, 0, 7 },

	{ 5, 7, 0 },
	{ 4, 6, 1 },
	{ 5, 7, 2 },
	{ 6, 4, 3 }
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

Vec3f projection(Vec3f vec)
{
	return vec3f_div_scalar(vec, vec.z);
}

float norm(float val, float min_val, float max_val)
{
	return (val - min_val) / (max_val - min_val);
}

Vec3f viewport(Vec3f vec)
{
	Vec3f ret = {
		.x = norm(vec.x, -1.0f, 1.0f),
		.y = norm(vec.y, -1.0f, 1.0f)
	};

	ret.x *= WIDTH;
	ret.y = (1 - ret.y) * HEIGHT;

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

int main(int argc, char **argv)
{
	SDL_Window *window;
	SDL_Surface *window_surface;
	SDL_Surface *surface;

	Canvas canvas;

	SDL_Init( SDL_INIT_VIDEO );

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
	float angle = 0;

	uint32_t color = rand_color();

	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		if (SDL_GetTicks() - last_tick < (1.0f / FPS * 1000)) {
			continue;
		}

		last_tick = SDL_GetTicks();

		Vec3f world[8];

		for (int i = 0; i < 8; i++) {
			// Putting the cube a little away to fit in [-1, 1] and avoid division by 0
			world[i] = vec3f_rotate_by_y_axis(cube[i], angle);
			world[i] = vec3f_translate(world[i], make_vec3f(0, 0, 2));
			world[i] = projection(world[i]);
			world[i] = viewport(world[i]);
		}

		for (int p = 0; p < 8; p++) {
			uintptr_t ptr[2] = { (uintptr_t)&canvas, (uintptr_t)&color};

			for (int l = 0; l < 3; l++) {
				Vec3f v = world[lines_indices[p][l]];

				raster_line(
					world[p].x, world[p].y,
					v.x, v.y,
					(void *)&ptr,
					cb
					);
			}
		}

		// One full rotation around y axis each 5 seconds
		angle += 2 * M_PI / FPS / 5.0f;

		SDL_BlitSurface(surface, 0, window_surface, 0);
		SDL_UpdateWindowSurface(window);

		canvas_fill(&canvas, make_rgbx(0, 0, 0));
	}

	SDL_FreeSurface(surface);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

