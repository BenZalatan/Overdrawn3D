#pragma once

#include "engine\entity.h"

#define ERROR_SLEEP_TIME 3000
#define WIN_WIDTH 1024
#define WIN_HEIGHT 576
#define WIN_MAXFPS 120.0

double lastTime = glfwGetTime();
double deltaTime = 0, nowTime = 0;