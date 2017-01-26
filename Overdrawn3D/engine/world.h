#pragma once

#include <stdint.h>

#include "entity.h"

#define WORLD_OBJECT_COUNT 0xFF /* 255 */

class world_t
{
public:

	world_t()
	{
		for(uint16_t i = 0; i < WORLD_OBJECT_COUNT; i++) objects[i] = entity_t();
	}

	entity_t objects[WORLD_OBJECT_COUNT];
};

world_t world;