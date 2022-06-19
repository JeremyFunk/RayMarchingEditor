#include "imgui_handler.h"
#include <string>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "constants.h"
#include "RMIO.h"
#include "scene.h"
namespace RMImGui {
	void DisplayTransformation(Primitive::ShaderPrimitive* p) {
		auto transformation = &(*p).transformation;
		float p3[3] = { (*transformation).position.x, (*transformation).position.y, (*transformation).position.z };
		auto changed = ImGui::DragFloat3("Position", p3, 0.01, -100.0, 100.0, "%.3f", 0);
		if (changed) {
			(*transformation).position.x = p3[0];
			(*transformation).position.y = p3[1];
			(*transformation).position.z = p3[2];
		}

		float r3[3] = { (*transformation).rotation.x, (*transformation).rotation.y, (*transformation).rotation.z };
		changed = ImGui::DragFloat3("Rotation", r3, 1.0, -180.0, 180.0, "%.3f", 0);
		if (changed) {
			(*transformation).rotation.x = r3[0];
			(*transformation).rotation.y = r3[1];
			(*transformation).rotation.z = r3[2];

			Primitive::updateTransformation(p);
		}

		float s3[3] = { (*transformation).scale.x, (*transformation).scale.y, (*transformation).scale.z };
		changed = ImGui::DragFloat3("Scale", s3, 0.01, -100.0, 100.0, "%.3f", 0);
		if (changed) {
			(*transformation).scale.x = s3[0];
			(*transformation).scale.y = s3[1];
			(*transformation).scale.z = s3[2];

			Primitive::updateTransformation(p);
		}
	}
	void DisplayCube(Primitive::ShaderPrimitive* cube) {
		DisplayTransformation(cube);
	}
	void DisplaySphere(Primitive::ShaderPrimitive* sphere) {
		DisplayTransformation(sphere);
	}
	void DisplayTorus(Primitive::ShaderPrimitive* torus) {
		DisplayTransformation(torus);
		ImGui::DragFloat("Inner Radius", &(torus->values[1]), 0.01, 0, 5.0, "%.3f", 0);
	}
	void DisplayMandelbulb(Primitive::ShaderPrimitive* mandelbulb) {
		DisplayTransformation(mandelbulb);
		ImGui::DragFloat("Power", &(mandelbulb->values[0]), 0.01, 0, 40.0, "%.3f", 0);
	}
	void DisplayModifier(Primitive::ShaderPrimitive* element, int index) {
		auto clicked = ImGui::Button("Delete");
		if (clicked) {
			(*element).remove_modifier(index);
		}
	}

