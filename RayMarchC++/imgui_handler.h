#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "primitive.h"
#include "constants.h"
namespace RMImGui {
    struct ImGuiData {
        int* shading_mode;
        Primitive::ShaderPrimitive primitives[COUNT_PRIMITIVE];
        Primitive::ShaderGroupPrimitive groupPrimitives[COUNT_GROUP_MODIFIER];

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