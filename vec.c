#include "vec.h"
#include <math.h>

Vec3 vec3_make(float x, float y, float z)
{
	Vec3 vec = { x, y, z };

	return vec;
}

Vec3 vec3_scale(Vec3 v, float s)
{
	return vec3_make(v.x * s, v.y * s, v.z * s);
}

Vec3 vec3_add(Vec3 a, Vec3 b)
{
	return vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
	return vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 a, Vec3 b)
{
	return vec3_make(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 vec3_div(Vec3 a, Vec3 b)
{
	return vec3_make(a.x / b.x, a.y / b.y, a.z / b.z);
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
	return vec3_make(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
		);
}

float vec3_length(Vec3 v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_normalize(Vec3 v)
{
	return vec3_scale(v, 1.0f / vec3_length(v));
}

