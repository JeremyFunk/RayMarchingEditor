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
#include "imgui_animation.h"
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
						auto trans = data.primitives[i].transformation;
						
						data.primitives[i] = im.primitives[i];
						//data.primitives[i].transformation = trans;

						//std::cout << trans.toString() << std::endl;
						//std::cout << data.primitives[i].transformation.position[0].keyframes.capacity() << std::endl;

						Primitive::updateTransformation(&data.primitives[i]);
						
					}
					for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
						data.groupPrimitives[i] = im.groupPrimitives[i];
					}
					data.cam_pos = im.cam_pos;
					data.cam_py = im.cam_py;
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
	static ImVec2 values[] = { ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f), ImVec2(100.0f, 100.0f), ImVec2(100.0f, 100.0f) };
	static int new_count = 0;
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

		ImGui::Begin("Test");
		


		/*if (ImGui::CurveEditor("Das editor", values, 2, ImVec2(600, 200), 0, &new_count))
		{
			std::cout << values[0] << " " << values[1] << " " << values[2] << " " << values[3] << std::endl;
		}*/
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImU32 bez_curve_color = ImColor(ImVec4(0.6f, 0.6f, 0.4f, 0.75f));
		const ImU32 bez_point_color = ImColor(ImVec4(0.8f, 0.8f, 0.6f, 0.75f));

		const ImVec2 p = ImGui::GetCursorScreenPos();
		float circle_size = 4.0;
		float click_dist = 8.0;

		float padding = 5;
		float min_x = p.x + padding;
		float min_y = p.y + padding;

		ImVec2 transformed[IM_ARRAYSIZE(values)] = {};
		for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
			transformed[i] = ImVec2(values[i].x + min_x, values[i].y + min_y);	
		}
		for (int i = 0; i < IM_ARRAYSIZE(values); i++) {
			draw_list->AddCircleFilled(transformed[i], circle_size, bez_point_color);

			if (i % 2 == 0) {
				draw_list->AddLine(transformed[i], transformed[i + 1], bez_curve_color, 1.0f);
			}
		}


		auto mouse_pos = ImGui::GetMousePos();
		auto transformed_pos = ImVec2(mouse_pos.x - min_x, mouse_pos.y - min_y);

		// Mouse events
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			std::cout << mouse_pos.x << " " << mouse_pos.y << std::endl;
			for (int i = 1; i <= 2; i+=1) {
				auto k = transformed[i];
				if (mouse_pos.y > k.y - click_dist / 2 && mouse_pos.y < k.y + click_dist / 2 && mouse_pos.x > k.x - click_dist / 2 && mouse_pos.x < k.x + click_dist / 2) {
					data.drag = DragStart::BezierPoint;
					data.dragId = i;
				}
			}
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			data.drag = DragStart::None;
			data.dragId = -1;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (data.drag == DragStart::BezierPoint) {
				auto k = &values[data.dragId];
				k->x = std::max(0.0f, transformed_pos.x);
				k->y = std::max(0.0f, transformed_pos.y);
			}
		}
		
		draw_list->AddBezierCubic(transformed[0], transformed[1], transformed[2], transformed[3], bez_curve_color, 1.0);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}