#pragma once
#include <vector>
#include "primitive.h"
#include "constants.h"
#include "imgui_handler.h"
#include "json_struct.h"
namespace Scene {

    struct SceneGroupModifier {
        Primitive::GroupModifierType modifier;
        int prim0, prim1, prim2, prim3;
        AnimatedFloat prim_attribute;
    };
    struct SceneModifier {
        Primitive::ModifierType modifier;
        AnimatedFloat attribute0;
        AnimatedFloat attribute1;
        AnimatedFloat attribute2;
        AnimatedFloat attribute3;
        AnimatedFloat attribute4;
    };
    struct SceneObject {
        AnimatedFloatVec3 position, rotation, scale;
        int prim_type;
        AnimatedFloat values[10];
        std::string name;
        std::vector<SceneModifier> modifiers;
    };

    struct SceneScript {
        boost::array<char, SCRIPT_SIZE> script = boost::array<char, SCRIPT_SIZE>();
        std::string name;
    };

    struct SceneGlobal {
        std::string name;
        AnimatedFloat f;
    };

    struct SceneLight {
        std::string name;
        AnimatedFloatVec3 color;
        AnimatedFloat attribute0, attribute1, attribute2;
        AnimatedFloat intensity;
        int type;
    };

    struct Scene {
        std::vector<SceneObject> objects = std::vector<SceneObject>();
        std::vector<SceneGroupModifier> group_modifiers = std::vector<SceneGroupModifier>();
        std::vector<SceneScript> scripts = std::vector<SceneScript>();
        std::vector<SceneLight> lights = std::vector<SceneLight>();
        std::string directory;
        RMImGui::CameraData cam_data;
        AnimatedFloatVec3 cam_pos;
        AnimatedFloatVec2 cam_py; 
        std::vector<SceneGlobal> globals = std::vector<SceneGlobal>();
    };

    Scene createScene(RMImGui::ImGuiData& data);
    RMImGui::ImGuiData convertScene(Scene scene);
    std::string toJson(Scene scene);
    Scene toScene(std::string str, std::string path);
}
