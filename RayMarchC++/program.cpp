// Include standard headers
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <cstdlib>

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
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>


#include <iostream>
#include "RMIO.h"
#include "primitive.h"
#include "helpers_glm.h"
#include "helpers.h"
#include "shader.h"
#include "free_cam.h"
#include "imgui_handler.h"
#include "scene.h"
int screen_width = 1920;
int screen_height = 1080;
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;
bool enteredCam = false;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;
bool moved = false;

int cam_mode = 0;

Camera camera(glm::vec3(0.0, 0.0, 3.0));
static RMImGui::ImGuiData data;

enum ShadingMode {
	Light, Normal, Flat, Render
};

int shading_mode = ShadingMode::Light;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}

void move() {
	if (!data.reposition_cam) {
		enteredCam = false;
	}
	moved = true;
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

	float xoffset = lastX - xpos;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	int middle_mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);

	if (cam_mode == 1) {
		camera.ProcessMouseMovement(xoffset, yoffset);
		if (xoffset != 0 || yoffset != 0) {
			move();
		}
	}
}


void processInput(GLFWwindow* window)
{
	bool moved = false;
	if (cam_mode == 1) {
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera.ProcessKeyboard(FORWARD, deltaTime);
			moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.ProcessKeyboard(BACKWARD, deltaTime);
			moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
			camera.ProcessKeyboard(LEFT, deltaTime);
			moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera.ProcessKeyboard(RIGHT, deltaTime);
			moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera.ProcessKeyboard(UP, deltaTime);
			moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera.ProcessKeyboard(DOWN, deltaTime);
			moved = true;
		}
		if (moved) {
			move();
		}
	}

}

bool update_camera() {
	camera.Position = data.cam_pos.toVec();

	//camera.Yaw = glm::degrees(atan2(data.cam_rot.x, data.cam_rot.z));
	//camera.Pitch = glm::degrees(asin(-data.cam_rot.y));

	auto front = vec3(camera.Front);
	auto up = vec3(camera.Up);
	auto right = vec3(camera.Right);
	

	camera.Yaw = data.cam_py[1].value;
	camera.Pitch = data.cam_py[0].value;
	camera.updateCameraVectors();

	return camera.Front != front || camera.Up != up || camera.Right != right;
}

