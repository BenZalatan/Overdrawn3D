#pragma once

#include <stdio.h>
#include <stdint.h>

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

	vec3_t addby(float v)
	{
		return vec3_t(x + v, y + v, z + v);
	}
	vec3_t add(vec3_t v)
	{
		return vec3_t(x + v.x, y + v.y, z + v.z);
	}

	vec3_t subtractby(float v)
	{
		return vec3_t(x - v, y - v, z - v);
	}
	vec3_t subtract(vec3_t v)
	{
		return vec3_t(x - v.x, y - v.y, z - v.z);
	}

	vec3_t multiplyby(float v)
	{
		return vec3_t(x * v, y * v, z * v);
	}
	vec3_t multiply(vec3_t v)
	{
		return vec3_t(x * v.x, y * v.y, z * v.z);
	}

	vec3_t divideby(float v)
	{
		return vec3_t(x / v, y / v, z / v);
	}
	vec3_t divide(vec3_t v)
	{
		return vec3_t(x / v.x, y / v.y, z / v.z);
	}

	vec3_t cross(vec3_t v)
	{
		return vec3_t(
				y * v.z - z * v.y,
                z * v.x - x * v.z,
				x * v.y - y * v.x);
	}

	float length()
	{
		return sqrt(x * x + y * y + z * z);
	}

	vec3_t normalize()
	{
		float len = length();

		if(len == 0.0) len = 1.0;

		return vec3_t(x / len, y / len, z / len);
	}

	bool equals(vec3_t v)
	{
		return (x == v.x && y == v.y && z == v.z);
	}
};

vec3_t generateNormal(vec3_t *verts)
{
	//verts length must be 3 (tri)
	return verts[0].subtract(verts[1]).cross(verts[1].subtract(verts[2])).normalize();
}