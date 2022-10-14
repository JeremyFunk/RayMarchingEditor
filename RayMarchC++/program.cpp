// Include standard headers
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <cstdlib>
#include "fglsl.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "window.h"

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
#include "window.h"
#include "camera.cpp"

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

unsigned int samples_out, color_out, depth_out;

static RMImGui::ImGuiData data;

enum ShadingMode {
	Light, Normal, Flat, Render
};

int shading_mode = ShadingMode::Light;

bool update_render_camera(CameraData& camera) {
	float old_cam_pos[] = { data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value };
	float old_cam_py[] = { data.cam_py[0].value, data.cam_py[1].value, data.cam_py[2].value };
	float pitchRadians = to_radians(camera.camera.Pitch);
	float yawRadians = to_radians(camera.camera.Yaw);

	float sinPitch = sin(pitchRadians);
	float cosPitch = cos(pitchRadians);
	float sinYaw = sin(yawRadians);
	float cosYaw = cos(yawRadians);

 	data.cam_rot = glm::normalize(vec3(cosPitch * sinYaw, -sinPitch, cosPitch * cosYaw));
	data.cam_pos[0].value = camera.camera.Position[0];
	data.cam_pos[1].value = camera.camera.Position[1];
	data.cam_pos[2].value = camera.camera.Position[2];
	data.cam_py[0].value = camera.camera.Pitch;
	data.cam_py[1].value = camera.camera.Yaw;

	return old_cam_pos[0] != data.cam_pos[0].value || old_cam_pos[1] != data.cam_pos[1].value || old_cam_pos[2] != data.cam_pos[2].value || old_cam_py[0] != data.cam_py[0].value || old_cam_py[1] != data.cam_py[1].value;
}

GLint recompileShader(Shader::ComputeShaderUniforms& uniforms) {
	auto fglsl = FGLSL::LoadFGLSL("cs.comp");
	fglsl.SetCondition("LIGHTS", true);
	fglsl.SetCondition("LIGHTS", false);
	fglsl.SetValue("MAX_PRIM_COUNT", "10");

	auto shaders = FGLSL::GenerateShaders(fglsl);
	shaders.SetCondition("LIGHTS", true);
	shaders.SetValue("MAX_PRIM_COUNT", "10");
	auto shader_code = shaders.GetShader();

	RMIO::Save("cs.compiled.comp", shader_code->code);
	auto computeCS = Shader::LoadComputeShader(shader_code->code);
	uniforms = Shader::LoadComputeShaderUniforms(computeCS);
	return computeCS;
}