bool update_render_camera() {
	float old_cam_pos[] = { data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value };
	float old_cam_py[] = { data.cam_py[0].value, data.cam_py[1].value, data.cam_py[2].value };
	float pitchRadians = to_radians(camera.Pitch);
	float yawRadians = to_radians(camera.Yaw);

	float sinPitch = sin(pitchRadians);
	float cosPitch = cos(pitchRadians);
	float sinYaw = sin(yawRadians);
	float cosYaw = cos(yawRadians);

 	data.cam_rot = glm::normalize(vec3(cosPitch * sinYaw, -sinPitch, cosPitch * cosYaw));
	data.cam_pos[0].value = camera.Position[0];
	data.cam_pos[1].value = camera.Position[1];
	data.cam_pos[2].value = camera.Position[2];
	data.cam_py[0].value = camera.Pitch;
	data.cam_py[1].value = camera.Yaw;

	return old_cam_pos[0] != data.cam_pos[0].value || old_cam_pos[1] != data.cam_pos[1].value || old_cam_pos[2] != data.cam_pos[2].value || old_cam_py[0] != data.cam_py[0].value || old_cam_py[1] != data.cam_py[1].value;
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
	if (key == GLFW_KEY_0 && action == GLFW_RELEASE) {
		enteredCam = true;
		moved = true;
		update_camera();
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (cam_mode == 1) {
		camera.ProcessMouseScroll(static_cast<float>(yoffset));
		if (yoffset != 0) {
			move();
		}
	}
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(screen_width, screen_height, "Ray Marching Editor", NULL, NULL);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	return 0;
}

void recompile(LuaContext &lua, RMImGui::ScriptData &c) {
	try {
		std::string data(c.script.begin(), c.script.end());
		lua.executeCode(data);
		auto eval = lua.readVariable<std::function<float(float)>>("evaluate");
		float result = eval(2.0);
		result = eval(2.2);
		result = eval(0.0);
		c.eval = eval;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	c.recompile = false;
}

int main()
{
	LuaContext lua;

	if (!RMIO::SetupDirectories()) {
		return -1;
	}
	if (setupWindow() == -1) {
		return -1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint realtimeVFShader = Shader::LoadShaders("vertex.vs", "fragment.fs");
	GLuint computeVFShader = Shader::LoadShaders("vertex.vs", "cs.fs");
	GLuint computeCS = Shader::LoadComputeShader("cs.comp");
	GLuint computeResetCS = Shader::LoadComputeShader("reset.comp");

	auto realtimeUniforms = Shader::LoadUniforms(realtimeVFShader);
	auto computeVFUniforms = Shader::LoadComputeShaderFragmentUniforms(computeVFShader);
	auto computeUniforms = Shader::LoadComputeShaderUniforms(computeCS);

	auto prim1 = Primitive::getJuliaPrimitive(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//auto prim1 = Primitive::getCubePrimitive(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//auto prim2 = Primitive::getSpherePrimitive(1.0, vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
	//prim.add_distort_modifier(vec3(1), 0.2, 2.2);
	//prim.add_round_modifier(0.2);
	data.addPrimitive(prim1);
	//data.addPrimitive(prim2);

	Primitive::ShaderGroupPrimitive group = Primitive::opSubtractionSmooth(1, 0, 0.2);
	//data.groupPrimitives[0] = group;
	data.cam_pos = vec3(0.0, 0.0, 4.0);
	data.cam_rot = vec3(-1.0, 0.0, 0.0);

	if (!LoadTextureFromFile("images/play.png", &data.textures.timeline.play)) return -1;
	if (!LoadTextureFromFile("images/loop.png", &data.textures.timeline.loop)) return -1;
	if (!LoadTextureFromFile("images/next.png", &data.textures.timeline.next)) return -1;
	if (!LoadTextureFromFile("images/previous.png", &data.textures.timeline.previous)) return -1;
	if (!LoadTextureFromFile("images/stop.png", &data.textures.timeline.stop)) return -1;
	if (!LoadTextureFromFile("images/revplay.png", &data.textures.timeline.reverse_play)) return -1;
	if (!LoadTextureFromFile("images/pause.png", &data.textures.timeline.pause)) return -1;
	
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
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	double xpos, ypos;

	//glm::vec3 eye(0, 0, -7);
	//glm::vec3 center(0);
	glm::vec3 up(0, 1, 0);
	//ArcballCamera camera(eye, center, up);
	
	glm::vec2 prev_mouse(-2.f);
	data.cam_py[0] = camera.Pitch;
	data.cam_py[1] = camera.Yaw;
	float power = 0.0;
	data.shading_mode = &shading_mode;
	int lastTimelineFrame = data.timeline.frame;

	int TEXTURE_WIDTH = screen_width, TEXTURE_HEIGHT = screen_height;
	int TILE_WIDTH = screen_width, TILE_HEIGHT = screen_height;
	int SAMPLES = 32;
	int SAMPLES_PER_ITER = 1;
	int render_sample = 0;
	int render_tile_x = 0;
	int render_tile_y = 0;
	unsigned int color_out;

	glGenTextures(1, &color_out);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_out);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glBindImageTexture(0, color_out, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	unsigned int samples_out;

	glGenTextures(1, &samples_out);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, samples_out);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, NULL);

	glBindImageTexture(1, samples_out, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16UI);

	unsigned int ssbo_prims;
	glGenBuffers(1, &ssbo_prims);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_prims);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOPrimitive) * COUNT_PRIMITIVE, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo_prims);

	unsigned int ssbo_gm;
	glGenBuffers(1, &ssbo_gm);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gm);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOGroupModifier) * COUNT_GROUP_MODIFIER, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_gm);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	float t = 0.0f;
	do{
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		t += deltaTime;

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (data.engine_state == RMImGui::GameEngineState::Start) {
			RMImGui::RenderImGui(data);
		}
		else if (data.engine_state == RMImGui::GameEngineState::Engine) {
			data.timeline.update(deltaTime);

			for (auto& c : data.globals) {
				if (c.f.mode == AnimatedFloatMode::Code && c.f.script != -1) {
					if (data.scripts[c.f.script].recompile) {
						recompile(lua, data.scripts[c.f.script]);
					}
					c.f.value = data.scripts[c.f.script].eval(data.timeline.frame / data.timeline.fps);
				}
				else {
					c.f.Recalculate(data.timeline.frame);
				}

				lua.writeVariable(c.name, c.f.value);
			}
			for (auto& c : data.scripts) {
				if (c.recompile) {
					recompile(lua, c);
				}
			}

			/*{
				float t = glfwGetTime() * .15f;
				data.primitives->values[0] = 0.45 * cos(0.5 + t * 1.2) - 0.3;
				data.primitives->values[1] = 0.45 * cos(3.9 + t * 1.7);
				data.primitives->values[2] = 0.45 * cos(1.4 + t * 1.3);
				data.primitives->values[3] = 0.45 * cos(1.1 + t * 2.5);
			}*/

			processInput(window);
			int middle_mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
			glfwGetCursorPos(window, &xpos, &ypos);

			if (lastTimelineFrame != data.timeline.frame || data.recalculate) {
				data.animate(data.timeline.frame);
				data.recalculate = false;
				moved = true;
			}
			if (data.reposition_cam && enteredCam) {
				bool move = update_render_camera();
				moved |= move;
			}
			else if (enteredCam) {
				bool move = update_camera();
				moved |= move;
			}

			auto view = camera.GetViewMatrix();
			bool render_cam = glm::length(camera.Position - data.cam_pos.toVec()) > 0.5;

			if (moved) {
				Shader::SSBOPrimitive prims[COUNT_PRIMITIVE];
				int i;
				for (i = 0; i < IM_ARRAYSIZE(data.primitives); i++) {
					prims[i].attribute0 = data.primitives[i].values[0].value;
					prims[i].attribute1 = data.primitives[i].values[1].value;
					prims[i].attribute2 = data.primitives[i].values[2].value;
					prims[i].attribute3 = data.primitives[i].values[3].value;
					prims[i].attribute4 = data.primitives[i].values[4].value;
					prims[i].attribute5 = data.primitives[i].values[5].value;

					prims[i].m00 = data.primitives[i].transformation.matrix[0][0];
					prims[i].m01 = data.primitives[i].transformation.matrix[0][1];
					prims[i].m02 = data.primitives[i].transformation.matrix[0][2];
					prims[i].m10 = data.primitives[i].transformation.matrix[1][0];
					prims[i].m11 = data.primitives[i].transformation.matrix[1][1];
					prims[i].m12 = data.primitives[i].transformation.matrix[1][2];
					prims[i].m20 = data.primitives[i].transformation.matrix[2][0];
					prims[i].m21 = data.primitives[i].transformation.matrix[2][1];
					prims[i].m22 = data.primitives[i].transformation.matrix[2][2];

					prims[i].position[0] = data.primitives[i].transformation.position[0].value;
					prims[i].position[1] = data.primitives[i].transformation.position[1].value;
					prims[i].position[2] = data.primitives[i].transformation.position[2].value;
					prims[i].prim_type = data.primitives[i].prim_type;
					for (int j = 0; j < IM_ARRAYSIZE(prims[i].modifiers); j++)
					{
						prims[i].modifiers[j].modifier = data.primitives[i].modifiers[j].modifier;
						prims[i].modifiers[j].modifierAttribute0 = data.primitives[i].modifiers[j].attribute0.value;
						prims[i].modifiers[j].modifierAttribute1 = data.primitives[i].modifiers[j].attribute1.value;
						prims[i].modifiers[j].modifierAttribute2 = data.primitives[i].modifiers[j].attribute2.value;
						prims[i].modifiers[j].modifierAttribute3 = data.primitives[i].modifiers[j].attribute3.value;
						prims[i].modifiers[j].modifierAttribute4 = data.primitives[i].modifiers[j].attribute4.value;
					}
				}
				for (i; i < COUNT_PRIMITIVE; i++) {
					prims[i].prim_type = 0;
				}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_prims);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(prims), &prims[0]);

				Shader::SSBOGroupModifier groups[COUNT_GROUP_MODIFIER];
				for (i = 0; i < IM_ARRAYSIZE(data.groupPrimitives); i++) {
					groups[i].modifier = data.groupPrimitives[i].modifier;
					groups[i].prim0 = data.groupPrimitives[i].prim0;
					groups[i].prim1 = data.groupPrimitives[i].prim1;
					groups[i].prim2 = data.groupPrimitives[i].prim2;
					groups[i].prim3 = data.groupPrimitives[i].prim3;
					groups[i].primAttribute = data.groupPrimitives[i].primAttribute.value;
				}
				for (i; i < COUNT_GROUP_MODIFIER; i++) {
					groups[i].modifier = 0;
				}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_gm);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(groups), &groups[0]);
			}

			if (*data.shading_mode == ShadingMode::Render) {
				if (moved)
				{
					moved = false;
					glUseProgram(computeResetCS);
					glDispatchCompute(
						(unsigned int)TEXTURE_WIDTH,
						(unsigned int)TEXTURE_HEIGHT,
						1
					);

					// make sure writing to image has finished before read
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					render_sample = 0;
					render_tile_x = render_tile_y = 0;
				}

				glUseProgram(computeCS);

				glUniformMatrix4fv(computeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
				glUniform3f(computeUniforms.camera_pos, camera.Position.x, camera.Position.y, camera.Position.z);
				glUniform2f(computeUniforms.u_resolution, float(screen_width), float(screen_height));
				glUniform1i(computeUniforms.shading_mode, shading_mode);
				glUniform1i(computeUniforms.render_cam, render_cam);
				int prim_count = 0;
				for (auto p : data.primitives) {
					if (p.prim_type != 0) {
						prim_count += 1;
					}
				}

				glUniform3f(computeUniforms.camera_pos_render, data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value);
				glUniform1i(computeUniforms.u_prim_count, prim_count);
				glFinish();
				auto start = std::chrono::high_resolution_clock::now();
				
				if (render_sample < SAMPLES) {
					if (render_sample < SAMPLES) {
						Shader::PrepareComputeShader(computeUniforms, data.primCount(), data.groupModifierCount(), render_tile_x * TILE_WIDTH, render_tile_y * TILE_HEIGHT, t, SAMPLES, SAMPLES_PER_ITER, render_sample);
						glDispatchCompute(
							ceil((unsigned int)std::min(TILE_WIDTH, TEXTURE_WIDTH - render_tile_x * TILE_WIDTH) / 8),
							ceil((unsigned int)std::min(TILE_HEIGHT, TEXTURE_HEIGHT - render_tile_y * TILE_HEIGHT) / 4),
							1
						);
							
						// make sure writing to image has finished before read
						glMemoryBarrier(GL_ALL_BARRIER_BITS);

						render_tile_x++;
					}
					if (TILE_WIDTH >= TEXTURE_WIDTH || render_tile_x * TILE_WIDTH > TEXTURE_WIDTH) {
						render_tile_x = 0;
						render_tile_y++;
					}
					if (TILE_HEIGHT >= TEXTURE_HEIGHT || render_tile_y * TILE_HEIGHT > TEXTURE_HEIGHT) {
						render_tile_y = 0;
						render_sample += SAMPLES_PER_ITER;
					}
				}

				glFinish();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
				if(ms != 0)
					std::cout << ms << "ms\n";
				glUseProgram(computeVFShader);
				Shader::PrepareComputeShaderFragment(computeVFUniforms, render_sample, SAMPLES);
				glUniform2f(computeVFUniforms.u_resolution, float(screen_width), float(screen_height));

			}else{
				glUseProgram(realtimeVFShader);

				glUniformMatrix4fv(realtimeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
				glUniform3f(realtimeUniforms.camera_pos, camera.Position.x, camera.Position.y, camera.Position.z);
				glUniform2f(realtimeUniforms.u_resolution, float(screen_width), float(screen_height));
				glUniform1i(realtimeUniforms.shading_mode, shading_mode);
				glUniform1i(realtimeUniforms.render_cam, render_cam);
				int prim_count = 0;
				for (auto p : data.primitives) {
					if (p.prim_type != 0) {
						prim_count += 1;
					}
				}

				glUniform3f(realtimeUniforms.camera_pos_render, data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value);
				glUniform1i(realtimeUniforms.u_prim_count, prim_count);

				Shader::PrepareShader(data.primCount(), data.groupModifierCount(), realtimeUniforms);
			}

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
			glFinish();
			auto start = std::chrono::high_resolution_clock::now();
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 indices starting at 0 -> 1 triangle
			glFinish(); 
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			if (ms != 0)
				std::cout << ms << "ms\n";
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			RMImGui::RenderImGui(data);

			lastTimelineFrame = data.timeline.frame;
		}
		glfwSwapBuffers(window);
		Sleep(1);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwWindowShouldClose(window) == 0);

	/*auto original = Scene::toJson(Scene::createScene(data));
	auto sceneBack = Scene::convertScene(Scene::toScene(original));
	auto transformed = Scene::toJson(Scene::createScene(sceneBack));
	std::cout << (original == transformed ? "Same" : "different") << std::endl << std::endl;
	std::cout << original << std::endl << std::endl;
	std::cout << transformed << std::endl << std::endl;*/


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glViewport(0, 0, screen_width, screen_height);
	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(realtimeVFShader);
	glDeleteProgram(computeCS);
	glDeleteProgram(computeVFShader);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	try {
		lua.~LuaContext();
	}
	catch (std::exception e) {

	}

	return 0;
}