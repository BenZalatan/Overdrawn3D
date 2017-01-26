#pragma once

#include "entity.h"

class physics_t
{
public:

	bool enabled;
	vec3_t velocity, drag;

	bool grounded;
};