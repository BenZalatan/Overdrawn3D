#pragma once

#include <stdio.h>
#include <Windows.h>

#include "glfw3.h"
#include "window.h"
#include "engine\camera.h"
#include "engine\world.h"

#include "engine/shadows.h"

/* 

KNOWN BUGS:

- objects go through camera (no camera collision detection)
- being inside of an object causes it to move with you (easy to see with pushable, bouncy objects) (will likely be fixed when camera collision is)
- [not a bug] heaviness doesn't work due to how the camera moves
- walking into a wall on top of a moving object causes you to teleport on top of it

NOTES:

- i tried out camera collision (as you can see by the messy world.h), but it caused problems, but ill retry it later

*/

// Input (must be before main, put in different file later)

uint16_t lastIndex = 9;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_W)
			camera.movingForward = true;
		else if (key == GLFW_KEY_S)
			camera.movingBackward = true;
		else if (key == GLFW_KEY_A)
			camera.movingLeft = true;
		else if (key == GLFW_KEY_D)
			camera.movingRight = true;
		else if (key == GLFW_KEY_L)
		{
			world.lights[0].location = camera.location;
			printf("New light location: ");
			world.lights[0].location.print();
		}
		else if(key == GLFW_KEY_C)
		{
			camera.scale.y = 1;
			camera.moveSpeed = 0.025;
		}
		else if(key == GLFW_KEY_R)
			world.objects[1].location = vec3_t(0, 50, 0);
		else if(key == GLFW_KEY_F)
			world.objects[1].physics.velocity.y += 0.25;
		else if(key == GLFW_KEY_LEFT_SHIFT && camera.scale.y != 1)
			camera.moveSpeed = 0.25;
		else if(key == GLFW_KEY_G)
		{
			world.objects[lastIndex] = entity_t(
				camera.location,
				vec3_t(0, 0, 0),
				vec3_t(3, 3, 3),
				vec3_t(1, 0, 0));
			world.objects[lastIndex].physics.enabled = true;
			world.objects[lastIndex].physics.drag = vec3_t(0.5, 1, 0.5);
			world.objects[lastIndex].physics.pushable = true;
			lastIndex++;
		}
		
		else if(key == GLFW_KEY_SPACE && camera.physics.grounded)
			camera.physics.velocity.y += 0.25;
	}
	else if(action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_W)
			camera.movingForward = false;
		else if (key == GLFW_KEY_S)
			camera.movingBackward = false;
		else if (key == GLFW_KEY_A)
			camera.movingLeft = false;
		else if (key == GLFW_KEY_D)
			camera.movingRight = false;
		else if(key == GLFW_KEY_C)
		{
			camera.scale.y = 5;
			camera.location.y += 2;
			camera.moveSpeed = 0.1;
		}
		else if(key == GLFW_KEY_LEFT_SHIFT && camera.scale.y != 1)
			camera.moveSpeed = 0.1;
	}
}

#define MOUSE_SENS 0.1f

float mouseLastX, mouseLastY;
bool mouseIgnore = true;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(mouseIgnore)
	{
		mouseIgnore = FALSE;
		mouseLastX = xpos;
		mouseLastY = ypos;
		return;
	}

	camera.rotation.y += (xpos - mouseLastX) * MOUSE_SENS;
	camera.rotation.x += (ypos - mouseLastY) * MOUSE_SENS;
	if(camera.rotation.x > 90.0) camera.rotation.x = 90.0;
	else if(camera.rotation.x < -90.0) camera.rotation.x = -90.0;

	mouseLastX = xpos;
	mouseLastY = ypos;
}

// Window stuff

GLFWwindow *window;

