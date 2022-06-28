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
	struct KeyframeBarData {
		float x1, x2, y;
		std::vector<int> frames;
		std::string name;
		std::vector<AnimatedFloat*> floats;
		KeyframeBarData(int x1, int x2, int y, std::string name): x1(x1), x2(x2), y(y), name(name) {
			frames = std::vector<int>();
			floats = std::vector<AnimatedFloat*>();
		}
		KeyframeBarData() {
			frames = std::vector<int>();
			floats = std::vector<AnimatedFloat*>();
		}
	};

	const ImU32 timeline_rect_bar = ImColor(ImVec4(0.6f, 0.6f, 0.4f, 0.75f));
	const ImU32 timeline_rect_frame_bar = ImColor(ImVec4(0.9f, 0.9f, 0.6f, 0.8f));
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

	bool DisplayTreeNode(const char* label, int* height) {
		*height += 1;
		return ImGui::TreeNode(label);
	}

	void DisplayDetailedKeyframe(AnimatedFloat& f, const char* name, std::string full_name, ImDrawList* draw_list, int* height, std::vector<KeyframeBarData>* data) {
		if (DisplayTreeNode(name, height)) {
			int first = f.firstFrame();
			int last = f.lastFrame();
			auto k = KeyframeBarData(first, last, *height, full_name);
			k.floats.push_back(&f);
			f.getKeyframes(&k.frames);
			data->push_back(k);
			

			ImGui::TreePop();
		}
	}

	void DisplayDetailedVector3(AnimatedFloatVec3& vec, const char* name, std::string full_name, ImDrawList* draw_list, int* height, std::vector<KeyframeBarData>* data) {
		if (DisplayTreeNode(name, height)) {
			int first = vec.firstFrame();
			int last = vec.lastFrame();
			auto k = KeyframeBarData(first, last, *height, full_name);
			k.floats.push_back(&vec[0]);
			k.floats.push_back(&vec[1]);
			k.floats.push_back(&vec[2]);
			data->push_back(k);

			if (vec[0].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[0], "X", full_name + "/X", draw_list, height, data);
			}
			if (vec[1].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[1], "Y", full_name + "/Y", draw_list, height, data);
			}
			if (vec[2].ContainsKeyframes()) {
				DisplayDetailedKeyframe(vec[2], "Z", full_name + "/Z", draw_list, height, data);
			}

			ImGui::TreePop();
		}
	}

	//static ImVec2 values[] = { ImVec2(0.0f, 0.0f), ImVec2(0.0f, 0.0f), ImVec2(100.0f, 100.0f), ImVec2(100.0f, 100.0f) };
	bool DisplayBezier(ImGuiData& data, int bezier_id) {
		auto window = &data.bezier_animation_windows[bezier_id];
		bool open = true;
		ImGui::Begin(std::string("Edit Animation: " + window->name).c_str(), &open);

		auto s = ImGui::GetContentRegionAvail();
		const ImVec2 p = ImGui::GetCursorScreenPos();

		int x_range_min, x_range_max;
		float y_range_min = FLT_MAX, y_range_max = FLT_MIN;

		x_range_min = window->f->keyframes[0].frame;
		x_range_max = window->f->keyframes[window->f->keyframes.size() - 1].frame;

		for (auto k : window->f->keyframes) {
			if (k.value < y_range_min) {
				y_range_min = k.value;
			}
			if (k.value > y_range_max) {
				y_range_max = k.value;
			}
		}



		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		const ImU32 bez_curve_color = ImColor(ImVec4(0.6f, 0.6f, 0.4f, 0.75f));
		const ImU32 bez_point_color = ImColor(ImVec4(0.8f, 0.8f, 0.6f, 0.75f));

		float circle_size = 4.0;
		float click_dist = 8.0;

		float padding = 5;
		float min_x = p.x + padding;
		float min_y = p.y + padding;
		float s_x = s.x - padding * 2;
		float s_y = s.y - padding * 2;


		float x_diff = x_range_max - x_range_min;
		float y_diff = y_range_max - y_range_min;

		struct BezierPointInfo {
			ImVec2 p;
			ImVec2 inbound, outbound;
			BezierPointInfo(ImVec2 p, ImVec2 inbound, ImVec2 outbound) : p(p), inbound(inbound), outbound(outbound) {

			}
		};

		auto values = std::vector<BezierPointInfo>();
		for (int i = 0; i < window->f->keyframes.size(); i++) {
			auto p = ImVec2(((window->f->keyframes[i].frame - x_range_min) / x_diff)* s_x + min_x, (1.0 - (window->f->keyframes[i].value - y_range_min) / y_diff)* s_y + min_y);

			if (i == 0) {
				auto v1 = ImVec2(((window->f->keyframes[i].frame + window->f->keyframes[i].inter_x_out - x_range_min) / x_diff) * s_x + min_x, (1.0 - (window->f->keyframes[i].value + window->f->keyframes[i].inter_y_out - y_range_min) / y_diff) * s_y + min_y);
				values.push_back(BezierPointInfo(p, ImVec2(FLT_MIN, FLT_MIN), v1));
			}
			else if (i == window->f->keyframes.size() - 1) {
				auto v1 = ImVec2(((window->f->keyframes[i].frame + window->f->keyframes[i].inter_x_in - x_range_min) / x_diff) * s_x + min_x, (1.0 - (window->f->keyframes[i].value + window->f->keyframes[i].inter_y_in - y_range_min) / y_diff) * s_y + min_y);
				values.push_back(BezierPointInfo(p, v1, ImVec2(FLT_MIN, FLT_MIN)));
			}
			else {
				auto v1 = ImVec2(((window->f->keyframes[i].frame + window->f->keyframes[i].inter_x_in - x_range_min) / x_diff) * s_x + min_x, (1.0 - (window->f->keyframes[i].value + window->f->keyframes[i].inter_y_in - y_range_min) / y_diff) * s_y + min_y);
				auto v2 = ImVec2(((window->f->keyframes[i].frame + window->f->keyframes[i].inter_x_out - x_range_min) / x_diff) * s_x + min_x, (1.0 - (window->f->keyframes[i].value + window->f->keyframes[i].inter_y_out - y_range_min) / y_diff) * s_y + min_y);
				values.push_back(BezierPointInfo(p, v1, v2));
			}
		}
		for (int i = 0; i < values.size(); i++) {
			draw_list->AddCircleFilled(values[i].p, circle_size, bez_point_color);
			draw_list->AddCircleFilled(values[i].inbound, circle_size, bez_point_color);
			draw_list->AddCircleFilled(values[i].outbound, circle_size, bez_point_color);

			if (values[i].inbound.x != FLT_MIN) {
				draw_list->AddLine(values[i].p, values[i].inbound, bez_curve_color, 1.0f);
			}
			if (values[i].outbound.x != FLT_MIN) {
				draw_list->AddLine(values[i].p, values[i].outbound, bez_curve_color, 1.0f);
			}
		}

		auto mouse_pos = ImGui::GetMousePos();
		auto transformed_mouse_pos = ImVec2(mouse_pos.x - min_x, mouse_pos.y - min_y);
		// Mouse events
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			for (int i = 0; i < values.size(); i ++) {
				auto k = &values[i];

				if (k->inbound.x != FLT_MIN) {
					if (mouse_pos.y > k->inbound.y - click_dist / 2 && mouse_pos.y < k->inbound.y + click_dist / 2 && mouse_pos.x > k->inbound.x - click_dist / 2 && mouse_pos.x < k->inbound.x + click_dist / 2) {
						data.drag = DragStart::BezierPoint;
						data.dragSubSubId = 0;
						data.dragSubId = i;
						data.dragId = bezier_id;
					}
				}

				if (k->outbound.x != FLT_MIN) {
					if (mouse_pos.y > k->outbound.y - click_dist / 2 && mouse_pos.y < k->outbound.y + click_dist / 2 && mouse_pos.x > k->outbound.x - click_dist / 2 && mouse_pos.x < k->outbound.x + click_dist / 2) {
						data.drag = DragStart::BezierPoint;
						data.dragSubSubId = 1;
						data.dragSubId = i;
						data.dragId = bezier_id;
					}
				}
			}
		}

		
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && data.drag == DragStart::BezierPoint && bezier_id == data.dragId) {
			auto point = &values[data.dragSubId];
			auto relative_mouse = ImVec2(mouse_pos.x - point->p.x, mouse_pos.y - point->p.y);

			float new_x = (relative_mouse.x / s.x) * x_diff;
			float new_y = (-(relative_mouse.y / s.y)) * y_diff;

			if (data.dragSubSubId == 0) {
				auto in_x = std::min(new_x, 0.0f);

				if (data.dragSubId != 0) {
					in_x = std::max(values[data.dragSubId - 1].p.x, in_x);
				}

				window->f->keyframes[data.dragSubId].inter_x_in = new_x;
				window->f->keyframes[data.dragSubId].inter_y_in = new_y;
			}else if (data.dragSubSubId == 1) {

				auto out_x = std::max(new_x, 0.0f);

				if (data.dragSubId != values.size() - 1) {
					out_x = std::min(values[data.dragSubId + 1].p.x, out_x);
				}
				window->f->keyframes[data.dragSubId].inter_x_out = out_x;
				window->f->keyframes[data.dragSubId].inter_y_out = new_y;
			}
		}

		for (int i = 0; i < values.size() - 1; i ++) {
			draw_list->AddBezierCubic(values[i].p, values[i].outbound, values[i+1].inbound, values[i+1].p, bez_curve_color, 1.0);
		}

		ImGui::End();
		return open;
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

		// Timeline covers
		draw_list->AddRectFilled(ImVec2(min_x, y), ImVec2(std::max(min_x, x + data.timeline.min_frame * timeline_step_width), y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);
		draw_list->AddRectFilled(ImVec2(std::max(min_x, min_x + data.timeline.max_frame * timeline_step_width + data.timeline.offset), y), ImVec2(std::max(min_x, p.x + s.x), y + timeline_height), timeline_rect_covers, 0.0f, ImDrawCornerFlags_All);

		auto bar_data = std::vector<KeyframeBarData>();
		auto mouse_pos = ImGui::GetMousePos();

		if (data.timeline.display == TimelineDisplay::Detailed) {
			draw_list->AddRectFilled(ImVec2(p.x - 5, p.y), ImVec2(p.x + TIMELINE_OBJECTS_WIDTH, p.y + TIMELINE_NUMBER_BAR_SIZE + 2), timeline_rect_strong, 0.0f, ImDrawCornerFlags_All);
			draw_list->AddRectFilled(ImVec2(p.x + TIMELINE_OBJECTS_WIDTH, p.y), ImVec2(p.x + TIMELINE_OBJECTS_WIDTH + 2, p.y + timeline_height + TIMELINE_NUMBER_BAR_SIZE + 2), timeline_rect_light, 0.0f, ImDrawCornerFlags_All);


			ImGui::Dummy(ImVec2(0.0f, TIMELINE_NUMBER_BAR_SIZE-2));
			
			//float display_height = 14.0;
			//float scale = 17.0;
			//float height = -scale;
			int height = -1;


			if (DisplayTreeNode("Objects", &height))
			{
				for (int i = 0; i < IM_ARRAYSIZE(data.primitives); i++) {
					if (data.primitives[i].prim_type != 0 && DisplayTreeNode(data.primitives[i].name.c_str(), &height))
					{
						if (data.primitives[i].transformation.containsKeyframes() && DisplayTreeNode("Transformation", &height)) {
							std::string full_name = data.primitives[i].name + "/Transformation";
							auto k = KeyframeBarData(data.primitives[i].transformation.firstFrame(), data.primitives[i].transformation.lastFrame(), height, full_name);
							k.floats.push_back(&data.primitives[i].transformation.position[0]);
							k.floats.push_back(&data.primitives[i].transformation.position[1]);
							k.floats.push_back(&data.primitives[i].transformation.position[2]);

							k.floats.push_back(&data.primitives[i].transformation.rotation[0]);
							k.floats.push_back(&data.primitives[i].transformation.rotation[1]);
							k.floats.push_back(&data.primitives[i].transformation.rotation[2]);

							k.floats.push_back(&data.primitives[i].transformation.scale[0]);
							k.floats.push_back(&data.primitives[i].transformation.scale[1]);
							k.floats.push_back(&data.primitives[i].transformation.scale[2]);

							bar_data.push_back(k);
							if (data.primitives[i].transformation.containsKeyframes()) {
								if (data.primitives[i].transformation.position.containsKeyframes())
									DisplayDetailedVector3(data.primitives[i].transformation.position, "Position", full_name + "/Position", draw_list, &height, &bar_data);
								if (data.primitives[i].transformation.rotation.containsKeyframes())
									DisplayDetailedVector3(data.primitives[i].transformation.rotation, "Rotation", full_name + "/Rotation", draw_list, &height, &bar_data);
								if (data.primitives[i].transformation.scale.containsKeyframes())
									DisplayDetailedVector3(data.primitives[i].transformation.scale, "Scale", full_name + "/Scale", draw_list, &height, &bar_data);
							}
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			for (int i = 0; i < bar_data.size(); i++) {
				auto b = bar_data[i];
				if (data.dragId == b.y && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
					
					int move = 0;
					data.dragData += (mouse_pos.x - last_mouse_pos.x);
					while (data.dragData > timeline_step_width) {
						move += 1;
						data.dragData -= timeline_step_width;
					}
					while (data.dragData < -timeline_step_width) {
						move -= 1;
						data.dragData += timeline_step_width;
					}
					if (move != 0) {
						if (data.drag == DragStart::KeyframeBar) {
							for (int j = 0; j < b.floats.size(); j++) {
								move = std::max(move, b.floats[j]->CanFrameMove(move));
							}
							for (int j = 0; j < b.floats.size(); j++) {
								bar_data[i].floats[j]->FrameMoves(move);
							}
						}
						else if (data.drag == DragStart::KeyframeBarFrame) {
							int swap = -1;
							bar_data[i].floats[0]->FrameMove(data.dragSubId, move, &swap);
							if (swap != -1) {
								data.dragSubId = swap;
							}
						}
					}
				}

				draw_list->AddRectFilled(ImVec2(std::max(min_x, x + b.x1 * timeline_step_width + 2), y + b.y * 17), ImVec2(std::max(min_x, x + b.x2 * timeline_step_width + 4), y + b.y * 17 + 14), timeline_rect_bar, 0.0f, ImDrawCornerFlags_All);

				for (auto k : b.frames) {
					if (x + k * timeline_step_width + 2 > min_x) {
						draw_list->AddRectFilled(ImVec2(x + k * timeline_step_width + 2, y + b.y * 17), ImVec2(x + k * timeline_step_width + 4, y + b.y * 17 + 14), timeline_rect_frame_bar, 0.0f, ImDrawCornerFlags_All);
					}
				}
			}
		}

		auto transformed_pos = ImVec2(mouse_pos.x - min_x, mouse_pos.y - p.y);

		int cursor = ImGuiMouseCursor_Arrow;
		// Mouse events
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && transformed_pos.y > TIMELINE_NUMBER_BAR_SIZE && transformed_pos.y < TIMELINE_NUMBER_BAR_SIZE + timeline_height && transformed_pos.x > 0 && transformed_pos.x < s.x) {
				data.drag = DragStart::Timeline;
			}
			else if (transformed_pos.y > 0 && transformed_pos.y < TIMELINE_NUMBER_BAR_SIZE && transformed_pos.x > 0 && transformed_pos.x < s.x) {
				data.drag = DragStart::TopBar;
			}
		}

		for (auto b : bar_data) {
			auto cur_min_x = x + b.x1 * timeline_step_width + 2;
			auto cur_max_x = x + b.x2 * timeline_step_width + 4;
			auto cur_y = y + b.y * 17;

			if (mouse_pos.x > cur_min_x && mouse_pos.x < cur_max_x && mouse_pos.y > cur_y && mouse_pos.y < cur_y + 14) {
				bool hoveringFrame = false;
				for (int i = 0; i < b.frames.size(); i++) {
					if (mouse_pos.x > x + b.frames[i] * timeline_step_width - 2 && mouse_pos.x < x + b.frames[i] * timeline_step_width + 6) {
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
							data.drag = DragStart::KeyframeBarFrame;
							data.dragSubId = i;
							data.dragId = b.y;
						}
						cursor = ImGuiMouseCursor_ResizeEW;
						hoveringFrame = true;
						break;
					}
				}
				if (!hoveringFrame) {
					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
						data.drag = DragStart::KeyframeBar;
						data.dragId = b.y;
					}
					cursor = ImGuiMouseCursor_Hand;
				}
				if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
					bool open = false;
					for (int i = 0; i < data.bezier_animation_windows.size(); i++) {
						if (b.floats.size() == 1) {
							if (data.bezier_animation_windows[i].f == b.floats[0]) {
								open = true;
							}
						}
					}
					if (!open) {
						data.bezier_animation_windows.push_back(BezierAnimationWindow(b.floats[0], b.name));
					}
				}
			}
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			data.drag = DragStart::None;
			data.dragId = 0;
			data.dragSubId = 0;
		}

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
			if (data.drag == DragStart::Timeline) {
				ImGui::ResetMouseDragDelta();
				data.timeline.offset = std::min(data.timeline.offset + (mouse_pos.x - last_mouse_pos.x), 0.0f);
			}
			else if (data.drag == DragStart::TopBar) {
				data.timeline.frame = std::round((transformed_pos.x / timeline_step_width) - data.timeline.offset / timeline_step_width);
			}
			else if (data.drag == DragStart::KeyframeBarFrame) {
				cursor = ImGuiMouseCursor_ResizeEW;
			}
			else if (data.drag == DragStart::KeyframeBar) {
				cursor = ImGuiMouseCursor_Hand;
			}
		}
		ImGui::SetMouseCursor(cursor);

		auto beziers = std::vector<BezierAnimationWindow>();
		for (int i = 0; i < data.bezier_animation_windows.size(); i++) {
			if (DisplayBezier(data, i)) {
				beziers.push_back(data.bezier_animation_windows[i]);
			}
		}
		data.bezier_animation_windows = beziers;

		ImGui::End();
		last_mouse_pos = mouse_pos;
	}

}
