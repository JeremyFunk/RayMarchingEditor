#include "window.h"
#include <cstdio>


WindowData windowData;

// Framebuffer ids. Needed to resize them when required. It is not needed externally, so not included in WindowData struct.

void windowSizeCallback(GLFWwindow* window, int width, int height) 
{
	windowData.screenWidth = width;
	windowData.screenHeight = height;
	glViewport(0, 0, windowData.screenWidth, windowData.screenHeight);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, windowData.colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, windowData.sampleBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	// Mouse did not get updated yet
	if (windowData.mouseData.lastX == -4206969)
	{
		windowData.mouseData.lastX = xpos;
		windowData.mouseData.lastY = ypos;
	}

	float xoffset = windowData.mouseData.lastX - xpos;
	float yoffset = windowData.mouseData.lastY - ypos; // reversed since y-coordinates go from bottom to top

	windowData.mouseData.lastX = xpos;
	windowData.mouseData.lastY = ypos;

	windowData.mouseData.xPos = xpos;
	windowData.mouseData.yPos = ypos;
}

//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	windowData.keyCallbacks.RunCallback(key, action);
//}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	windowData.mouseData.scrollSpeed = yoffset;
}

int SetupWindow() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	windowData.window = glfwCreateWindow(windowData.screenWidth, windowData.screenHeight, "Ray Marching Editor", NULL, NULL);
	if (windowData.window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(windowData.window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(windowData.window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetWindowSizeCallback(windowData.window, windowSizeCallback);
	glfwSetCursorPosCallback(windowData.window, mouseCallback);
	//glfwSetKeyCallback(windowData.window, keyCallback);
	glfwSetScrollCallback(windowData.window, scrollCallback);
	return 0;
}

void SetupBuffers() {
	glGenTextures(1, &windowData.colorBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, windowData.colorBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, windowData.screenWidth, windowData.screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, windowData.colorBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	glGenTextures(1, &windowData.sampleBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, windowData.sampleBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, windowData.screenWidth, windowData.screenHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);

	glBindImageTexture(1, windowData.sampleBuffer, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16UI);
}

void RunWindow() {

}

int CleanUp() {
	return 0;
}

WindowData& GetWindowData() {
	return windowData;
}