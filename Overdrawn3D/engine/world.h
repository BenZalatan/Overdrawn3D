#pragma once

#include <stdint.h>
#include <typeinfo>

#include "entity.h"
#include "physics.h"
#include "light.h"

#include "world.h"

#define WORLD_OBJECT_COUNT 0xFF /* 255 */

#define WORLD_COLLISION_STEP_COUNT 4

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

	if(!physics.inheritMaterial)
		physics.inheritedMaterial = physics.material;

	physics.addedVelocity = vec3_t();

	bool x = true, y = true, z = true;
	for(uint32_t i = 0; i < WORLD_OBJECT_COUNT; i++)
	{
		if(!world.objects[i].enabled || world.objects[i].location.equals(location)) continue;

		bool stepx, stepy, stepz;
		stepx = stepy = stepz = true;

		//stepx etc are probably not needed and can be replaced with x etc

		for(uint8_t j = 0; j < WORLD_COLLISION_STEP_COUNT; j++)
		{
			if(intersects(world.objects[i], vec3_t(j * physics.velocity.x / WORLD_COLLISION_STEP_COUNT, 0, 0)))
				stepx = false;
			if(intersects(world.objects[i], vec3_t(0, j * physics.velocity.y / WORLD_COLLISION_STEP_COUNT, 0)))
				stepy = false;
			if(intersects(world.objects[i], vec3_t(0, 0, j * physics.velocity.z / WORLD_COLLISION_STEP_COUNT)))
				stepz = false;
		}

		if(!stepx)
		{
			x = false;
			if(world.objects[i].physics.pushable)
			{
				world.objects[i].physics.velocity.x += physics.fullVelocity().x * (1 - physics.inheritedMaterial.heaviness);
				physics.velocity.x = physics.velocity.x * (1 - physics.inheritedMaterial.heaviness);
			}
		}
		if(!stepy)
		{
			y = false;
			physics.lastGroundedObject = i;

			if(physics.inheritMaterial)
				physics.inheritedMaterial = world.objects[i].physics.material;

			if(physics.movingPlatform && world.objects[i].physics.enabled)
				physics.addedVelocity = world.objects[i].physics.fullVelocity();
		}
		if(!stepz)
		{
			z = false;
			if(world.objects[i].physics.pushable)
			{
				world.objects[i].physics.velocity.z += physics.fullVelocity().z * (1 - physics.inheritedMaterial.heaviness);
				physics.velocity.z = physics.velocity.z * (1 - physics.inheritedMaterial.heaviness);
			}
		}
	}

	if(x) location.x += physics.fullVelocity().x;
	else physics.velocity.x *= -physics.material.bounciness;

	if(y)
	{
		location.y += physics.fullVelocity().y;
		physics.grounded = false;
		physics.inheritedMaterial = physics.material;
	}
	else
	{
		physics.grounded = true;

		location.y =
			world.objects[physics.lastGroundedObject].location.y +
			world.objects[physics.lastGroundedObject].scale.y / 2.0 +
			scale.y / 2.0 +
			0.001;
		
		physics.velocity.y *= -physics.inheritedMaterial.bounciness;

		if(physics.velocity.y != 0.0)
			location.y += physics.fullVelocity().y;
	}

	if(z) location.z += physics.fullVelocity().z;
	else physics.velocity.z *= -physics.material.bounciness;

	physics.velocity.x *= physics.drag.x * (1 + physics.inheritedMaterial.slipperiness) > 1.0 ? 1.0 : physics.drag.x * (1 + physics.inheritedMaterial.slipperiness);
	physics.velocity.y *= physics.drag.y * (1 + physics.inheritedMaterial.slipperiness) > 1.0 ? 1.0 : physics.drag.y * (1 + physics.inheritedMaterial.slipperiness);
	physics.velocity.z *= physics.drag.z * (1 + physics.inheritedMaterial.slipperiness) > 1.0 ? 1.0 : physics.drag.z * (1 + physics.inheritedMaterial.slipperiness);

	if(physics.useGravity)
		physics.velocity.y -= WORLD_GRAVITY * (1 + physics.material.heaviness);

	// TEMPORARY; ONLY FOR TESTING

	if(location.y <= -50.0)
	{
		location = vec3_t(0, 50, 0);
		physics.velocity = vec3_t();
	}
}

