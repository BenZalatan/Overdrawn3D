#pragma once

#include <math.h>

#include <stdio.h>

#include "glfw3.h"
#include "entity.h"
#include "../window.h"
#include "world.h"

class camera_t : public entity_t
{
public:

	camera_t()
	{
		camera_t(90);
	}
	camera_t(float _fov)
	{
		nearClip = 0.001;
		farClip = 1000.0;
		fov = _fov;

		moveSpeed = 0.1;
		movingForward = movingBackward = movingLeft = movingRight = false;
	}

	float moveSpeed;
	bool movingForward, movingBackward, movingLeft, movingRight;

	void init()
	{
		glEnable(GL_NORMALIZE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_COLOR_MATERIAL);
		
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		glMatrixMode(GL_PROJECTION);

		buildPerspective();
		glScalef(1.0f, 1.0f, 1.0f);

		glMatrixMode(GL_MODELVIEW);

		for(uint8_t i = 0; i < LIGHT_COUNT; i++)
			world.lights[i].init(i);
	}
	
	void draw()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glRotatef(rotation.x, 1, 0, 0);
		glRotatef(rotation.y, 0, 1, 0);
		glRotatef(rotation.z, 0, 0, 1);

		glTranslatef(-location.x, -location.y, -location.z);

		glShadeModel(GL_SMOOTH);

		for(uint8_t i = 0; i < LIGHT_COUNT; i++)
			world.lights[i].run(i);

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

	void run()
	{
		if(movingForward)
		{
			float angle = 0.0;
			if(movingLeft) angle -= 45.0;
			if(movingRight) angle += 45.0;

			physics.velocity.x = forward(angle).multiplyby(moveSpeed).x;
			physics.velocity.z = forward(angle).multiplyby(moveSpeed).z;
		}
		else if(movingBackward)
		{
			float angle = 0.0;
			if(movingLeft) angle -= 45.0;
			if(movingRight) angle += 45.0;

			physics.velocity.x = forward(-angle).multiplyby(-moveSpeed).x;
			physics.velocity.z = forward(-angle).multiplyby(-moveSpeed).z;
		}
		else
		{
			if(movingLeft)
			{
				physics.velocity.x = forward(-90).multiplyby(moveSpeed).x;
				physics.velocity.z = forward(-90).multiplyby(moveSpeed).z;
			}
			if(movingRight)
			{
				physics.velocity.x = forward(90).multiplyby(moveSpeed).x;
				physics.velocity.z = forward(90).multiplyby(moveSpeed).z;
			}
		}
	}

private:

	float nearClip, farClip, fov;

	void buildPerspective()
	{
		float
			fovy = RAD_TO_DEG * 2 * atan(tan(fov * DEG_TO_RAD / 2) * ((float)WIN_HEIGHT / (float)WIN_WIDTH)),
			h = tan(fovy / 360 * PI) * nearClip,
			w = h * WIN_WIDTH / WIN_HEIGHT;
	
		glFrustum(-w, w, -h, h, nearClip, farClip);
	}
};

camera_t camera;