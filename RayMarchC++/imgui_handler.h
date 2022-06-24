#pragma once
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "primitive.h"
#include "constants.h"
#include "imgui_data.h"
namespace RMImGui {

    void SetupImGui(GLFWwindow* window);
    void RenderImGui(ImGuiData& data);
}