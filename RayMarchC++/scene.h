#include <vector>
#include "primitive.h"
#include "constants.h"
#include "imgui_handler.h"
#include "json_struct.h"
namespace Scene {

    struct SceneGroupModifier {
        Primitive::GroupModifierType modifier;
        int prim0, prim1, prim2, prim3;
        float primAttribute;
    };
    struct SceneModifier {
        Primitive::ModifierType modifier;
        float attribute0;
        float attribute1;
        float attribute2;
        float attribute3;
        float attribute4;
    };
    struct SceneObject {
        glm::vec3 position, rotation, scale;
        int prim_type;
        float values[10];
        std::string name;
        std::vector<SceneModifier> modifiers;
    };

    struct Scene {
        std::vector<SceneObject> objects;
        std::vector<SceneGroupModifier> group_modifiers;
    };

    Scene createScene(RMImGui::ImGuiData& data);
    RMImGui::ImGuiData convertScene(Scene scene);
    std::string toJson(Scene scene);
    Scene toScene(std::string str);
}
