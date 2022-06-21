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

    struct Scene {
        std::vector<SceneObject> objects;
        std::vector<SceneGroupModifier> group_modifiers;
        glm::vec3 cam_pos, cam_rot;
        glm::vec2 cam_py;
    };

    Scene createScene(RMImGui::ImGuiData& data);
    RMImGui::ImGuiData convertScene(Scene scene);
    std::string toJson(Scene scene);
    Scene toScene(std::string str);
}
