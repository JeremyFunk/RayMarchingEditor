#include "imgui_data.h"
#include <string>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "constants.h"
#include "RMIO.h"
#include "scene.h"
#include "imgui_animation.h"
namespace RMImGui {
	const ImU32 timeline_rect_bar = ImColor(ImVec4(0.6f, 0.6f, 0.4f, 0.75f));
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

		ImGui::PushItemWidth(100);
		controls_x += 35;
		ImGui::SameLine(controls_x);
		const char* items[] = {
			"Overview",
			"Detailed"
		};




		const char* current_item;
		if (data.timeline.display == TimelineDisplay::Detailed) {
			current_item = "Detailed";
		}
		else {
			current_item = "Overview";
		}

		if (ImGui::BeginCombo("##TimelineDisplay", current_item)) {
			for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
				bool is_selected = false;
				if (ImGui::Selectable(items[n], is_selected)) {
					if (items[n] == "Detailed") {
						data.timeline.display = TimelineDisplay::Detailed;
					}
					else if (items[n] == "Overview") {
						data.timeline.display = TimelineDisplay::Overview;
					}
				}
			}
			ImGui::EndCombo();
		}

		//ImGui::SameLine(controls_x); ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.pause), ImVec2(15.0, 15.0)); controls_x += 25;
		//ImGui::SameLine(controls_x); ImGui::ImageButton((void*)(intptr_t)(data.textures.timeline.stop), ImVec2(15.0, 15.0)); controls_x += 25;
	}

	static ImVec2 last_mouse_pos = ImVec2(FLT_MIN, FLT_MIN);

	void DisplayDetailedKeyframe(AnimatedFloat& f, const char* name, ImDrawList* draw_list, float* height, float scale, float display_height, float px, float py) {
		if (ImGui::TreeNode(name)) {
			draw_list->AddRectFilled(ImVec2(px, py + *height), ImVec2(px + 100.0, py + *height + display_height), timeline_rect_bar, 0.0f, ImDrawCornerFlags_All);

			ImGui::TreePop();
		}
		*height += scale;
	}

	void DisplayDetailedVector3(AnimatedFloatVec3& vec, const char* name, ImDrawList* draw_list, float* height, float scale, float display_height, float px, float py) {
		if (ImGui::TreeNode(name)) {
			*height += scale;
			if (vec[0].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[0], "X", draw_list, height, scale, display_height, px, py);
			}
			if (vec[1].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[1], "Y", draw_list, height, scale, display_height, px, py);
			}
			if (vec[2].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[2], "Z", draw_list, height, scale, display_height, px, py);
			}

			ImGui::TreePop();
		}
	}

	void DisplayTimeline(ImGuiData& data) {
		if (last_mouse_pos.x == FLT_MIN) {
			last_mouse_pos = ImGui::GetMousePos();
		}

		bool open = false;
		if (!ImGui::Begin("Timeline", &open))
		{
			ImGui::End();
			return;
		}
		DisplayTimelineTopBar(data);

		auto animation_data = data.selectedAnimationData();
		auto keyframes = animation_data.getKeyframes();

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
		const float TIMELINE_OBJECTS_WIDTH = 200;
		float timeline_step_width = 7.5;
		float timeline_height = 1000.0;

		int frameOffset = -data.timeline.offset / timeline_step_width;
		float posOffset = -data.timeline.offset;
		x -= posOffset;
		float min_x = p.x;

		if (data.timeline.display == TimelineDisplay::Detailed) {
			x += TIMELINE_OBJECTS_WIDTH + 2;
			min_x += TIMELINE_OBJECTS_WIDTH;
		}
		draw_list->AddRectFilled(ImVec2(x, p.y), ImVec2(min_x + s.x, y + TIMELINE_NUMBER_BAR_SIZE), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
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
			if (x_pos < min_x)
				continue;
			if (i % 10 == 0)
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + timeline_height), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
			else
				draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 1, y + timeline_height), timeline_rect_light, 0.0f, ImDrawCornerFlags_All);
		}

		if (data.timeline.display == TimelineDisplay::Overview) {
			for (auto a : keyframes) {
				auto x_pos = x + a * timeline_step_width;
				if (x_pos > min_x) {
					draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + timeline_height), timeline_rect_keyframe, 0.0f, ImDrawCornerFlags_All);
				}
			}
		}

		auto timeline_frame_x = x + data.timeline.frame * timeline_step_width;
		if (timeline_frame_x > min_x) {
			draw_list->AddRectFilled(ImVec2(timeline_frame_x, y), ImVec2(timeline_frame_x + 2, y + timeline_height), timeline_rect_accent, 0.0f, ImDrawCornerFlags_All);
		}

		if (data.timeline.display == TimelineDisplay::Detailed) {
			for (auto a : keyframes) {
				auto x_pos = x + a * timeline_step_width;
				if (x_pos > min_x) {
					draw_list->AddRectFilled(ImVec2(x_pos, y), ImVec2(x_pos + 2, y + timeline_height), timeline_rect_keyframe, 0.0f, ImDrawCornerFlags_All);
				}
			}
		}


		// Timeline covers
		draw_list->AddRectFilled(ImVec2(min_x, y), ImVec2(std::max(min_x, x + data.timeline.min_frame * timeline_step_width), y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);
		draw_list->AddRectFilled(ImVec2(std::max(min_x, min_x + data.timeline.max_frame * timeline_step_width + data.timeline.offset), y), ImVec2(std::max(min_x, p.x + s.x), y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);


		if (data.timeline.display == TimelineDisplay::Detailed) {
			draw_list->AddRectFilled(ImVec2(p.x - 5, p.y), ImVec2(p.x + TIMELINE_OBJECTS_WIDTH, p.y + TIMELINE_NUMBER_BAR_SIZE + 2), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
			draw_list->AddRectFilled(ImVec2(p.x + TIMELINE_OBJECTS_WIDTH, p.y), ImVec2(p.x + TIMELINE_OBJECTS_WIDTH + 2, p.y + timeline_height + TIMELINE_NUMBER_BAR_SIZE + 2), timeline_rect_light, 0.0f, ImDrawCornerFlags_All);


			ImGui::Dummy(ImVec2(0.0f, TIMELINE_NUMBER_BAR_SIZE-2));
			
			float display_height = 14.0;
			float scale = 17.0;
			float height = 0.0;

			if (ImGui::TreeNode("Objects"))
			{
				height += scale;
				for (auto o : data.primitives) {
					if (o.prim_type != 0 && ImGui::TreeNode(o.name.c_str()))
					{
						height += scale;
						if (o.transformation.containsKeyframes() && ImGui::TreeNode("Transformation")) {
							height += scale;
							if (o.transformation.containsKeyframes()) {
								if(o.transformation.position.containsKeyframes())
									DisplayDetailedVector3(o.transformation.position, "Position", draw_list, &height, scale, display_height, min_x, y);
								if (o.transformation.rotation.containsKeyframes())
									DisplayDetailedVector3(o.transformation.rotation, "Rotation", draw_list, &height, scale, display_height, min_x, y);
								if (o.transformation.scale.containsKeyframes())
									DisplayDetailedVector3(o.transformation.scale, "Scale", draw_list, &height, scale, display_height, min_x, y);
							}
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}

		auto mouse_pos = ImGui::GetMousePos();
		auto transformed_pos = ImVec2(mouse_pos.x - min_x, mouse_pos.y - p.y);

		// Mouse events
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			if (transformed_pos.y > TIMELINE_NUMBER_BAR_SIZE && transformed_pos.y < TIMELINE_NUMBER_BAR_SIZE + timeline_height && transformed_pos.x > 0 && transformed_pos.x < s.x) {
				data.drag = DragStart::Timeline;
			}
			else if (transformed_pos.y > 0 && transformed_pos.y < TIMELINE_NUMBER_BAR_SIZE && transformed_pos.x > 0 && transformed_pos.x < s.x) {
				data.drag = DragStart::TopBar;
			}
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			data.drag = DragStart::None;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (data.drag == DragStart::Timeline) {
				ImGui::ResetMouseDragDelta();
				data.timeline.offset = std::min(data.timeline.offset + (mouse_pos.x - last_mouse_pos.x), 0.0f);
			}
			else if (data.drag == DragStart::TopBar) {
				data.timeline.frame = std::round((transformed_pos.x / timeline_step_width) - data.timeline.offset / timeline_step_width);
			}
		}

		ImGui::End();
		last_mouse_pos = mouse_pos;
	}

}
