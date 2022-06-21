#include "imgui_handler.h"
#include <string>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "constants.h"
#include "RMIO.h"
#include "scene.h"
#include "custom_imgui.h"
namespace RMImGui {
	void DisplayTransformation(Primitive::ShaderPrimitive* p, int frame) {
		auto transformation = &(*p).transformation;
		AnimatedFloat* p3[3] = { &(*transformation).position[0], &(*transformation).position[1], &(*transformation).position[2] };
		bool selected = ImGui::KeyframeDragFloat3("Position", frame, p3, 0.01, -100.0, 100.0, "%.3f", 0);

		AnimatedFloat* r3[3] = { &(*transformation).rotation[0], &(*transformation).rotation[1], &(*transformation).rotation[2] };
		selected = ImGui::KeyframeDragFloat3("Rotation", frame, r3, 1.0, 0.0, 0.0, "%.3f", 0) || selected;

		AnimatedFloat* s3[3] = { &(*transformation).scale[0], &(*transformation).scale[1], &(*transformation).scale[2] };
		selected = ImGui::KeyframeDragFloat3("Scale", frame, s3, 0.01, -100.0, 100.0, "%.3f", 0) || selected;

		if (selected) {
			updateTransformation(p);
		}
	}
	void DisplayCube(Primitive::ShaderPrimitive* cube, int frame) {
		DisplayTransformation(cube, frame);
	}
	void DisplaySphere(Primitive::ShaderPrimitive* sphere, int frame) {
		DisplayTransformation(sphere, frame);
	}
	void DisplayTorus(Primitive::ShaderPrimitive* torus, int frame) {
		DisplayTransformation(torus, frame);
		ImGui::KeyframeDragFloat("Inner Radius", frame, &(torus->values[1]), 0.01, 0, 5.0, "%.3f", 0);
	}
	void DisplayMandelbulb(Primitive::ShaderPrimitive* mandelbulb, int frame) {
		DisplayTransformation(mandelbulb, frame);
		ImGui::KeyframeDragFloat("Power", frame, &(mandelbulb->values[0]), 0.01, 0, 40.0, "%.3f", 0);
	}
	void DisplayModifier(Primitive::ShaderPrimitive* element, int index) {
		auto clicked = ImGui::Button("Delete");
		if (clicked) {
			(*element).removeModifier(index);
		}
	}

