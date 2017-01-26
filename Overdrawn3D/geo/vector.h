#pragma once

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
};