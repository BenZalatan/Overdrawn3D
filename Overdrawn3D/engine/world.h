#pragma once

#include <stdint.h>

#include "entity.h"
#include "physics.h"
#include "light.h"

#define WORLD_OBJECT_COUNT 0xFF /* 255 */

class world_t
{
public:

	world_t()
	{
		for(uint16_t i = 0; i < WORLD_OBJECT_COUNT; i++) objects[i] = entity_t();
	}

	entity_t objects[WORLD_OBJECT_COUNT];
	light_t lights[LIGHT_COUNT];
};

world_t world;

/******/

#define WORLD_GRAVITY 0.005

void entity_t::runPhysics()
{
	if(!physics.enabled) return;

	bool x = true, y = true, z = true;
	for(uint32_t i = 0; i < WORLD_OBJECT_COUNT; i++)
	{
		if(!world.objects[i].enabled || world.objects[i].location.equals(location)) continue;

		if(intersects(world.objects[i], vec3_t(physics.velocity.x, 0, 0)))
		{
			x = false;
			if(world.objects[i].physics.pushable)
			{
				world.objects[i].physics.velocity.x += physics.velocity.x;
			}
		}
		if(intersects(world.objects[i], vec3_t(0, physics.velocity.y, 0)))
		{
			y = false;
			if(world.objects[i].physics.pushable)
			{
				world.objects[i].physics.velocity.y += physics.velocity.y;
			}
		}
		if(intersects(world.objects[i], vec3_t(0, 0, physics.velocity.z)))
		{
			z = false;
			if(world.objects[i].physics.pushable)
			{
				world.objects[i].physics.velocity.z += physics.velocity.z;
			}
		}
	}

	if(x) location.x += physics.velocity.x;

	if(y)
	{
		location.y += physics.velocity.y;
		physics.grounded = false;
	}
	else
	{
		physics.grounded = true;
		physics.velocity.y = 0;
	}

	if(z) location.z += physics.velocity.z;

	physics.velocity.x *= physics.drag.x;
	physics.velocity.y *= physics.drag.y;
	physics.velocity.z *= physics.drag.z;

	physics.velocity.y -= WORLD_GRAVITY;

	// TEMPORARY; ONLY FOR TESTING

	if(location.y <= -50.0)
		location = vec3_t(0, 50, 0);
}

void runWorldPhysics()
{
	for(uint32_t i = 0; i < WORLD_OBJECT_COUNT; i++)
		world.objects[i].runPhysics();
}