void runWorldPhysics()
{
	for(uint32_t i = 0; i < WORLD_OBJECT_COUNT; i++)
	{
		if(!world.objects[i].physics.enabled) continue;

		world.objects[i].runPhysics();
	}
}

// CAMERA STUFF

void camera_t::init()
{
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glDepthFunc(GL_LEQUAL);
		
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glMatrixMode(GL_PROJECTION);

	buildPerspective();
	glScalef(1.0f, 1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);

	for(uint8_t i = 0; i < LIGHT_COUNT; i++)
		world.lights[i].init(i);
}
	
void camera_t::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glRotatef(rotation.x, 1, 0, 0);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.z, 0, 0, 1);

	glTranslatef(-location.x, -location.y - scale.y / 2, -location.z);

	glShadeModel(GL_FLAT);

	for(uint8_t i = 0; i < LIGHT_COUNT; i++)
	{
		world.lights[i].run(i);

		if(drawLights)
		{
			glBegin(GL_QUADS);

			glColor3f(world.lights[i].renderer.color.x, world.lights[i].renderer.color.y, world.lights[i].renderer.color.z);

			vec3_t loc = world.lights[i].location;

			loc.x -= world.lights[i].scale.x / 2.0;
			loc.y -= world.lights[i].scale.y / 2.0;
			loc.z -= world.lights[i].scale.z / 2.0;

			// BOTTOM
			glNormal3f(0, -1, 0);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z);

			// TOP
			glNormal3f(0, 1, 0);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z);

			// LEFT
			glNormal3f(-1, 0, 0);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z);

			// RIGHT
			glNormal3f(1, 0, 0);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z);

			// BACK
			glNormal3f(0, 0, -1);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z);

			// FRONT
			glNormal3f(0, 0, 1);
			glVertex3f(
				loc.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x + world.lights[i].scale.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);
			glVertex3f(
				loc.x,
				loc.y + world.lights[i].scale.y,
				loc.z + world.lights[i].scale.z);

			glEnd();
		}
	}

	for(uint16_t i = 0; i < WORLD_OBJECT_COUNT; i++)
	{
		glBegin(GL_QUADS);

		glColor3f(world.objects[i].renderer.color.x, world.objects[i].renderer.color.y, world.objects[i].renderer.color.z);

		/*

		DRAWING AT CORNER (vs drawing in middle):

		glVertex3f(
			world.objects[i].location.x,
			world.objects[i].location.y,
			world.objects[i].location.z);
		glVertex3f(
			world.objects[i].location.x,
			world.objects[i].location.y,
			world.objects[i].location.z + world.objects[i].scale.z);
		glVertex3f(
			world.objects[i].location.x + world.objects[i].scale.x,
			world.objects[i].location.y,
			world.objects[i].location.z + world.objects[i].scale.z);
		glVertex3f(
			world.objects[i].location.x + world.objects[i].scale.x,
			world.objects[i].location.y,
			world.objects[i].location.z);
			
		*/

		vec3_t loc = world.objects[i].location;

		loc.x -= world.objects[i].scale.x / 2.0;
		loc.y -= world.objects[i].scale.y / 2.0;
		loc.z -= world.objects[i].scale.z / 2.0;

		// BOTTOM
		glNormal3f(0, -1, 0);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z);

		// TOP
		glNormal3f(0, 1, 0);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z);

		// LEFT
		glNormal3f(-1, 0, 0);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z);

		// RIGHT
		glNormal3f(1, 0, 0);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z);

		// BACK
		glNormal3f(0, 0, -1);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z);

		// FRONT
		glNormal3f(0, 0, 1);
		glVertex3f(
			loc.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x + world.objects[i].scale.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);
		glVertex3f(
			loc.x,
			loc.y + world.objects[i].scale.y,
			loc.z + world.objects[i].scale.z);

		glEnd();
	}
}