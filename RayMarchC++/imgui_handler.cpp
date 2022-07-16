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
#include "imgui_code.h"

namespace RMImGui {
	void DisplayTransformation(ImGuiData* d, Primitive::ShaderPrimitive* p, int frame) {
		auto transformation = &(*p).transformation;
		AnimatedFloat* p3[3] = { &(*transformation).position[0], &(*transformation).position[1], &(*transformation).position[2] };
		auto selected1 = ImGui::KeyframeDragFloat3(d, p->name + "/Position", "Position", frame, p3, 0.01, -100.0, 100.0, "%.3f", 0);

		AnimatedFloat* r3[3] = { &(*transformation).rotation[0], &(*transformation).rotation[1], &(*transformation).rotation[2] };
		auto selected2 = ImGui::KeyframeDragFloat3(d, p->name + "/Rotation", "Rotation", frame, r3, 1.0, 0.0, 0.0, "%.3f", 0);

		AnimatedFloat* s3[3] = { &(*transformation).scale[0], &(*transformation).scale[1], &(*transformation).scale[2] };
		auto selected3 = ImGui::KeyframeDragFloat3(d, p->name + "/Scale", "Scale", frame, s3, 0.01, -100.0, 100.0, "%.3f", 0);

		if (selected1.clicked || selected2.clicked || selected3.clicked) {
			updateTransformation(p);
		}
	}
	void DisplayCube(ImGuiData* d, Primitive::ShaderPrimitive* cube, int frame) {
		DisplayTransformation(d, cube, frame);
	}
	void DisplaySphere(ImGuiData* d, Primitive::ShaderPrimitive* sphere, int frame) {
		DisplayTransformation(d, sphere, frame);
	}
	void DisplayTorus(ImGuiData* d, Primitive::ShaderPrimitive* torus, int frame) {
		DisplayTransformation(d, torus, frame);
		ImGui::KeyframeDragFloat(d, torus->name + "/Inner Radius", "Inner Radius", frame, &(torus->values[1]), 0.01, 0, 5.0, "%.3f", 0);
	}
	void DisplayMandelbulb(ImGuiData* d, Primitive::ShaderPrimitive* mandelbulb, int frame) {
		DisplayTransformation(d, mandelbulb, frame);
		ImGui::KeyframeDragFloat(d, mandelbulb->name + "/Power", "Power", frame, &(mandelbulb->values[0]), 0.01, 0, 40.0, "%.3f", 0);
	}
	void DisplayJulia(ImGuiData* d, Primitive::ShaderPrimitive* julia, int frame) {
		DisplayTransformation(d, julia, frame);
		auto res1 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat1", "Quat1", frame, &(julia->values[0]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res2 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat2", "Quat2", frame, &(julia->values[1]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res3 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat3", "Quat3", frame, &(julia->values[2]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res4 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat4", "Quat4", frame, &(julia->values[3]), 0.01, -40.0, 40.0, "%.3f", 0);
	}

	void DisplayModifier(Primitive::ShaderPrimitive* element, int index) {
		auto clicked = ImGui::Button("Delete");
		if (clicked) {
			(*element).removeModifier(index);
		}
	}

