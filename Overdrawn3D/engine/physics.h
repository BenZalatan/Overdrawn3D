#pragma once

#include "entity.h"

class physicsmat_t
{
public:

	physicsmat_t()
	{
		bounciness = slipperiness = heaviness = 0;
	}
	physicsmat_t(float bounce, float slip, float weight)
	{
		bounciness = bounce;
		slipperiness = slip;
		heaviness = weight;
	}

	float bounciness, slipperiness, heaviness;
};

class physics_t
{
public:

	physics_t()
	{
		enabled = false;
		velocity = vec3_t();
		addedVelocity = vec3_t();
		drag = vec3_t(0.5, 1, 0.5);
		grounded = false;
		pushable = false;

		lastGroundedObject = 0;

		material = physicsmat_t();
		inheritMaterial = true;

		movingPlatform = true;
		useGravity = true;
	}

	bool enabled;
	vec3_t velocity, addedVelocity /* automatic */, drag;

	bool grounded /* auto */, pushable, inheritMaterial, movingPlatform, useGravity;

	uint32_t lastGroundedObject;

	physicsmat_t material, inheritedMaterial /* automatic */;

	vec3_t fullVelocity()
	{
		return velocity.add(addedVelocity);
	}
};