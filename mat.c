#include "mat.h"
#include <math.h>

Mat4 mat4_mul(Mat4 a, Mat4 b)
{
	Mat4 ret;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float sum = 0.0f;

			for (int k = 0; k < 4; k++) {
				sum += a.m[i][k] * b.m[k][j];
			}

			ret.m[i][j] = sum;
		}
	}

	return ret; 
}

Mat4 mat4_translate(float tx, float ty, float tz)
{
	Mat4 ret = {
		.m = {
			{ 1.0f, 0.0f, 0.0f, tx },
			{ 0.0f, 1.0f, 0.0f, ty },
			{ 0.0f, 0.0f, 1.0f, tz },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};

	return ret;
}

Mat4 mat4_rotate_x(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);

	Mat4 ret = {
		.m = {
			{ 1.0f, 0.0f, 0.0f, 0.0f },
			{ 0.0f, c, -s, 0.0f },
			{ 0.0f, s, c, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};

	return ret;
}

Mat4 mat4_rotate_y(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);

	Mat4 ret = {
		.m = {
			{ c, 0.0f, s, 0.0f },
			{ 0.0f, 1.0f, 0.0f, 0.0f },
			{ -s, 0.0f, c, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};

	return ret;
}

Mat4 mat4_rotate_z(float angle)
{
	float c = cosf(angle);
	float s = sinf(angle);

	Mat4 ret = {
		.m = {
			{ c, -s, 0.0f, 0.0f },
			{ s, c, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f, 1.0f }
		}
	};

	return ret;
}

