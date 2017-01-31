#pragma once

#include <math.h>

#include <stdio.h>

#include "glfw3.h"
#include "entity.h"
#include "../window.h"

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

		drawLights = false;
	}

	bool drawLights;

	float moveSpeed;
	bool movingForward, movingBackward, movingLeft, movingRight;

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

	void init();
	void draw();

private:

	float nearClip, farClip, fov;

	void buildPerspective()
	{
		float
			fovy = RAD_TO_DEG * 2 * atan(tan(fov * DEG_TO_RAD / 2) * ((float)glfwGetVideoMode(glfwGetPrimaryMonitor())->height / (float)glfwGetVideoMode(glfwGetPrimaryMonitor())->width)),
			h = tan(fovy / 360 * PI) * nearClip,
			w = h * WIN_WIDTH / WIN_HEIGHT;
	
		glFrustum(-w, w, -h, h, nearClip, farClip);
	}
};

camera_t camera;