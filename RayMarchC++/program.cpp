// Include standard headers
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "primitive.h"
#include "helpers.h"
#include "shader.h"
#include "free_cam.h"
#include "imgui_handler.h"

int screen_width = 1280;
int screen_height = 720;
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

int cam_mode = 0;

Camera camera(glm::vec3(0.0, 0.0, 3.0));

enum ShadingMode {
	Light, Normal, Flat
};

int shading_mode = ShadingMode::Light;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	int middle_mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (cam_mode == 1) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void processInput(GLFWwindow* window)
{
	if (cam_mode == 1) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, deltaTime);
	}

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
		if (cam_mode == 1) {
			cam_mode = 0;
		}
		else {
			cam_mode = 1;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
int setupWindow() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1920, 1080, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	return 0;
}

static RMImGui::ImGuiData data;

int main()
{
	if (setupWindow() == -1) {
		return -1;
	}

	auto mat = transformationMatrix(glm::vec3(90, 45, 20), glm::vec3(2.0, 3.0, 4.0));

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = Shader::LoadShaders("vertex.vs", "fragment.fs");
	
	auto uniforms = Shader::LoadUniforms(programID);

	

	auto prim1 = Primitive::getCubePrimitive(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	auto prim2 = Primitive::getSpherePrimitive(1.0, vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//prim.add_distort_modifier(vec3(1), 0.2, 2.2);
	//prim.add_round_modifier(0.2);
	data.addPrimitive(prim1);
	data.addPrimitive(prim2);

	Primitive::ShaderGroupPrimitive group = Primitive::opSubtractionSmooth(1, 0, 0.2);
	data.groupPrimitives[0] = group;

	//primitives.push_back(Primitive::getTorusPrimitive(1.0, 0.5, vec3(0.0, 0.0, 0.0), vec3(0.0, 45.0, 0.0), vec3(1.0, 1.0, 1.0)));
	//primitives.push_back(Primitive::getMandelbulbPrimitive(0.0, vec3(0.0, 0.0, 4.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0)));

	static const GLfloat vertices[] = {
		 -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
	};
	static const GLfloat tex_coords[] = {
		 0.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, 0.0f,
		 0.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint texbuffer;
	glGenBuffers(1, &texbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

	RMImGui::SetupImGui(window);

	double xpos, ypos;

	//glm::vec3 eye(0, 0, -7);
	//glm::vec3 center(0);
	glm::vec3 up(0, 1, 0);
	//ArcballCamera camera(eye, center, up);
	
	
	glm::vec2 prev_mouse(-2.f);

	float power = 0.0;

	data.shading_mode = &shading_mode;
	

	do{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		int middle_mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

		glfwGetCursorPos(window, &xpos, &ypos);

		auto view = camera.GetViewMatrix();
		

		glUniformMatrix4fv(uniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
		//std::cout << glm::to_string(camera.eye()) << std::endl;
		glUniform3f(uniforms.camera_pos, camera.Position.x, camera.Position.y, camera.Position.z);
		glUniform2f(uniforms.u_resolution, float(screen_width), float(screen_height));
		glUniform1i(uniforms.shading_mode, shading_mode);
		int prim_count = 0;
		for (auto p : data.primitives) {
			if (p.prim_type != 0) {
				prim_count += 1;
			}
		}
		glUniform1i(uniforms.u_prim_count, prim_count);
		PrepareShader(data.primitives, data.groupPrimitives, uniforms);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//glUniform3f(sh_color, 1.0, 0.6, 0.7);

		// Use our shader
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, texbuffer);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		
		RMImGui::RenderImGui(data);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glViewport(0, 0, screen_width, screen_height);
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}