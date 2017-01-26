#pragma once

#include <stdio.h>

class vec3_t
{
public:

	vec3_t()
	{
		x = y = z = 0;
	}
	vec3_t(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	float x, y, z;

	float *toArray(float w = 1.0)
	{
		float ret[] = { x, y, z, w };
		return ret;
	}

	void print()
	{
		printf("(%f, %f, %f)\n", x, y, z);
	}

	vec3_t multiplyby(float v)
	{
		return vec3_t(x * v, y * v, z * v);
	}
	vec3_t multiply(vec3_t v)
	{
		return vec3_t(x * v.x, y * v.y, z * v.z);
	}
};