
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "animator.h"
#include <imgui_internal.h>

namespace ImGui {

    bool KeyframeDragFloat(const char* label, int frame, AnimatedFloat* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
        ImGuiStyle* style = &ImGui::GetStyle();
        auto clicked = false;
        if (v->ContainsKeyframe(frame)) {
            auto a = style->Colors[ImGuiCol_FrameBg];
            auto b = style->Colors[ImGuiCol_FrameBgActive];
            auto c = style->Colors[ImGuiCol_FrameBgHovered];
            style->Colors[ImGuiCol_FrameBg] = ImVec4(0.5, 0.5, 0.3, 0.6);
            style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.6, 0.6, 0.38, 0.6);
            style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.55, 0.55, 0.35, 0.6);
            clicked = ImGui::DragFloat(label, &v->value, v_speed, v_min, v_max, format, flags);
            style->Colors[ImGuiCol_FrameBg] = a;
            style->Colors[ImGuiCol_FrameBgActive] = b;
            style->Colors[ImGuiCol_FrameBgHovered] = c;
        }
        else {
            clicked = ImGui::DragFloat(label, &v->value, v_speed, v_min, v_max, format, flags);
        }
        if (ImGui::IsItemHovered() && ImGui::IsKeyReleased(ImGuiKey_K)) {
            v->AddKeyframe(frame, v->value);
        }
        return clicked;
    }

    bool KeyframeDragFloatN(const char* label, int frame, AnimatedFloat* p_data[], int components, float v_speed, const float p_min, const float p_max, const char* format, ImGuiSliderFlags flags)
    {
        ImGuiStyle* style = &ImGui::GetStyle();
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        bool value_changed = false;
        BeginGroup();
        PushID(label);
        PushMultiItemsWidths(components, CalcItemWidth());
        size_t type_size = sizeof(AnimatedFloat);
        for (int i = 0; i < components; i++)
        {
            PushID(i);
            if (i > 0)
                SameLine(0, g.Style.ItemInnerSpacing.x);
            value_changed |= KeyframeDragFloat("", frame, p_data[i], v_speed, p_min, p_max, format, flags);
            PopID();
            PopItemWidth();
        }
        PopID();

        const char* label_end = FindRenderedTextEnd(label);
        if (label != label_end)
        {
            SameLine(0, g.Style.ItemInnerSpacing.x);
            TextEx(label, label_end);
        }

        EndGroup();
        return value_changed;
    }

    bool KeyframeDragFloat3(const char* label, int frame, AnimatedFloat* v[3], float v_speed, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
        bool clicked = KeyframeDragFloatN(label, frame, v, 3, v_speed, v_min, v_max, format, flags);

        return clicked;
    }

    bool KeyframeDragFloat2(const char* label, int frame, AnimatedFloat* v[2], float v_speed, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
        bool clicked = KeyframeDragFloatN(label, frame, v, 2, v_speed, v_min, v_max, format, flags);

        return clicked;
    }
}
