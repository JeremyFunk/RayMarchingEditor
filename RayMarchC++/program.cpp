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
#include "free_cam.h"
#include "imgui_handler.h"
#include "scene.h"
#include "window.h"
#include "camera.cpp"
#include "shader_manager.cpp"

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

unsigned int samples_out, color_out, depth_out;

static RMImGui::ImGuiData data;


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
	
	ShaderManager::LoadShaders();

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
	int lastTimelineFrame = data.timeline.frame;

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
			ShaderManager::Recompile();
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

			

			if (camera.moved || data.rerender) {
				ShaderManager::ReloadSSBOs(data);
			}

			auto view = camera.camera.GetViewMatrix();
			if (data.shading_mode == RMImGui::ShadingMode::Render) {
				if (camera.moved || data.rerender)
				{
					ShaderManager::ResetFramebuffers();
				}
				ShaderManager::RenderCS(data, camera, view, t);
			}else{
				ShaderManager::RenderVS(data, camera, view);
			}
			ShaderManager::RenderQuad();

			data.rerender = false;
			RMImGui::RenderImGui(data);
			lastTimelineFrame = data.timeline.frame;
		}
		glfwSwapBuffers(GetWindowData().window);
		Sleep(1);
		GetWindowData().PreparePoll();
		glfwPollEvents();

	} 
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


	glfwTerminate();

	return 0;
}