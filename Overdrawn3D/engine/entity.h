#pragma once

#include <math.h>

#include "../geo/vector.h"
#include "renderer.h"
#include "physics.h"

#define PI 3.14159265359
#define RAD_TO_DEG (180 / PI)
#define DEG_TO_RAD (PI / 180)

class entity_t
{
public:

	entity_t()
	{
		enabled = false;
	}
	entity_t(vec3_t loc, vec3_t rot, vec3_t size, vec3_t col = vec3_t(1, 1, 1))
	{
		location = loc;
		rotation = rot;
		scale = size;

		renderer.color = col;

		physics.enabled = false;
		physics.velocity = vec3_t();
		physics.drag = vec3_t(0.5, 1, 0.5);

		enabled = true;
	}

	vec3_t location, rotation, scale;
	renderer_t renderer;
	physics_t physics;
	bool enabled;

	vec3_t forward(float angle = 0.0)
	{
		return vec3_t(
			-cos((rotation.y + 90 + angle) * DEG_TO_RAD),
			0,
			-sin((rotation.y + 90 + angle) * DEG_TO_RAD)
			);
	}

	bool intersects(entity_t obj2, vec3_t offset = vec3_t(0, 0, 0))
	{
		vec3_t loc = location;

		loc.x -= scale.x / 2.0 - offset.x;
		loc.y -= scale.y / 2.0 - offset.y;
		loc.z -= scale.z / 2.0 - offset.z;

		loc.x += offset.x;
		loc.y += offset.y;
		loc.z += offset.z;

		obj2.location.x -= obj2.scale.x / 2.0;
		obj2.location.y -= obj2.scale.y / 2.0;
		obj2.location.z -= obj2.scale.z / 2.0;

		return
			(loc.x + scale.x >= obj2.location.x && obj2.location.x + obj2.scale.x >= loc.x) && 
			(loc.y + scale.y >= obj2.location.y && obj2.location.y + obj2.scale.y >= loc.y) && 
			(loc.z + scale.z >= obj2.location.z && obj2.location.z + obj2.scale.z >= loc.z);
	}

	void runPhysics();
};