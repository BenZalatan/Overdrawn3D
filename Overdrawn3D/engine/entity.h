#pragma once

#include "../geo/vector.h"
#include "renderer.h"

class entity_t
{
public:

	entity_t()
	{
	}
	entity_t(vec3_t loc, vec3_t rot, vec3_t size, vec3_t col = vec3_t(1, 1, 1))
	{
		location = loc;
		rotation = rot;
		scale = size;

		renderer.color = col;
	}

	vec3_t location, rotation, scale;
	renderer_t renderer;
};

///// PHYSICS /////

class physics_t
{
public:

	bool enabled;
	vec3_t velocity, drag;

	bool isGrounded() { return grounded; }

	void run(entity_t *obj)
	{
		if(!enabled) return;

		bool x, y, z;
		for(uint32_t i; i < WORLD_OBJECT_COUNT; i++)
		{
			if(!intersects(obj, world.objects[i], vec3_t(velocity.x, 0, 0))) x = false;
			if(!intersects(obj, world.objects[i], vec3_t(0, velocity.y, 0))) y = false;
			if(!intersects(obj, world.objects[i], vec3_t(0, 0, velocity.z))) z = false;
		}

		if(x) obj->location.x += velocity.x;

		if(y)
		{
			obj->location.y += velocity.y;
			grounded = false;
		}
		else grounded = true;

		if(z) obj->location.z += velocity.z;
	}

private:

	bool grounded;

	bool intersects(entity_t *obj1, entity_t *obj2, vec3_t offset = vec3_t(0, 0, 0))
	{
		obj1->location.x -= obj1->scale.x / 2.0 - offset.x;
		obj1->location.y -= obj1->scale.y / 2.0 - offset.y;
		obj1->location.z -= obj1->scale.z / 2.0 - offset.z;

		obj2->location.x -= obj2->scale.x / 2.0;
		obj2->location.y -= obj2->scale.y / 2.0;
		obj2->location.z -= obj2->scale.z / 2.0;

		return
			(obj1->location.x + obj1->scale.x >= obj2->location.x && obj2->location.x + obj2->scale.x >= obj1->location.x) && 
			(obj1->location.y + obj1->scale.y >= obj2->location.y && obj2->location.y + obj2->scale.y >= obj1->location.y) && 
			(obj1->location.z + obj1->scale.z >= obj2->location.z && obj2->location.z + obj2->scale.z >= obj1->location.z);
	}
};