#ifndef MAT_H
#define MAT_H

struct mat4 {
	float m[4][4];
};
typedef struct mat4 Mat4;

Mat4 mat4_mul(Mat4 a, Mat4 b);
Mat4 mat4_translate(float tx, float ty, float tz);
Mat4 mat4_rotate_x(float angle);
Mat4 mat4_rotate_y(float angle);
Mat4 mat4_rotate_z(float angle);

#endif