int main()
{
	if(!glfwInit())
	{
		printf("Failed to initialize GLFW.\n");
		Sleep(ERROR_SLEEP_TIME);
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(
		/*WIN_WIDTH*/ glfwGetVideoMode(glfwGetPrimaryMonitor())->width,
		/*WIN_HEIGHT*/ glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
		"Test",
		glfwGetPrimaryMonitor(),
		NULL);

	if(!window)
	{
		glfwTerminate();
		printf("Failed to create a GLFW window.\n");
		Sleep(ERROR_SLEEP_TIME);
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// Initialize world

	camera = camera_t(110.0);
	camera.location = vec3_t(0, 5, 0);
	camera.scale = vec3_t(1, 5, 1); // camera height is 1/2 scale.y (5 = 2.5 actual height)
	camera.physics.drag = vec3_t(0.5, 1, 0.5);
	camera.physics.enabled = true;
	camera.physics.pushable = true;
	camera.enabled = true;
	camera.drawLights = true;

	world = world_t();
	world.objects[0] = entity_t(
		vec3_t(0, 0, 0),
		vec3_t(0, 0, 0),
		vec3_t(20, 1, 20));
	//
	world.objects[2] = entity_t(
		vec3_t(25, 0, 0),
		vec3_t(0, 0, 0),
		vec3_t(20, 1, 20),
		vec3_t(0, 1, 1));
	world.objects[2].physics.material.slipperiness = 0.97;

	world.objects[5] = entity_t(
		vec3_t(12.5, 0, 0),
		vec3_t(0, 0, 0),
		vec3_t(5, 1, 5));
	
	world.objects[3] = entity_t(
		vec3_t(-25, 0, 0),
		vec3_t(0, 0, 0),
		vec3_t(20, 1, 20),
		vec3_t(1, 0, 1));
	world.objects[3].physics.material.bounciness = 1;

	world.objects[6] = entity_t(
		vec3_t(-12.5, 0, 0),
		vec3_t(0, 0, 0),
		vec3_t(5, 1, 5));

	world.objects[4] = entity_t(
		vec3_t(35, 5, 0),
		vec3_t(0, 0, 0),
		vec3_t(1, 10, 20));

	world.objects[7] = entity_t(
		vec3_t(-35, 5, 0),
		vec3_t(0, 0, 0),
		vec3_t(1, 10, 20));

	world.objects[8] = entity_t(
		vec3_t(0, 0, 21),
		vec3_t(0, 0, 0),
		vec3_t(20, 1, 20));
	world.objects[8].physics.enabled = true;
	world.objects[8].physics.drag = vec3_t(1, 0, 0);
	world.objects[8].physics.useGravity = false;
	//

	world.objects[1] = entity_t(
		vec3_t(5, 20, 5),
		vec3_t(0, 0, 0),
		vec3_t(3, 3, 3),
		vec3_t(1, 0, 0));
	world.objects[1].physics.enabled = true;
	world.objects[1].physics.drag = vec3_t(0.5, 1, 0.5);
	world.objects[1].physics.pushable = true;
	//world.objects[1].physics.material = physicsmat_t(0.25, 0.65, 0.5);
	//world.objects[1].physics.inheritMaterial = false;

	world.lights[0] = light_t();
	//world.lights[0].location = vec3_t(0.5, 3, 1);
	world.lights[0].location = vec3_t(3, 12, 1);
	world.lights[0].diffuse = vec3_t(1, 1, 1);
	world.lights[0].enabled = true;
	world.lights[0].scale = vec3_t(1, 1, 1);
	world.lights[0].renderer.color = vec3_t(1, 1, 1);

	camera.init();
	
	// Main loop

	bool rev = false;

	while (!glfwWindowShouldClose(window))
	{
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / (1 / WIN_MAXFPS);
		lastTime = nowTime;

		while (deltaTime >= 1.0)
		{
			camera.draw();
			camera.run();
			camera.runPhysics();
			runWorldPhysics();

			world.objects[8].physics.velocity.x = 0.025 * (rev ? -1 : 1);
			if(world.objects[8].location.x >= 25.0) rev = true;
			else if(world.objects[8].location.x <= -25.0) rev = false;

			glfwSwapBuffers(window);
			glfwPollEvents();

			deltaTime--;
		}
	}

	glfwTerminate();
	return 0;
}