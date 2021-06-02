#ifndef APP_MATH_H

#include <math.h>
#include "app.h"

#define PI 3.1415926535897f

typedef struct vector2 vector2;
struct vector2
{
	float32 X;
	float32 Y;
};

typedef struct vector3 vector3;
struct vector3
{
	float32 X;
	float32 Y;
	float32 Z;
};

#include "app_math.cpp"

#define APP_MATH_H
#endif