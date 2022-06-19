#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "primitive.h"
#include "constants.h"
namespace RMImGui {
    enum class TimelineMode {
        Play, Reverse, Pause
    };

    struct TexturesTimeline {
        int play, pause, stop, next, previous, reverse_play;
    };

    struct Textures {
        TexturesTimeline timeline;
    };

    struct TimelineData {
        TimelineMode mode = TimelineMode::Pause;
        int frame = 0;
        float time = 0.0;
        float fps = 30.0;
        int min_frame = 0, max_frame = 200;

        void update(float dt) {
            if (mode == TimelineMode::Play) {
                time += dt;
                while (time > 1.0 / fps) {
                    frame += 1;
                    time -= 1.0 / fps;
                }

                if (frame >= max_frame) {
                    mode = TimelineMode::Pause;
                    frame = max_frame;
                }
            }
            if (mode == TimelineMode::Reverse) {
                time += dt;
                while (time > 1.0 / fps) {
                    frame -= 1;
                    time -= 1.0 / fps;
                }

                if (frame <= min_frame) {
                    mode = TimelineMode::Pause;
                    frame = min_frame;
                }
            }
        }
    };

    struct ImGuiData {
        int* shading_mode;
        Primitive::ShaderPrimitive primitives[COUNT_PRIMITIVE];
        Primitive::ShaderGroupPrimitive groupPrimitives[COUNT_GROUP_MODIFIER];
        glm::vec3 cam_pos, cam_rot;
        glm::vec2 cam_py;
        bool reposition_cam = false;
        Textures textures;
        TimelineData timeline;

        void addPrimitive(Primitive::ShaderPrimitive prim) {
            for (int i = 0; i < COUNT_PRIMITIVE; i++) {
                if (primitives[i].prim_type == 0) {
                    primitives[i] = prim;
                    break;
                }
            }
        }


        void remove_group_modifier(int index) {
            groupPrimitives[index].modifier = Primitive::GroupModifierType::NONE_GROUP;
            for (int i = 0; i < COUNT_GROUP_MODIFIER - 1; i++) {
                if (groupPrimitives[i].modifier == 0) {
                    for (int j = i + 1; j < COUNT_GROUP_MODIFIER; j++) {
                        if (groupPrimitives[j].modifier != 0) {
                            groupPrimitives[i] = groupPrimitives[j];
                            groupPrimitives[j].modifier = Primitive::GroupModifierType::NONE_GROUP;
                            break;
                        }
                    }
                }
            }
        }
    };

    void SetupImGui(GLFWwindow* window);
    void RenderImGui(ImGuiData& data);
}