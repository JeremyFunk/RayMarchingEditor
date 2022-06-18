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

	void DisplayTimeline(ImGuiData& data) {
		bool open = false;
		if (!ImGui::Begin("Timeline", &open))
		{
			ImGui::End();
			return;
		}

		// Tip: If you do a lot of custom rendering, you probably want to use your own geometrical types and benefit of overloaded operators, etc. 
		// Define IM_VEC2_CLASS_EXTRA in imconfig.h to create implicit conversions between your types and ImVec2/ImVec4. 
		// ImGui defines overloaded operators but they are internal to imgui.cpp and not exposed outside (to avoid messing with your types) 
		// In this example we are not using the maths operators! 
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// Primitives 

		ImGui::Text("Primitives");
		static float sz = 36.0f;
		static float thickness = 4.0f;
		static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
		ImGui::DragFloat("Size", &sz, 0.2f, 2.0f, 72.0f, "%.0f");
		ImGui::DragFloat("Thickness", &thickness, 0.05f, 1.0f, 8.0f, "%.02f");
		ImGui::ColorEdit4("Color", &col.x);
		{
			const ImVec2 p = ImGui::GetCursorScreenPos();
			const ImU32 col32 = ImColor(col);
			float x = p.x + 4.0f, y = p.y + 4.0f, spacing = 8.0f;

			for (int i = 0; i < 200; i += 5) {
				const ImU32 col32 = ImColor(ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
				draw_list->AddRectFilled(ImVec2(i * 5.0, 0.0), ImVec2(i * 5.0 + 2.0, 100.0), col32, 0.0f, ImDrawCornerFlags_All);
			}
			for (int n = 0; n < 2; n++)
			{
				// First line uses a thickness of 1.0, second line uses the configurable thickness 
				float th = (n == 0) ? 1.0f : thickness;
				draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6, th); x += sz + spacing;     // Hexagon 
				draw_list->AddCircle(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 20, th); x += sz + spacing;    // Circle 
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 0.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_All, th); x += sz + spacing;
				draw_list->AddRect(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight, th); x += sz + spacing;
				draw_list->AddTriangle(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32, th); x += sz + spacing;
				draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y), col32, th); x += sz + spacing;               // Horizontal line (note: drawing a filled rectangle will be faster!) 
				draw_list->AddLine(ImVec2(x, y), ImVec2(x, y + sz), col32, th); x += spacing;                  // Vertical line (note: drawing a filled rectangle will be faster!) 
				draw_list->AddLine(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, th); x += sz + spacing;               // Diagonal line 
				draw_list->AddBezierCurve(ImVec2(x, y), ImVec2(x + sz * 1.3f, y + sz * 0.3f), ImVec2(x + sz - sz * 1.3f, y + sz - sz * 0.3f), ImVec2(x + sz, y + sz), col32, th);
				x = p.x + 4;
				y += sz + spacing;
			}
			draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 6); x += sz + spacing;       // Hexagon 
			draw_list->AddCircleFilled(ImVec2(x + sz * 0.5f, y + sz * 0.5f), sz * 0.5f, col32, 32); x += sz + spacing;      // Circle 
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32); x += sz + spacing;
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f); x += sz + spacing;
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + sz), col32, 10.0f, ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotRight); x += sz + spacing;
			draw_list->AddTriangleFilled(ImVec2(x + sz * 0.5f, y), ImVec2(x + sz, y + sz - 0.5f), ImVec2(x, y + sz - 0.5f), col32); x += sz + spacing;
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + sz, y + thickness), col32); x += sz + spacing;          // Horizontal line (faster than AddLine, but only handle integer thickness) 
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + thickness, y + sz), col32); x += spacing + spacing;     // Vertical line (faster than AddLine, but only handle integer thickness) 
			draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + 1, y + 1), col32);          x += sz;                  // Pixel (faster than AddLine) 
			draw_list->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + sz, y + sz), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255), IM_COL32(255, 255, 0, 255), IM_COL32(0, 255, 0, 255));
			ImGui::Dummy(ImVec2((sz + spacing) * 8, (sz + spacing) * 3));
		}
		ImGui::Separator();
		{
			static ImVector<ImVec2> points;
			static bool adding_line = false;
			ImGui::Text("Canvas example");
			if (ImGui::Button("Clear")) points.clear();
			if (points.Size >= 2) { ImGui::SameLine(); if (ImGui::Button("Undo")) { points.pop_back(); points.pop_back(); } }
			ImGui::Text("Left-click and drag to add lines,\nRight-click to undo");

			// Here we are using InvisibleButton() as a convenience to 1) advance the cursor and 2) allows us to use IsItemHovered() 
			// But you can also draw directly and poll mouse/keyboard by yourself. You can manipulate the cursor using GetCursorPos() and SetCursorPos(). 
			// If you only use the ImDrawList API, you can notify the owner window of its extends by using SetCursorPos(max). 
			ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates! 
			ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available 
			if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
			if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
			draw_list->AddRectFilledMultiColor(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 50, 50, 255), IM_COL32(50, 50, 60, 255), IM_COL32(60, 60, 70, 255), IM_COL32(50, 50, 60, 255));
			draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(255, 255, 255, 255));

			bool adding_preview = false;
			ImGui::InvisibleButton("canvas", canvas_size);
			ImVec2 mouse_pos_in_canvas = ImVec2(ImGui::GetIO().MousePos.x - canvas_pos.x, ImGui::GetIO().MousePos.y - canvas_pos.y);
			if (adding_line)
			{
				adding_preview = true;
				points.push_back(mouse_pos_in_canvas);
				if (!ImGui::IsMouseDown(0))
					adding_line = adding_preview = false;
			}
			if (ImGui::IsItemHovered())
			{
				if (!adding_line && ImGui::IsMouseClicked(0))
				{
					points.push_back(mouse_pos_in_canvas);
					adding_line = true;
				}
				if (ImGui::IsMouseClicked(1) && !points.empty())
				{
					adding_line = adding_preview = false;
					points.pop_back();
					points.pop_back();
				}
			}
			draw_list->PushClipRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), true);      // clip lines within the canvas (if we resize it, etc.) 
			for (int i = 0; i < points.Size - 1; i += 2)
				draw_list->AddLine(ImVec2(canvas_pos.x + points[i].x, canvas_pos.y + points[i].y), ImVec2(canvas_pos.x + points[i + 1].x, canvas_pos.y + points[i + 1].y), IM_COL32(255, 255, 0, 255), 2.0f);
			draw_list->PopClipRect();
			if (adding_preview)
				points.pop_back();
		}
		ImGui::End();
	}

	void RenderImGui(ImGuiData& data) {
		//DisplayTimeline(data);

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