	void DisplayModifiers(ImGuiData* d, Primitive::ShaderPrimitive* element, int index, int frame) {
		if (index < element->mod_count) {
			auto m = element->modifiers[index];

			element->modifiers[index].setSelected(false);
			if (m.modifier == Primitive::ModifierType::DISTORT) {
				if (ImGui::TreeNode("Distort Modifier"))
				{
					element->modifiers[index].setSelected(true);
					AnimatedFloat* p3[3] = { &(*element).modifiers[index].attribute0, &(*element).modifiers[index].attribute1, &(*element).modifiers[index].attribute2 };
					auto changed = ImGui::KeyframeDragFloat3(d, element->name + "->Modifier->Distort->Offset", "Offset", frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);
					
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Distort/Factor", "Factor", frame, &(*element).modifiers[index].attribute3, 0.01, 0.0, 4.0, "%.3f", 0);
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Distort/Frequency", "Frequency", frame, &(*element).modifiers[index].attribute4, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::TWIST) {
				if (ImGui::TreeNode("Twist Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Twist/Power", "Power", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::BEND) {
				if (ImGui::TreeNode("Bend Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Bend/Power", "Power", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION) {
				if (ImGui::TreeNode("Repetition Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Repetition/Repetition Period", "Repetition Period", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					DisplayModifier(element, index);

					ImGui::TreePop();
				}
			}
			else if (m.modifier == Primitive::ModifierType::REPETITION_LIMITED) {
				if (ImGui::TreeNode("Repetition Limited Modifier"))
				{
					element->modifiers[index].setSelected(true);
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Repetition Limited Modifier/Repetition Period", "Repetition Period", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 10.0, "%.3f", 0);

					AnimatedFloat* p3[3] = { &(*element).modifiers[index].attribute1, &(*element).modifiers[index].attribute2, &(*element).modifiers[index].attribute3 };
					auto changed = ImGui::KeyframeDragFloat3(d, element->name + "/Modifier/Repetition Limited Modifier/Offset", "Offset", frame, p3, 0.01, 0.0, 20.0, "%.3f", 0);
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
					ImGui::KeyframeDragFloat(d, element->name + "/Modifier/Round/Strength", "Strength", frame, &(*element).modifiers[index].attribute0, 0.01, 0.0, 4.0, "%.3f", 0);

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
	void DisplayElement(ImGuiData* data, int element) {
		auto p = &data->primitives[element];
		if (p->prim_type == 1) {
			DisplaySphere(data, p, data->timeline.frame);
		}
		if (p->prim_type == 2) {
			DisplayTorus(data, p, data->timeline.frame);
		}
		if (p->prim_type == 3) {
			DisplayCube(data, p, data->timeline.frame);
		}
		if (p->prim_type == 4) {
			DisplayMandelbulb(data, p, data->timeline.frame);
		}
		if (p->prim_type == 5) {
			DisplayJulia(data, p, data->timeline.frame);
		}

		for (int i = 0; i < COUNT_PRIMITIVE_MODIFIER; i++) {
			DisplayModifiers(data, p, i, data->timeline.frame);
		}

		auto clicked = ImGui::Button("Delete");
		if (clicked) {
			data->primitives[element].prim_type = 0;
			data->primitives[element].name = "";
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

	bool DisplayGroupObjectSelect(ImGuiData* data, int i, int p) {
		auto g = data->groupPrimitives[i];
		int* primI;
		if (p == 0)
			primI = &(data->groupPrimitives[i].prim0);
		else if (p == 1)
			primI = &(data->groupPrimitives[i].prim1);
		else if (p == 2)
			primI = &(data->groupPrimitives[i].prim2);
		else
			primI = &(data->groupPrimitives[i].prim3);

		std::string display_name = "";
		if (*primI != -1) {
			if (data->primitives[*primI].prim_type == 0)
				*primI = -1;
			else
				display_name = data->primitives[*primI].name;
		}

		if (ImGui::BeginCombo(std::string("##primitives" + std::to_string(i) + std::to_string(p)).c_str(), display_name.c_str())) {
			for (int i = 0; i < COUNT_PRIMITIVE; i++) {
				if (data->primitives[i].prim_type != 0) {
					if (ImGui::Selectable(data->primitives[i].name.c_str(), false)) {
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

	void DisplayGroup(ImGuiData* data, int i) {
		// For int
		if (DisplayGroupObjectSelect(data, i, 0)) {
			if (DisplayGroupObjectSelect(data, i, 1)) {
				if (DisplayGroupObjectSelect(data, i, 2)) {
					if(DisplayGroupObjectSelect(data, i, 3)) {
					}
				}
			}
		}

		auto p = data->groupPrimitives[i];

		std::string name = "GroupModifier->";
		switch (p.modifier) {
			case Primitive::GroupModifierType::INTERSECTION:
				name = "Intersection";
			case Primitive::GroupModifierType::NONE_GROUP:
				name = "None";
			case Primitive::GroupModifierType::SUBTRACTION:
				name = "Subtraction";
			case Primitive::GroupModifierType::UNION:
				name = "Union";
			case Primitive::GroupModifierType::SMOOTH_INTERSECTION:
				name = "Smooth Intersection";
			case Primitive::GroupModifierType::SMOOTH_SUBTRACTION:
				name = "Smooth Subtraction";
			case Primitive::GroupModifierType::SMOOTH_UNION:
				name = "Smooth Union";
		}

		if (p.modifier == Primitive::GroupModifierType::SMOOTH_INTERSECTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_SUBTRACTION ||
			p.modifier == Primitive::GroupModifierType::SMOOTH_UNION) {
			ImGui::KeyframeDragFloat(data, name + "/Smoothing Factor", "Smoothing factor", data->timeline.frame, &(data->groupPrimitives[i].primAttribute), 0.01, 0.0, 10.0, "%.3f", 0);
		}

		if (ImGui::Button("Delete")) {
			data->removeGroupModifier(i);
		}
	}

	void DisplayGroups(ImGuiData* data) {
		for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
			data->groupPrimitives[i].setSelected(false);
			auto p = data->groupPrimitives[i];
			if (p.modifier == 0) {
				auto selected = "Modifier";

				if (ImGui::BeginCombo("##shading_mode", selected)) {
					if (ImGui::Selectable("Union", false))
						data->groupPrimitives[i] = Primitive::opUnion(0, 1);
					if (ImGui::Selectable("Subtraction", false))
						data->groupPrimitives[i] = Primitive::opSubtraction(0, 1);
					if (ImGui::Selectable("Intersection", false))
						data->groupPrimitives[i] = Primitive::opIntersection(0, 1);
					if (ImGui::Selectable("Smooth Union", false))
						data->groupPrimitives[i] = Primitive::opUnionSmooth(0, 1, 0.0);
					if (ImGui::Selectable("Smooth Subtraction", false))
						data->groupPrimitives[i] = Primitive::opSubtractionSmooth(0, 1, 0.0);
					if (ImGui::Selectable("Smooth Intersection", false))
						data->groupPrimitives[i] = Primitive::opIntersectionSmooth(0, 1, 0.0);

					ImGui::EndCombo();
				}
				
				break;
			}
			else {
				if (ImGui::TreeNode(p.name().c_str()))
				{
					data->groupPrimitives[i].setSelected(true);
					DisplayGroup(data, i);
					ImGui::TreePop();
				}
			}
			//std::cout << std::to_string(i) << p.name << std::endl;
		}
	}

	void DisplayObjects(ImGuiData* data) {
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
					data->addPrimitive(prim);
				}

				clicked = ImGui::Button("Torus");
				if (clicked)
				{
					auto prim = Primitive::getTorusPrimitive(1.0, 0.5, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data->addPrimitive(prim);
				}

				clicked = ImGui::Button("Sphere");
				if (clicked)
				{
					auto prim = Primitive::getSpherePrimitive(1.0, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data->addPrimitive(prim);
				}

				clicked = ImGui::Button("Mandelbulb");
				if (clicked)
				{
					auto prim = Primitive::getMandelbulbPrimitive(1.0, glm::vec3(0.0), glm::vec3(0.0), glm::vec3(1.0));
					data->addPrimitive(prim);
				}
			}
			if (ImGui::CollapsingHeader("Objects"))
			{
				int count = 0;
				for (int i = 0; i < COUNT_PRIMITIVE; i++) {
					data->primitives[i].setSelected(false);
					auto p = data->primitives[i];
					if (p.prim_type == 0) {
						continue;
					}

					if (ImGui::TreeNode(std::string(std::string(p.name) + "##" + std::to_string(count)).c_str()))
					{
						data->primitives[i].setSelected(true);
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
			ImGui::KeyframeDragFloat3(&data, "Camera/Position", "Position", data.timeline.frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);

			AnimatedFloat* p2[2] = { &data.cam_py[0], &data.cam_py[1] };
			ImGui::KeyframeDragFloat2(&data, "Camera/PY", "Pitch/Yaw", data.timeline.frame, p2, 0.01, -500.0, 500.0, "%.3f", 0);

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

	bool DisplayAnimationWindow(ImGuiData& d, int index) {
		if (d.windows[index].f->mode == AnimatedFloatMode::Bezier) {
			return DisplayBezier(d, index);
		}
		return DisplayCode(&d.windows[index], d);
	}

	void RenderImGui(ImGuiData& data) {
		DisplayTimeline(data);
		
		ImGui::BeginMainMenuBar();
		DisplayFileMenu(data);
		ImGui::EndMainMenuBar();

		ImGui::Begin("Objects");
		DisplayObjects(&data);
		ImGui::End();

		ImGui::Begin("Engine");
		DisplayEngine(data);
		ImGui::End();

		auto windows = std::vector<AnimationWindow>();
		for (int i = 0; i < data.windows.size(); i++) {
			if (DisplayAnimationWindow(data, i)) {
				windows.push_back(data.windows[i]);
			}
		}
		data.windows = windows;


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}