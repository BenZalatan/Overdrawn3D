#pragma once

#include <stdio.h>
#include <Windows.h>

#include "glfw3.h"
#include "window.h"
#include "engine\camera.h"
#include "engine\world.h"

GLFWwindow *window;

int main()
{
	if(!glfwInit())
	{
		printf("Failed to initialize GLFW.\n");
		Sleep(ERROR_SLEEP_TIME);
		return -1;
	}

	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Test", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
		printf("Failed to create a GLFW window.\n");
		Sleep(ERROR_SLEEP_TIME);
		return -1;
	}

	glfwMakeContextCurrent(window);

	camera = camera_t(90.0);
	world = world_t();
	world.objects[0] = entity_t(
		vec3_t(0, -2, 0),
		vec3_t(0, 0, 0),
		vec3_t(20, 0, 20));

	camera.init();

    while (!glfwWindowShouldClose(window))
    {
		nowTime = glfwGetTime();
        deltaTime += (nowTime - lastTime) / (1 / WIN_MAXFPS);
        lastTime = nowTime;

        while (deltaTime >= 1.0)
		{
			camera.draw();

			glfwSwapBuffers(window);
			glfwPollEvents();

			deltaTime--;
		}
    }

    glfwTerminate();
    return 0;
}