int main()
{
	//auto fglsl = FGLSL::LoadFGLSL("cs.comp");
	///*for (auto c : fglsl.conditional) {
	//	std::cout << c.name << std::endl;
	//}
	//for (auto c : fglsl.tokens) {
	//	std::cout << c.name << std::endl;
	//}*/
	//fglsl.SetValue("MAX_PRIM_COUNT", "20");
	//fglsl.SetValue("MAX_PRIM_COUNT", "30");
	//fglsl.SetValue("MAX_MOD_COUNT", "30");
	//fglsl.SetValue("MAX_MOD_COUNT", "40");
	//fglsl.SetCondition("LIGHTS", false);
	//fglsl.SetCondition("LIGHTS", true);

	//auto shaders = FGLSL::GenerateShaders(fglsl);
	//for (auto c : shaders.shaders) {
	//	for (auto s : c.settings) {
	//		std::cout << s.name << s.value << std::endl;
	//	}
	//	for (auto s : c.conditions) {
	//		std::cout << s.name << s.active << std::endl;
	//	}
	//	std::cout << std::endl;
	//}

	//shaders.SetValue("MAX_MOD_COUNT", "40");
	//shaders.SetValue("MAX_PRIM_COUNT", "30");
	//shaders.SetCondition("LIGHTS", true);
	//auto shader = shaders.GetShader();
	//
	//std::cout << shader->code;

	if (!RMIO::SetupDirectories()) {
		return -1;
	}
	if (SetupWindow() == -1) {
		return -1;
	}
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint realtimeVFShader = Shader::LoadShaders("vertex.vs", "fragment.fs");
	GLuint computeVFShader = Shader::LoadShaders("vertex.vs", "cs.fs");
	Shader::ComputeShaderUniforms computeUniforms;
	GLuint computeCS = recompileShader(computeUniforms);
	GLuint computeResetCS = Shader::LoadComputeShaderByPath("reset.comp");

	auto realtimeUniforms = Shader::LoadUniforms(realtimeVFShader);
	auto computeVFUniforms = Shader::LoadComputeShaderFragmentUniforms(computeVFShader);

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

	RMImGui::SetupImGui(GetWindowData().window);
	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	CameraData camera;

	double xpos, ypos;

	//glm::vec3 eye(0, 0, -7);
	//glm::vec3 center(0);
	glm::vec3 up(0, 1, 0);
	//ArcballCamera camera(eye, center, up);
	
	glm::vec2 prev_mouse(-2.f);
	data.cam_py[0] = camera.camera.Pitch;
	data.cam_py[1] = camera.camera.Yaw;
	float power = 0.0;
	data.shading_mode = &shading_mode;
	int lastTimelineFrame = data.timeline.frame;

	bool tiling = false;
	int TILE_WIDTH = GetWindowData().screenWidth, TILE_HEIGHT = GetWindowData().screenHeight;
	int SAMPLES_PER_ITER = 1;
	int render_sample = 0;
	int render_tile_x = 0;
	int render_tile_y = 0;

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

	unsigned int ssbo_lights;
	glGenBuffers(1, &ssbo_lights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_lights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOLight) * COUNT_LIGHTS, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_lights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	unsigned int ssbo_materials;
	glGenBuffers(1, &ssbo_materials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_materials);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOMaterial) * COUNT_MATERIALS, NULL, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_materials);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	data.lights.AddElement(RMImGui::Light::PointLight(glm::vec3(4, 4, 3), glm::vec3(0.2, 0.3, 0.4), 300.0));
	
	auto defMat = RMImGui::Material::DefaultMaterial();
	defMat.name = "Default Material";
	data.materials.AddElement(defMat);

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

		if (data.recompileShader) {
			data.recompileShader = false;
			glDeleteShader(computeCS);
			computeCS = recompileShader(computeUniforms);
		}

		if (data.engine_state == RMImGui::GameEngineState::Start) {
			RMImGui::RenderImGui(data);
		}
		else if (data.engine_state == RMImGui::GameEngineState::Engine) {
			data.timeline.update(deltaTime);

			/*for (auto& c : data.globals) {
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
			}*/

			/*{
				float t = glfwGetTime() * .15f;
				data.primitives->values[0] = 0.45 * cos(0.5 + t * 1.2) - 0.3;
				data.primitives->values[1] = 0.45 * cos(3.9 + t * 1.7);
				data.primitives->values[2] = 0.45 * cos(1.4 + t * 1.3);
				data.primitives->values[3] = 0.45 * cos(1.1 + t * 2.5);
			}*/

			camera.UpdateMouse(GetWindowData(), deltaTime, data.cam_pos.toVec(), data.cam_py.toVec());
			int middle_mouse_state = glfwGetMouseButton(GetWindowData().window, GLFW_MOUSE_BUTTON_MIDDLE);
			glfwGetCursorPos(GetWindowData().window, &xpos, &ypos);

			if (lastTimelineFrame != data.timeline.frame || data.recalculate) {
				data.animate(data.timeline.frame);
				data.recalculate = false;
				camera.moved = true;
			}
			if (data.reposition_cam && camera.enteredCam) {
				bool move = update_render_camera(camera);
				camera.moved |= move;
			}
			else if (camera.enteredCam) {
				//bool move = update_camera();
				//camera.moved |= move;
			}

			auto view = camera.camera.GetViewMatrix();
			bool render_cam = glm::length(camera.camera.Position - data.cam_pos.toVec()) > 0.5;

			if (camera.moved || data.rerender) {
				Shader::SSBOLight lights[COUNT_LIGHTS];
				for (int i = 0; i < IM_ARRAYSIZE(data.lights.values); i++) {
					if (!(data.lights[i].type == 1 || data.lights[i].type == 2)) {
						lights[i].attribute0 = 0.0;
						lights[i].attribute1 = 0.0;
						lights[i].attribute2 = 0.0;
						lights[i].colorR = 0.0;
						lights[i].colorG = 0.0;
						lights[i].colorB = 0.0;
						lights[i].type = 0;
						continue;
					}

					if (data.lights[i].type == 2) {
						auto temp_vec = glm::normalize(glm::vec3(data.lights[i].attribute0.value, data.lights[i].attribute1.value, data.lights[i].attribute2.value));
						lights[i].attribute0 = temp_vec.x;
						lights[i].attribute1 = temp_vec.y;
						lights[i].attribute2 = temp_vec.z;
					}
					else {
						lights[i].attribute0 = data.lights[i].attribute0.value;
						lights[i].attribute1 = data.lights[i].attribute1.value;
						lights[i].attribute2 = data.lights[i].attribute2.value;
					}
					lights[i].colorR = data.lights[i].color.values[0].value * data.lights[i].intensity.value;
					lights[i].colorG = data.lights[i].color.values[1].value * data.lights[i].intensity.value;
					lights[i].colorB = data.lights[i].color.values[2].value * data.lights[i].intensity.value;
					lights[i].type = data.lights[i].type;
				}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_lights);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(lights), &lights[0]);


				Shader::SSBOMaterial materials[COUNT_MATERIALS];
				for (int i = 0; i < IM_ARRAYSIZE(data.materials.values); i++) {
					materials[i].albedoR = data.materials[i].albedo[0].value;
					materials[i].albedoG = data.materials[i].albedo[1].value;
					materials[i].albedoB = data.materials[i].albedo[2].value;

					materials[i].transmission = data.materials[i].transmission.value;
					materials[i].ior = data.materials[i].ior.value;
					materials[i].roughness = data.materials[i].roughness.value;
					materials[i].metallic = data.materials[i].metallic.value;
				}
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_materials);
				glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(materials), &materials[0]);


				Shader::SSBOPrimitive prims[COUNT_PRIMITIVE];
				int i;
				for (i = 0; i < IM_ARRAYSIZE(data.primitives); i++) {
					prims[i].material = data.primitives[i].material;

					prims[i].attribute0 = data.primitives[i].values[0].value;
					prims[i].attribute1 = data.primitives[i].values[1].value;
					prims[i].attribute2 = data.primitives[i].values[2].value;
					prims[i].attribute3 = data.primitives[i].values[3].value;
					prims[i].attribute4 = data.primitives[i].values[4].value;

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
				if (camera.moved || data.rerender)
				{
					glUseProgram(computeResetCS);
					glDispatchCompute(
						ceil((unsigned int)std::min(TILE_WIDTH, GetWindowData().screenWidth - render_tile_x * TILE_WIDTH) / 8),
						ceil((unsigned int)std::min(TILE_HEIGHT, GetWindowData().screenHeight - render_tile_y * TILE_HEIGHT) / 4),
						1
					);

					// make sure writing to image has finished before read
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					render_sample = 0;
					render_tile_x = render_tile_y = 0;
				}

				glUseProgram(computeCS);

				glUniformMatrix4fv(computeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
				glUniform3f(computeUniforms.camera_pos, camera.camera.Position.x, camera.camera.Position.y, camera.camera.Position.z);
				glUniform2f(computeUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));
				int prim_count = 0;
				for (auto p : data.primitives) {
					if (p.prim_type != 0) {
						prim_count += 1;
					}
				}

				glUniform3f(computeUniforms.camera_pos_render, data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value);
				glUniform1i(computeUniforms.u_prim_count, prim_count);
				glUniform1i(computeUniforms.render_mode, (int) data.renderMode);
				glUniform1i(computeUniforms.show_bounce, data.show_bounce);
				glUniform1f(computeUniforms.render_mode_data1, data.minDepth);
				glUniform1f(computeUniforms.render_mode_data2, data.maxDepth);
				glFinish();
				auto start = std::chrono::high_resolution_clock::now();
				
				if (render_sample < data.samples) {
					Shader::PrepareComputeShader(computeUniforms, data.primCount(), data.groupModifierCount(), render_tile_x * TILE_WIDTH, render_tile_y * TILE_HEIGHT, t, data.samples, SAMPLES_PER_ITER, render_sample, data.cam_data);
					if (tiling) {
						glDispatchCompute(
							ceil((unsigned int)std::min(TILE_WIDTH, GetWindowData().screenWidth - render_tile_x * TILE_WIDTH) / 8),
							ceil((unsigned int)std::min(TILE_HEIGHT, GetWindowData().screenHeight - render_tile_y * TILE_HEIGHT) / 4),
							1
						);
					}
					else {
						glDispatchCompute(
							ceil((unsigned int)GetWindowData().screenWidth / 8),
							ceil((unsigned int)GetWindowData().screenHeight / 4),
							1
						);
					}
							
					// make sure writing to image has finished before read
					glMemoryBarrier(GL_ALL_BARRIER_BITS);
					if (tiling) {
						render_tile_x++;
						if (TILE_WIDTH >= GetWindowData().screenWidth || render_tile_x * TILE_WIDTH > GetWindowData().screenWidth) {
							render_tile_x = 0;
							render_tile_y++;
						}
						if (TILE_HEIGHT >= GetWindowData().screenHeight || render_tile_y * TILE_HEIGHT > GetWindowData().screenHeight) {
							render_tile_y = 0;
							render_sample += SAMPLES_PER_ITER;
						}
					}
					else {
						render_sample++;
					}
				}

				glFinish();
				auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
				if(ms != 0)
					std::cout << ms << "ms\n";
				glUseProgram(computeVFShader);
				Shader::PrepareComputeShaderFragment(computeVFUniforms, render_sample, data.samples);
				glUniform2f(computeVFUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));

			}else{
				glUseProgram(realtimeVFShader);

				glUniformMatrix4fv(realtimeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
				glUniform3f(realtimeUniforms.camera_pos, camera.camera.Position.x, camera.camera.Position.y, camera.camera.Position.z);
				glUniform2f(realtimeUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));
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

				Shader::PrepareShader(data.primCount(), data.groupModifierCount(), data.cam_data.focal_length.value, realtimeUniforms);
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
			//glFinish();
			//auto start = std::chrono::high_resolution_clock::now();
			// Draw the triangle !
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 indices starting at 0 -> 1 triangle
			//glFinish(); 
			//auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
			/*if (ms != 0)
				std::cout << ms << "ms\n";*/
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			data.rerender = false;
			RMImGui::RenderImGui(data);
			lastTimelineFrame = data.timeline.frame;
		}
		glfwSwapBuffers(GetWindowData().window);
		Sleep(1);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwWindowShouldClose(GetWindowData().window) == 0);

	/*auto original = Scene::toJson(Scene::createScene(data));
	auto sceneBack = Scene::convertScene(Scene::toScene(original));
	auto transformed = Scene::toJson(Scene::createScene(sceneBack));
	std::cout << (original == transformed ? "Same" : "different") << std::endl << std::endl;
	std::cout << original << std::endl << std::endl;
	std::cout << transformed << std::endl << std::endl;*/


	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(realtimeVFShader);
	glDeleteProgram(computeCS);
	glDeleteProgram(computeVFShader);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();



	return 0;
}