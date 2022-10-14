#pragma once

#include <vector>
#include <map>
#include "constants.h"
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>


struct WindowMouseData {
	double lastX = -4206969, lastY, xPos, yPos, xOffset, yOffset, scrollSpeed;
};

struct WindowData {
	GLFWwindow* window;
	int screenWidth = DEFAULT_WINDOW_WIDTH;
	int screenHeight = DEFAULT_WINDOW_HEIGHT;
	WindowMouseData mouseData;
	unsigned int colorBuffer, sampleBuffer;
};