	void DisplayModifiers(Primitive::ShaderPrimitive* element, int index) {
		if (index < element->mod_count) {
			auto m = element->modifiers[index];

			if (m.modifier == Primitive::ModifierType::DISTORT) {
				if (ImGui::TreeNode("Distort Modifier"))
				{
					float p3[3] = { (*element).modifiers[index].attribute0, (*element).modifiers[index].attribute1, (*element).modifiers[index].attribute2 };
					auto changed = ImGui::DragFloat3("Offset", p3, 0.01, -500.0, 500.0, "%.3f", 0);
					if (changed) {
						(*element).modifiers[index].attribute0 = p3[0];
						(*element).modifiers[index].attribute1 = p3[1];
						(*element).modifiers[index].attribute2 = p3[2];
					}
					ImGui::DragFloat("Factor", &(*element).modifiers[index].attribute3, 0.01, 0.0, 4.0, "%.3f", 0);
					ImGui::DragFloat("Frequency", &(*element).modifiers[index].attribute4, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::TWIST) {
				if (ImGui::TreeNode("Twist Modifier"))
				{
					ImGui::DragFloat("Power", &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::BEND) {
				if (ImGui::TreeNode("Bend Modifier"))
				{
					ImGui::DragFloat("Power", &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION) {
				if (ImGui::TreeNode("Repetition Modifier"))
				{
					ImGui::DragFloat("Repetition Period", &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION_LIMITED) {
				if (ImGui::TreeNode("Repetition Limited Modifier"))
				{
					ImGui::DragFloat("Repetition Period", &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					float p3[3] = { (*element).modifiers[index].attribute1, (*element).modifiers[index].attribute2, (*element).modifiers[index].attribute3 };
					auto changed = ImGui::DragFloat3("Offset", p3, 0.01, 0.0, 20.0, "%.3f", 0);
					if (changed) {
						(*element).modifiers[index].attribute1 = p3[0];
						(*element).modifiers[index].attribute2 = p3[1];
						(*element).modifiers[index].attribute3 = p3[2];
					}

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::ROUND) {
				if (ImGui::TreeNode("Round Modifier"))
				{
					ImGui::DragFloat("Strength", &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}

		}
		else if (element->mod_count == index) {
			if (ImGui::BeginCombo("##shading_mode", "Add Modifier")) {
				if (ImGui::Selectable("Distort Modifier", true))
					(*element).add_distort_modifier(glm::vec3(0.0), 0.0, 0.0);
				if (ImGui::Selectable("Twist Modifier", false))
					(*element).add_twist_modifier(0.0);
				if (ImGui::Selectable("Bend Modifier", false))
					(*element).add_bend_modifier(0.0);
				if (ImGui::Selectable("Repetition Modifier", false))
					(*element).add_repetition_modifier(10.0);
				if (ImGui::Selectable("Repetition Limited Modifier", false))
					(*element).add_repetition_limited_modifier(1.0, glm::vec3(1.0));
				if (ImGui::Selectable("Round Modifier", false))
					(*element).add_round_modifier(0.0);

				ImGui::EndCombo();
			}
		}

	}
	void DisplayElement(ImGuiData& data, int element) {
		auto p = &data.primitives[element];
		if (p->prim_type == 1) {
			DisplaySphere(p);
		}
		if (p->prim_type == 2) {
			DisplayTorus(p);
		}
		if (p->prim_type == 3) {
			DisplayCube(p);
		}
		if (p->prim_type == 4) {
			DisplayMandelbulb(p);
		}

		for (int i = 0; i < COUNT_PRIMITIVE_MODIFIER; i++) {
			DisplayModifiers(p, i);
		}

		auto clicked = ImGui::Button("Delete");
		if (clicked) {
			data.primitives[element].prim_type = 0;
			data.primitives[element].name = "";
		}
	}
	void SetupImGui(GLFWwindow* window) {
		IMGUI_CHECKVERSION();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	bool DisplayGroupObjectSelect(ImGuiData& data, int i, int p) {
		auto g = data.groupPrimitives[i];
		int* primI;
		if (p == 0)
			primI = &(data.groupPrimitives[i].prim0);
		else if (p == 1)
			primI = &(data.groupPrimitives[i].prim1);
		else if (p == 2)
			primI = &(data.groupPrimitives[i].prim2);
		else
			primI = &(data.groupPrimitives[i].prim3);

		std::string display_name = "";
		if (*primI != -1) {
			if (data.primitives[*primI].prim_type == 0)
				*primI = -1;
			else
				display_name = data.primitives[*primI].name;
		}

		if (ImGui::BeginCombo(std::string("##primitives" + std::to_string(i) + std::to_string(p)).c_str(), display_name.c_str())) {
			for (int i = 0; i < COUNT_PRIMITIVE; i++) {
				if (data.primitives[i].prim_type != 0) {
					if (ImGui::Selectable(data.primitives[i].name.c_str(), false)) {
						*primI = i;
					}
				}
			}
			if (ImGui::Selectable("None", false)) {
				*primI = -1;
			}
			ImGui::EndCombo();
		}

		if (display_name == "")
			return false;
		return true;
	}

	void DisplayGroup(ImGuiData& data, int i) {
		// For int
		if (DisplayGroupObjectSelect(data, i, 0))
			if (DisplayGroupObjectSelect(data, i, 1))
				if (DisplayGroupObjectSelect(data, i, 2))
					DisplayGroupObjectSelect(data, i, 3);

		auto p = data.groupPrimitives[i];

		if (p.modifier == Primitive::GroupModifierType::SMOOTH_INTERSECTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_SUBTRACTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_UNION) {
			ImGui::DragFloat("Smoothing factor", &(data.groupPrimitives[i].primAttribute), 0.01, 0.0, 10.0, "%.3f", 0);
		}

		if (ImGui::Button("Delete")) {
			data.remove_group_modifier(i);
		}
	}

	void DisplayGroups(ImGuiData& data) {
		for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
			auto p = data.groupPrimitives[i];
			if (p.modifier == 0) {
				auto selected = "Modifier";

				if (ImGui::BeginCombo("##shading_mode", selected)) {
					if (ImGui::Selectable("Union", false))
						data.groupPrimitives[i] = Primitive::opUnion(0, 1);
					if (ImGui::Selectable("Subtraction", false))
						data.groupPrimitives[i] = Primitive::opSubtraction(0, 1);
					if (ImGui::Selectable("Intersection", false))
						data.groupPrimitives[i] = Primitive::opIntersection(0, 1);
					if (ImGui::Selectable("Smooth Union", false))
						data.groupPrimitives[i] = Primitive::opUnionSmooth(0, 1, 0.0);
					if (ImGui::Selectable("Smooth Subtraction", false))
						data.groupPrimitives[i] = Primitive::opSubtractionSmooth(0, 1, 0.0);
					if (ImGui::Selectable("Smooth Intersection", false))
						data.groupPrimitives[i] = Primitive::opIntersectionSmooth(0, 1, 0.0);

					ImGui::EndCombo();
				}
				
				break;
			}
			else {
				if (ImGui::TreeNode(p.name().c_str()))
				{
					DisplayGroup(data, i);
					ImGui::TreePop();
				}
			}
			//std::cout << std::to_string(i) << p.name << std::endl;
		}
	}

	void DisplayObjects(ImGuiData& data) {
		ImGui::BeginTabBar("##tabs");
		

		if (ImGui::BeginTabItem("Groups")) {
			DisplayGroups(data);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Objects")) {
			if (ImGui::CollapsingHeader("General"))
			{
				ImGui::Text("Add");
				auto clicked = ImGui::Button("Cube");
				if (clicked)
				{
					auto prim = Primitive::getCubePrimitive(glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data.addPrimitive(prim);
				}

				clicked = ImGui::Button("Torus");
				if (clicked)
				{
					auto prim = Primitive::getTorusPrimitive(1.0, 0.5, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data.addPrimitive(prim);
				}

				clicked = ImGui::Button("Sphere");
				if (clicked)
				{
					auto prim = Primitive::getSpherePrimitive(1.0, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data.addPrimitive(prim);
				}

				clicked = ImGui::Button("Mandelbulb");
				if (clicked)
				{
					auto prim = Primitive::getMandelbulbPrimitive(1.0, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data.addPrimitive(prim);
				}
			}
			if (ImGui::CollapsingHeader("Objects"))
			{
				int count = 0;
				for (int i = 0; i < COUNT_PRIMITIVE; i++) {
					auto p = data.primitives[i];
					if (p.prim_type == 0) {
						continue;
					}
					//std::cout << std::to_string(i) << p.name << std::endl;
					if (ImGui::TreeNode(std::string(std::string(p.name) + "##" + std::to_string(count)).c_str()))
					{
						DisplayElement(data, i);
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	void DisplayEngine(ImGuiData& data) {
		ImGui::BeginTabBar("##tabs");
		if (ImGui::BeginTabItem("Engine")) {
			auto selected = "Light Shading";
			if (*data.shading_mode == 1)
				selected = "Normal Shading";
			if (*data.shading_mode == 2)
				selected = "Flat Shading";

			if (ImGui::BeginCombo("##shading_mode", selected)) {
				if (ImGui::Selectable("Light Shading", true))
					*data.shading_mode = 0;
				if (ImGui::Selectable("Normal Shading", false))
					*data.shading_mode = 1;
				if (ImGui::Selectable("Flat Shading", false))
					*data.shading_mode = 2;

				ImGui::EndCombo();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Camera")) {
			float p3[3] = { data.cam_pos.x, data.cam_pos.y, data.cam_pos.z };
			auto changed = ImGui::DragFloat3("Offset", p3, 0.01, -500.0, 500.0, "%.3f", 0);
			if (changed) {
				data.cam_pos.x = p3[0];
				data.cam_pos.y = p3[1];
				data.cam_pos.z = p3[2];
			}

			ImGui::Checkbox("Reposition Camera", &data.reposition_cam);

			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	void DisplayFileMenu(ImGuiData& data) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				auto file = RMIO::ExplorerOpenFile();
				
				if (file.selected) {
					auto content = RMIO::Load(file.path);
					auto im = Scene::convertScene(Scene::toScene(content));
					for(int i = 0; i < COUNT_PRIMITIVE; i++){
						data.primitives[i] = im.primitives[i];
					}
					for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
						data.groupPrimitives[i] = im.groupPrimitives[i];
					}
					data.cam_pos = im.cam_pos;
					data.cam_rot = im.cam_rot;
				}
			}
			if (ImGui::MenuItem("Save", "Ctrl+S") || ImGui::MenuItem("Save As", "Ctrl+Shift+S")) {
				auto file = RMIO::ExplorerSaveFile();

				if (file.selected) {
					RMIO::Save(file.path, Scene::toJson(Scene::createScene(data)));
				}
			}
			ImGui::EndMenu();
		}
	}

	void DisplayTimelineTopBar(ImGuiData& data) {
		// Timeline controls
		ImGui::PushItemWidth(50);

		ImGui::DragInt("Start", &data.timeline.min_frame);
		float controls_x = 70 + ImGui::CalcTextSize("Start").x;

		ImGui::SameLine(controls_x);
		ImGui::DragInt("End", &data.timeline.max_frame);
		controls_x += 70 + ImGui::CalcTextSize("End").x;

		ImGui::SameLine(controls_x);
		ImGui::DragInt("Frame", &data.timeline.frame);
		controls_x += 70 + ImGui::CalcTextSize("Frame").x;

		ImGui::SameLine(controls_x); 
		if (ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.previous), ImVec2(15.0, 15.0))) {

		}
		controls_x += 25;

		if (data.timeline.mode == TimelineMode::Pause) {
			ImGui::SameLine(controls_x);
			if (ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.reverse_play), ImVec2(15.0, 15.0))) {
				data.timeline.mode = TimelineMode::Reverse;
			}
			controls_x += 25;

			ImGui::SameLine(controls_x);
			if (ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.play), ImVec2(15.0, 15.0))) {
				data.timeline.mode = TimelineMode::Play;
			}
			controls_x += 25;
		}
		else {
			ImGui::SameLine(controls_x);
			if (ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.pause), ImVec2(15.0, 15.0))) {
				data.timeline.mode = TimelineMode::Pause;
			}
			controls_x += 25;
		}

		ImGui::SameLine(controls_x);
		if (ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.next), ImVec2(15.0, 15.0))) {

		}
		controls_x += 25;

		//ImGui::SameLine(controls_x); ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.pause), ImVec2(15.0, 15.0)); controls_x += 25;
		//ImGui::SameLine(controls_x); ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.stop), ImVec2(15.0, 15.0)); controls_x += 25;

	}

	void DisplayTimeline(ImGuiData& data) {


		bool open = false;
		if (!ImGui::Begin("Timeline", &open))
		{
			ImGui::End();
			return;
		}

		DisplayTimelineTopBar(data);
		

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		static float sz = 36.0f;
		static float thickness = 4.0f;
		static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);

		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 s = ImGui::GetWindowSize();
		float x = p.x, y = p.y;
		const ImU32 timeline_rect_light = ImColor(ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		const ImU32 timeline_rect_strong = ImColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		const ImU32 timeline_rect_accent = ImColor(ImVec4(0.4f, 0.3f, 0.2f, 1.0f));
		const ImU32 timeline_rect_covers = ImColor(ImVec4(0.15f, 0.15f, 0.15f, 0.75f));
		const ImU32 text_strong = ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));


		// Timeline number bar
		const float TIMELINE_NUMBER_BAR_SIZE = 20;
		float timeline_step_width = 7.5;
		draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + s.x, y + TIMELINE_NUMBER_BAR_SIZE), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
		for (int i = 0; i < s.x / timeline_step_width; i += 10) {

			std::string timeline_number = std::to_string(i);
			auto text_size = ImGui::CalcTextSize(timeline_number.c_str()).x;

			draw_list->AddText(ImVec2(x + i * timeline_step_width - text_size * .5 + 3, y + 2), text_strong, timeline_number.c_str());
		}

		{
			std::string timeline_number = std::to_string(data.timeline.frame);
			auto text_size = ImGui::CalcTextSize(timeline_number.c_str()).x;
			float x_start = x + data.timeline.frame * timeline_step_width - text_size * .5 + 3;
			draw_list->AddRectFilled(ImVec2(x_start - 5, y), ImVec2(x_start + 5 + text_size, y + TIMELINE_NUMBER_BAR_SIZE), timeline_rect_accent, 15.0f, ImDrawCornerFlags_All);
			draw_list->AddText(ImVec2(x_start, y + 2), text_strong, timeline_number.c_str());
		}


		y += TIMELINE_NUMBER_BAR_SIZE + 2;

		// Timeline lines
		for (int i = 0; i < s.x / timeline_step_width; i += 5) {
			auto x_pos = x + i * timeline_step_width;
			if (i % 10 == 0)
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + sz), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
			else
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 1, y + sz), timeline_rect_light, 0.0f, ImDrawCornerFlags_All);
		}

		draw_list->AddRectFilled(ImVec2(x + data.timeline.frame * timeline_step_width, y), ImVec2(x + data.timeline.frame * timeline_step_width + 2, y + sz), timeline_rect_accent, 0.0f, ImDrawCornerFlags_All);

		// Timeline covers
		draw_list->AddRectFilled(ImVec2(p.x, y), ImVec2(x + data.timeline.min_frame * timeline_step_width + 2, y + sz), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);
		draw_list->AddRectFilled(ImVec2(p.x + data.timeline.max_frame * timeline_step_width, y), ImVec2(p.x + s.x, y + sz), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);

		ImGui::End();
	}

	void RenderImGui(ImGuiData& data) {
		DisplayTimeline(data);

		ImGui::BeginMainMenuBar();
		DisplayFileMenu(data);
		ImGui::EndMainMenuBar();

		ImGui::Begin("Objects");
		DisplayObjects(data);
		ImGui::End();

		ImGui::Begin("Engine");
		DisplayEngine(data);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}