	void DisplayModifiers(Primitive::ShaderPrimitive* element, int index, int frame) {
		if (index < element->mod_count) {
			auto m = element->modifiers[index];

			element->modifiers[index].setSelected(false);
			if (m.modifier == Primitive::ModifierType::DISTORT) {
				if (ImGui::TreeNode("Distort Modifier"))
				{
					element->modifiers[index].setSelected(true);
					AnimatedFloat* p3[3] = { &(*element).modifiers[index].attribute0, &(*element).modifiers[index].attribute1, &(*element).modifiers[index].attribute2 };
					auto changed = ImGui::KeyframeDragFloat3("Offset", frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);
					
					ImGui::KeyframeDragFloat("Factor", frame, &(*element).modifiers[index].attribute3, 0.01, 0.0, 4.0, "%.3f", 0);
					ImGui::KeyframeDragFloat("Frequency", frame, &(*element).modifiers[index].attribute4, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::TWIST) {
				if (ImGui::TreeNode("Twist Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat("Power", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::BEND) {
				if (ImGui::TreeNode("Bend Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat("Power", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION) {
				if (ImGui::TreeNode("Repetition Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat("Repetition Period", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION_LIMITED) {
				if (ImGui::TreeNode("Repetition Limited Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat("Repetition Period", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					AnimatedFloat* p3[3] = { &(*element).modifiers[index].attribute1, &(*element).modifiers[index].attribute2, &(*element).modifiers[index].attribute3 };
					auto changed = ImGui::KeyframeDragFloat3("Offset", frame, p3, 0.01, 0.0, 20.0, "%.3f", 0);
					/*if (changed) {
						(*element).modifiers[index].attribute1 = p3[0];
						(*element).modifiers[index].attribute2 = p3[1];
						(*element).modifiers[index].attribute3 = p3[2];
					}*/

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::ROUND) {
				if (ImGui::TreeNode("Round Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat("Strength", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}

		}
		else if (element->mod_count == index) {
			if (ImGui::BeginCombo("##shading_mode", "Add Modifier")) {
				if (ImGui::Selectable("Distort Modifier", true))
					(*element).addDistortModifier(glm::vec3(0.0), 0.0, 0.0);
				if (ImGui::Selectable("Twist Modifier", false))
					(*element).addTwistModifier(0.0);
				if (ImGui::Selectable("Bend Modifier", false))
					(*element).addBendModifier(0.0);
				if (ImGui::Selectable("Repetition Modifier", false))
					(*element).addRepetitionModifier(10.0);
				if (ImGui::Selectable("Repetition Limited Modifier", false))
					(*element).addRepetitionLimitedModifier(1.0, glm::vec3(1.0));
				if (ImGui::Selectable("Round Modifier", false))
					(*element).addRoundModifier(0.0);

				ImGui::EndCombo();
			}
		}

	}
	void DisplayElement(ImGuiData& data, int element) {
		auto p = &data.primitives[element];
		if (p->prim_type == 1) {
			DisplaySphere(p, data.timeline.frame);
		}
		if (p->prim_type == 2) {
			DisplayTorus(p, data.timeline.frame);
		}
		if (p->prim_type == 3) {
			DisplayCube(p, data.timeline.frame);
		}
		if (p->prim_type == 4) {
			DisplayMandelbulb(p, data.timeline.frame);
		}

		for (int i = 0; i < COUNT_PRIMITIVE_MODIFIER; i++) {
			DisplayModifiers(p, i, data.timeline.frame);
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
		if (DisplayGroupObjectSelect(data, i, 0)) {
			if (DisplayGroupObjectSelect(data, i, 1)) {
				if (DisplayGroupObjectSelect(data, i, 2)) {
					if(DisplayGroupObjectSelect(data, i, 3)) {
					}
				}
			}
		}

		auto p = data.groupPrimitives[i];

		if (p.modifier == Primitive::GroupModifierType::SMOOTH_INTERSECTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_SUBTRACTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_UNION) {
			ImGui::KeyframeDragFloat("Smoothing factor", data.timeline.frame, &(data.groupPrimitives[i].primAttribute), 0.01, 0.0, 10.0, "%.3f", 0);
		}

		if (ImGui::Button("Delete")) {
			data.removeGroupModifier(i);
		}
	}

	void DisplayGroups(ImGuiData& data) {
		for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
			data.groupPrimitives[i].setSelected(false);
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
					data.groupPrimitives[i].setSelected(true);
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
					data.primitives[i].setSelected(false);
					auto p = data.primitives[i];
					if (p.prim_type == 0) {
						continue;
					}

					if (ImGui::TreeNode(std::string(std::string(p.name) + "##" + std::to_string(count)).c_str()))
					{
						data.primitives[i].setSelected(true);
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
			AnimatedFloat* p3[3] = { &data.cam_pos[0], &data.cam_pos[1], &data.cam_pos[2] };
			ImGui::KeyframeDragFloat3("Position", data.timeline.frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);

			AnimatedFloat* p2[2] = { &data.cam_py[0], &data.cam_py[1] };
			ImGui::KeyframeDragFloat2("Pitch/Yaw", data.timeline.frame, p2, 0.01, -500.0, 500.0, "%.3f", 0);

			ImGui::Checkbox("Reposition Camera", &data.reposition_cam);

			data.camSelected = true;

			ImGui::EndTabItem();
		}
		else {
			data.camSelected = false;
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

		ImGui::DragInt("Start", &data.timeline.min_frame, 1.0, 0, data.timeline.max_frame);
		float controls_x = 70 + ImGui::CalcTextSize("Start").x;

		ImGui::SameLine(controls_x);
		ImGui::DragInt("End", &data.timeline.max_frame, 1.0, data.timeline.min_frame, 100000);
		controls_x += 70 + ImGui::CalcTextSize("End").x;

		ImGui::SameLine(controls_x);
		ImGui::DragInt("Frame", &data.timeline.frame, 1.0f, data.timeline.min_frame, data.timeline.max_frame);
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

		auto animation_data = data.selectedAnimationData();
		auto keyframes = animation_data.getKeyframes();

		//std::cout << std::to_string(animation_data.selected_primitives.capacity()) << "   " << std::to_string(animation_data.group_primitivies.capacity()) << std::endl;
		//std::cout << std::to_string(keyframes.capacity()) << std::endl;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		static float thickness = 4.0f;
		static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);

		const ImVec2 p = ImGui::GetCursorScreenPos();
		const ImVec2 s = ImGui::GetWindowSize();
		float x = p.x, y = p.y;
		const ImU32 timeline_rect_light = ImColor(ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		const ImU32 timeline_rect_strong = ImColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		const ImU32 timeline_rect_accent = ImColor(ImVec4(0.4f, 0.3f, 0.2f, 1.0f));
		const ImU32 timeline_rect_covers = ImColor(ImVec4(0.15f, 0.15f, 0.15f, 0.75f));
		const ImU32 timeline_rect_keyframe = ImColor(ImVec4(0.15f, 0.15f, 0.4f, 0.75f));
		const ImU32 text_strong = ImColor(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
		
		// Timeline number bar
		const float TIMELINE_NUMBER_BAR_SIZE = 20;
		float timeline_step_width = 7.5;
		float timeline_height = 100.0;

		int frameOffset = -data.timeline.offset / timeline_step_width;
		float posOffset = -data.timeline.offset;
		x -= posOffset;

		draw_list->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + s.x, y + TIMELINE_NUMBER_BAR_SIZE), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
		for (int i = frameOffset; i < s.x / timeline_step_width + frameOffset; i += 1) {
			if (i % 10 != 0)
				continue;

			std::string timeline_number = std::to_string(i);
			auto text_size = ImGui::CalcTextSize(timeline_number.c_str()).x;

			draw_list->AddText(ImVec2(x + i * timeline_step_width - text_size * .5f + 3, y + 2), text_strong, timeline_number.c_str());
		}

		{
			std::string timeline_number = std::to_string(data.timeline.frame);
			auto text_size = ImGui::CalcTextSize(timeline_number.c_str()).x;
			float x_start = x + data.timeline.frame * timeline_step_width - text_size * .5f + 3;
			draw_list->AddRectFilled(ImVec2(x_start - 5, y), ImVec2(x_start + 5 + text_size, y + TIMELINE_NUMBER_BAR_SIZE), timeline_rect_accent, 15.0f, ImDrawCornerFlags_All);
			draw_list->AddText(ImVec2(x_start, y + 2), text_strong, timeline_number.c_str());
		}


		y += TIMELINE_NUMBER_BAR_SIZE + 2;

		// Timeline lines
		for (int i = frameOffset; i < s.x / timeline_step_width + frameOffset; i += 1) {
			if (i % 5 != 0)
				continue;
			auto x_pos = x + i * timeline_step_width;
			if (i % 10 == 0)
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + timeline_height), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
			else
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 1, y + timeline_height), timeline_rect_light, 0.0f, ImDrawCornerFlags_All);
		}

		for (auto a : keyframes) {
			auto x_pos = x + a * timeline_step_width;
			draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + timeline_height), timeline_rect_keyframe, 0.0f, ImDrawCornerFlags_All);
		}

		draw_list->AddRectFilled(ImVec2(x + data.timeline.frame * timeline_step_width, y), ImVec2(x + data.timeline.frame * timeline_step_width + 2, y + timeline_height), timeline_rect_accent, 0.0f, ImDrawCornerFlags_All);

		// Timeline covers
		draw_list->AddRectFilled(ImVec2(p.x, y), ImVec2(x + data.timeline.min_frame * timeline_step_width, y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);
		draw_list->AddRectFilled(ImVec2(p.x + data.timeline.max_frame * timeline_step_width + data.timeline.offset, y), ImVec2(p.x + s.x, y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);

		auto pos = ImGui::GetMousePos();
		pos = ImVec2(pos.x - p.x, pos.y - p.y);


		// Mouse events
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			if (pos.y > TIMELINE_NUMBER_BAR_SIZE && pos.y < TIMELINE_NUMBER_BAR_SIZE + timeline_height && pos.x > 0 && pos.x < s.x) {
				data.drag = DragStart::Timeline;
			}
			else if (pos.y > 0 && pos.y < TIMELINE_NUMBER_BAR_SIZE && pos.x > 0 && pos.x < s.x) {
				data.drag = DragStart::TopBar;
			}
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			data.drag = DragStart::None;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (data.drag == DragStart::Timeline) {
				auto d = ImGui::GetMouseDragDelta();
				ImGui::ResetMouseDragDelta();
				data.timeline.offset = std::min(data.timeline.offset + d.x, 0.0f);
			}else if (data.drag == DragStart::TopBar) {
				data.timeline.frame = std::round(pos.x / timeline_step_width);
			}
		}

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