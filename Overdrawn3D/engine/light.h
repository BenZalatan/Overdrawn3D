#pragma once

#include <stdint.h>

#include "glfw3.h"
#include "entity.h"

#define LIGHT_COUNT 8

class light_t : public entity_t
{
public:

	light_t()
	{
		diffuse = vec3_t(1, 1, 1);
	}

	vec3_t diffuse; // used for color

	void init(uint8_t index)
	{
		if(!enabled) return;

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0 + index);

		glLightfv(GL_LIGHT0 + index, GL_DIFFUSE, diffuse.toArray(1.0));

		glLightfv(GL_LIGHT0 + index, GL_POSITION, location.toArray(1.0));

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse.toArray(1.0));
	}

	void run(uint8_t index)
	{
		if(!enabled) return;

		glLightfv(GL_LIGHT0 + index, GL_POSITION, location.toArray(1.0));
	}
};