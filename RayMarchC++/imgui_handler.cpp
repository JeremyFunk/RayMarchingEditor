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
		auto res5 = ImGui::KeyframeDragFloat(d, mandelbulb->name + "/Iterations", "Iterations", frame, &(mandelbulb->values[1]), 0.01, -40.0, 40.0, "%.3f", 0);
	}
	void DisplayJulia(ImGuiData* d, Primitive::ShaderPrimitive* julia, int frame) {
		DisplayTransformation(d, julia, frame);
		auto res1 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat1", "Quat1", frame, &(julia->values[0]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res2 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat2", "Quat2", frame, &(julia->values[1]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res3 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat3", "Quat3", frame, &(julia->values[2]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res4 = ImGui::KeyframeDragFloat(d, julia->name + "/Quat4", "Quat4", frame, &(julia->values[3]), 0.01, -40.0, 40.0, "%.3f", 0);
		auto res5 = ImGui::KeyframeDragFloat(d, julia->name + "/Iterations", "Iterations", frame, &(julia->values[4]), 0.01, -40.0, 40.0, "%.3f", 0);
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
			if (ImGui::BeginCombo("##modifier", "Add Modifier")) {
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


		if (ImGui::BeginCombo("##materialSelect", "Select Material")) {
			for (int i = 0; i < data->materials.count; i++) {
				if (ImGui::Selectable(data->materials[i].name.c_str(), i == p->material)) {
					data->recalculate = true;
					p->material = i;
				}
			}
			
			ImGui::EndCombo();
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

				if (ImGui::BeginCombo("##group_modifier", selected)) {
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

	void DisplayGlobals(ImGuiData* data) {
		static char name[64] = "";
		if (ImGui::Button("Add Global")) {
			
			for (int i = 0; i < 64; i++) {
				name[i] = '\0';
			}

			ImGui::OpenPopup("add_global_variable");
		}

		if (ImGui::BeginPopupContextItem("add_global_variable"))
		{
			ImGui::InputText("Enter Global Name", name, IM_ARRAYSIZE(name));
			ImGui::SameLine();
			if (ImGui::Button("Enter") || ImGui::IsKeyReleased(ImGuiKey_Enter)) {
				auto reserved = { "and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while" };
				bool illegal = false;
				for (auto s : reserved) {
					if (name == s) {
						illegal = true;
					}
				}
				auto first_element_illegal = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
				auto legal = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h', 'H', 'i', 'I', 'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N', 'o', 'O', 'p', 'P', 'q', 'Q', 'r', 'R', 's', 'S', 't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'Y', 'z', 'Z', '_' };
				for (int i = 0; i < 64; i++) {
					if (name[i] == '\0') {
						continue;
					}
					bool islegal = false;
					for (auto s : legal) {
						if (s == name[i]) {
							islegal = true;
							break;
						}
					}

					if (!islegal) {
						illegal = true;
						break;
					}

					if (i == 0) {
						for (auto s : first_element_illegal) {
							if (s == name[i]) {
								islegal = false;
								break;
							}
						}
					}

					if (!islegal) {
						illegal = true;
						break;
					}
				}

				if (illegal) {
					RMIO::PlayErrorSound();
				}
				else {
					GlobalVariable g;
					AnimatedFloat f = AnimatedFloat(0.0);
					g.name = std::string(name);
					g.f = f;
					data->globals.push_back(g);
					ImGui::CloseCurrentPopup();
				}

			}
			ImGui::EndPopup();
		}
		static int toDelete = -1;
		for (int i = 0; i < data->globals.size(); i++) {
			ImGui::KeyframeDragFloat(data, "Global/" + data->globals[i].name, data->globals[i].name.c_str(), data->timeline.frame, &data->globals[i].f, 0.01, -4.0, 4.0, "%.3f");
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				toDelete = i;
				ImGui::OpenPopup("delete_global_variable");
			}
		}

		if (ImGui::BeginPopupContextItem("delete_global_variable"))
		{
			ImGui::Text("Are you sure you want to delete?");

			if (ImGui::Button("Yes")) {
				auto s = std::vector<GlobalVariable>();
				for (int i = 0; i < data->globals.size(); i++) {
					if (i != toDelete) {
						s.push_back(data->globals[i]);
					}
				}
				data->globals = s;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void DisplayLight(ImGuiData* data, int i) {

		auto l = &data->lights[i];
		float p3[] = { l->color[0].value, l->color[1].value, l->color[2].value };

		if (ImGui::ColorPicker3("Color", p3)) {
			l->color[0].value = p3[0];
			l->color[1].value = p3[1];
			l->color[2].value = p3[2];
			data->rerender = true;
		}
		
		if (l->type == 1) {
			ImGui::KeyframeDragFloat(data, "Light/Intensity", "Intensity", data->timeline.frame, &l->intensity, 1.f, 0.0f, 100000.f);
			AnimatedFloat* p3[3] = { &l->attribute0, &l->attribute1, &l->attribute2 };
			ImGui::KeyframeDragFloat3(data, "Light/Position", "Position", data->timeline.frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);
		}
		else if (l->type == 2) {
			ImGui::KeyframeDragFloat(data, "Light/Intensity", "Intensity", data->timeline.frame, &l->intensity, 0.01f, 0.0f, 100000.f);
			AnimatedFloat* p3[3] = { &l->attribute0, &l->attribute1, &l->attribute2 };
			ImGui::KeyframeDragFloat3(data, "Light/Direction", "Direction", data->timeline.frame, p3, 0.01, -500.0, 500.0, "%.3f", 0);
		}

		if (ImGui::Button("Delete")) {
			data->lights.RemoveElement(i);
		}
	}

	void DisplayMaterial(ImGuiData* data, int i) {
		if (i == 0) {
			ImGui::Text("The default material cannot be changed or deleted!");
			return;
		}
		auto m = &data->materials[i];
		float p3[] = { m->albedo[0].value, m->albedo[1].value, m->albedo[2].value };

		if (ImGui::ColorPicker3("Albedo", p3)) {
			m->albedo[0].value = p3[0];
			m->albedo[1].value = p3[1];
			m->albedo[2].value = p3[2];
			data->rerender = true;
		}
		ImGui::KeyframeDragFloat(data, "Material/Rougness", "Roughness", data->timeline.frame, &m->roughness, 0.001f, 0.0f, 1.f);
		ImGui::KeyframeDragFloat(data, "Material/Metallic", "Metallic", data->timeline.frame, &m->metallic, 0.001f, 0.0f, 1.f);
		ImGui::KeyframeDragFloat(data, "Material/Transmission", "Transmission", data->timeline.frame, &m->transmission, 0.001f, 0.0f, 1.f);
		ImGui::KeyframeDragFloat(data, "Material/IOR", "IOR", data->timeline.frame, &m->ior, 0.001f, 0.0f, 4.f);

		if (ImGui::Button("Delete")) {
			data->materials.RemoveElement(i);
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
				for (int i = 0; i < COUNT_PRIMITIVE; i++) {
					data->primitives[i].setSelected(false);
					auto p = data->primitives[i];
					if (p.prim_type == 0) {
						continue;
					}

					if (ImGui::TreeNode(std::string(std::string(p.name) + "##" + std::to_string(i)).c_str()))
					{
						data->primitives[i].setSelected(true);
						DisplayElement(data, i);
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Lights")) {
			if (ImGui::CollapsingHeader("General"))
			{
				ImGui::Text("Add");
				if (ImGui::Button("Point Light"))
				{
					auto prim = RMImGui::Light::PointLight(glm::vec3(0.0), glm::vec3(1.0), 1000.0);
					data->lights.AddElement(prim);
				}

				if (ImGui::Button("Directional Light"))
				{
					auto prim = RMImGui::Light::DirectionalLight(glm::vec3(0.0, -1.0, 0.0), glm::vec3(1.0), 0.5);
					data->lights.AddElement(prim);
				}
			}

			if (ImGui::CollapsingHeader("Lights"))
			{
				for (int i = 0; i < data->lights.count; i++) {
					auto l = data->lights[i];
					if (l.type == 0) {
						continue;
					}

					if (ImGui::TreeNode(std::string(std::string(l.name) + "##" + std::to_string(i)).c_str()))
					{
						DisplayLight(data, i);
						ImGui::TreePop();
					}
				}
			}
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Materials")) {
			if (ImGui::CollapsingHeader("General"))
			{
				if (ImGui::Button("Add Material"))
				{
					auto prim = RMImGui::Material::DefaultMaterial();
					data->materials.AddElement(prim);
				}
			}

			if (ImGui::CollapsingHeader("Materials"))
			{
				for (int i = 0; i < data->materials.count; i++) {
					auto m = data->materials[i];
					if (!m.active) {
						continue;
					}

					if (ImGui::TreeNode(std::string(std::string(m.name) + "##" + std::to_string(i)).c_str()))
					{
						DisplayMaterial(data, i);
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
			{
				auto selected = "Light Shading";
				if (*data.shading_mode == 1)
					selected = "Normal Shading";
				if (*data.shading_mode == 2)
					selected = "Flat Shading";
				if (*data.shading_mode == 3)
					selected = "Render Shading";

				if (ImGui::BeginCombo("##shader", selected)) {
					if (ImGui::Selectable("Light Shading", true))
						*data.shading_mode = 0;
					if (ImGui::Selectable("Normal Shading", false))
						*data.shading_mode = 1;
					if (ImGui::Selectable("Flat Shading", false))
						*data.shading_mode = 2;
					if (ImGui::Selectable("Render Shading", false)) {
						data.recalculate = true;
						*data.shading_mode = 3;
					}

					ImGui::EndCombo();
				}
			}

			{
				auto selected = "Default Shading";
				if (data.renderMode == RenderMode::Depth)
					selected = "Depth Shading";

				if (ImGui::BeginCombo("##shading", selected)) {
					if (ImGui::Selectable("Default Shading", true)) {
						data.recalculate = true;
						data.renderMode = RenderMode::Default;
					}
					if (ImGui::Selectable("Depth Shading", false)) {
						data.recalculate = true;
						data.renderMode = RenderMode::Depth;
					}

					ImGui::EndCombo();
				}
			}

			{
				if (data.renderMode == RenderMode::Depth) {
					if (ImGui::DragFloat("Min Depth", &data.minDepth, 0.01, 0.0, data.maxDepth))
						data.recalculate = true;
					if(ImGui::DragFloat("Max Depth", &data.maxDepth, 0.01, data.minDepth))
						data.recalculate = true;
				}
			}

			if (ImGui::InputInt("Bounces", &data.show_bounce)) {
				data.recalculate = true;
			}

			if (ImGui::Button("Recompile Compute Shader")) {
				data.recompileShader = true;
				data.recalculate = true;
			}

			if (ImGui::InputInt("Samples", &data.samples)) {
				data.recalculate = true;
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

			ImGui::KeyframeDragFloat(&data, "Camera->Focal length", "Focal length", data.timeline.frame, &data.cam_data.focal_length, 0.01f, 0.f, 20.f);
			ImGui::KeyframeDragFloat(&data, "Camera->Focus distance", "Focus distance", data.timeline.frame, &data.cam_data.focus_dist, 0.01f, 0.f, 20.f);
			ImGui::KeyframeDragFloat(&data, "Camera->Apeture size", "Apeture size", data.timeline.frame, &data.cam_data.apeture_size, 0.01f, 0.f, 20.f);


			ImGui::EndTabItem();
		}
		else {
			data.camSelected = false;
		}


		if (ImGui::BeginTabItem("Global Variables")) {
			DisplayGlobals(&data);
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
					auto im = Scene::convertScene(Scene::toScene(content, RMIO::PathGetDirectoryPart(file.file)));
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
					data.project_path = RMIO::PathGetDirectoryPart(file.path);
					data.scripts = im.scripts;
					data.cam_data = im.cam_data;
					data.lights = im.lights;
					data.materials = im.materials;
					data.samples = im.samples;
					for (int i = 0; i < data.scripts.size(); i++) {
						data.scripts[i].compile();
					}
					data.recalculate = true;
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

	void RenderStart(ImGuiData& data) {
		ImGui::Begin("Select Option");

		if (ImGui::Button("Open Scene")) {
			auto file = RMIO::ExplorerOpenFile();
			if (file.selected) {
				data.project_path = RMIO::PathGetDirectoryPart(file.path);
				auto content = RMIO::Load(file.path);
				auto im = Scene::convertScene(Scene::toScene(content, data.project_path));
				for (int i = 0; i < COUNT_PRIMITIVE; i++) {
					auto trans = data.primitives[i].transformation;

					data.primitives[i] = im.primitives[i];
					Primitive::updateTransformation(&data.primitives[i]);
				}

				for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
					data.groupPrimitives[i] = im.groupPrimitives[i];
				}
				data.scripts = im.scripts;
				data.cam_pos = im.cam_pos;
				data.cam_py = im.cam_py;
				data.globals = im.globals;
				data.lights = im.lights;
				data.materials = im.materials;
				data.samples = im.samples;
				data.engine_state = GameEngineState::Engine;
				for (int i = 0; i < data.scripts.size(); i++) {
					data.scripts[i].compile();
				}
				data.recalculate = true;
				data.cam_data = im.cam_data;
			}
			RMIO::SetupProjectDirectories(file.path);
		}
		if (ImGui::Button("New Scene")) {
			auto file = RMIO::ExplorerSaveFile();

			if (file.selected) {
				RMIO::Save(file.path, Scene::toJson(Scene::createScene(data)));
				RMIO::SaveAppData("data\\last_path.txt", file.path);
				data.engine_state = GameEngineState::Engine;
				data.project_path = RMIO::PathGetDirectoryPart(file.path);
			}

			RMIO::SetupProjectDirectories(file.path);
		}

		ImGui::End();
	}

	void RenderEngineImGui(ImGuiData& data) {
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
	}
	void RenderImGui(ImGuiData& data) {
		switch (data.engine_state) {
		case GameEngineState::Engine:
			RenderEngineImGui(data);
			break;
		case GameEngineState::Start:
			RenderStart(data);
			break;
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}