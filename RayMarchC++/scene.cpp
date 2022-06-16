#include "scene.h"
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;
namespace Scene {

    Scene createScene(RMImGui::ImGuiData& data) {
        Scene scene;
        scene.objects = std::vector<SceneObject>();
        scene.group_modifiers = std::vector<SceneGroupModifier>();

        for (int i = 0; i < COUNT_PRIMITIVE; i++) {
            if (data.primitives[i].prim_type != 0) {
                SceneObject object;
                object.position = data.primitives[i].transformation.position;
                object.rotation = data.primitives[i].transformation.rotation;
                object.scale = data.primitives[i].transformation.scale;
                object.prim_type = data.primitives[i].prim_type;
                object.name = data.primitives[i].name;
                for (int j = 0; j < 10; j++) {
                    object.values[j] = data.primitives[i].values[j];
                }
                object.modifiers = std::vector<SceneModifier>();
                for (int j = 0; j < data.primitives[i].mod_count; j++) {
                    if (data.primitives[i].modifiers[j].modifier != 0) {
                        SceneModifier m;
                        m.attribute0 = data.primitives[i].modifiers[j].attribute0;
                        m.attribute1 = data.primitives[i].modifiers[j].attribute1;
                        m.attribute2 = data.primitives[i].modifiers[j].attribute2;
                        m.attribute3 = data.primitives[i].modifiers[j].attribute3;
                        m.attribute4 = data.primitives[i].modifiers[j].attribute4;
                        m.modifier = data.primitives[i].modifiers[j].modifier;
                        object.modifiers.push_back(m);
                    }
                }

                scene.objects.push_back(object);
            }
        }

        for (int j = 0; j < COUNT_GROUP_MODIFIER; j++) {
            if (data.groupPrimitives[j].modifier != 0) {
                SceneGroupModifier g;
                g.modifier = data.groupPrimitives[j].modifier;
                g.prim0 = data.groupPrimitives[j].prim0;
                g.prim1 = data.groupPrimitives[j].prim1;
                g.prim2 = data.groupPrimitives[j].prim2;
                g.prim3 = data.groupPrimitives[j].prim3;
                g.prim_attribute = data.groupPrimitives[j].primAttribute;

                scene.group_modifiers.push_back(g);
            }
        }

        return scene;
    }
    std::string toJson(Scene scene) {
        std::vector<json> objects = std::vector<json>();
        std::vector<json> group_modifiers = std::vector<json>();

        for (int i = 0; i < scene.objects.size(); i++) {
            json o;
            std::vector<json> modifiers = std::vector<json>();
            for (int j  = 0; j < scene.objects[i].modifiers.size(); j++) {
                json m;
                m["modifier"] = scene.objects[i].modifiers[j].modifier;
                m["attribute0"] = scene.objects[i].modifiers[j].attribute0;
                m["attribute1"] = scene.objects[i].modifiers[j].attribute1;
                m["attribute2"] = scene.objects[i].modifiers[j].attribute2;
                m["attribute3"] = scene.objects[i].modifiers[j].attribute3;
                m["attribute4"] = scene.objects[i].modifiers[j].attribute4;
                modifiers.push_back(m);
            }
            o["position"][0] = scene.objects[i].position.x;
            o["position"][1] = scene.objects[i].position.y;
            o["position"][2] = scene.objects[i].position.z;

            o["rotation"][0] = scene.objects[i].rotation.x;
            o["rotation"][1] = scene.objects[i].rotation.y;
            o["rotation"][2] = scene.objects[i].rotation.z;

            o["scale"][0] = scene.objects[i].scale.x;
            o["scale"][1] = scene.objects[i].scale.y;
            o["scale"][2] = scene.objects[i].scale.z;

            o["prim_type"] = scene.objects[i].prim_type;
            o["values"] = scene.objects[i].values;

            o["modifiers"] = modifiers;

            o["name"] = scene.objects[i].name;

            objects.push_back(o);
        }

        for (int i = 0; i < scene.group_modifiers.size(); i++) {
            json g;
            
            g["modifier"] = scene.group_modifiers[i].modifier;
            g["prim0"] = scene.group_modifiers[i].prim0;
            g["prim1"] = scene.group_modifiers[i].prim1;
            g["prim2"] = scene.group_modifiers[i].prim2;
            g["prim3"] = scene.group_modifiers[i].prim3;
            g["prim_attribute"] = scene.group_modifiers[i].prim_attribute;

            group_modifiers.push_back(g);
        }

        json j;
        j["objects"] = objects;
        j["group_modifiers"] = group_modifiers;

        return j.dump();
    }

    Scene toScene(std::string str) {
        json j = json::parse(str);
        Scene s;
        s.objects = std::vector<SceneObject>();
        s.group_modifiers = std::vector<SceneGroupModifier>();
        
        for (int i = 0; i < j["objects"].size(); i++) {
            json jo = j["objects"][i];
            SceneObject o;
            o.modifiers = std::vector<SceneModifier>();
            o.position = glm::vec3(jo["position"][0], jo["position"][1], jo["position"][2]);
            o.rotation = glm::vec3(jo["rotation"][0], jo["rotation"][1], jo["rotation"][2]);
            o.scale = glm::vec3(jo["scale"][0], jo["scale"][1], jo["scale"][2]);

            o.name = jo["name"].get<std::string>();

            o.prim_type = jo["prim_type"];
            for (int n = 0; n < jo["values"].size(); n++) {
                o.values[n] = jo["values"][n];
            }
            for (int n = 0; n < jo["modifiers"].size(); n++) {
                json jm = jo["modifiers"][n];
                SceneModifier m;
                m.modifier = jm["modifier"];
                m.attribute0 = jm["attribute0"];
                m.attribute1 = jm["attribute1"];
                m.attribute2 = jm["attribute2"];
                m.attribute3 = jm["attribute3"];
                m.attribute4 = jm["attribute4"];

                o.modifiers.push_back(m);
            }
            s.objects.push_back(o);
        }

        for (int i = j["objects"].size(); i < COUNT_PRIMITIVE; i++) {
            SceneObject o;

            o.prim_type = 0;
            s.objects.push_back(o);

        }

        for (int i = 0; i < j["group_modifiers"].size(); i++) {
            json jg = j["group_modifiers"][i];
            SceneGroupModifier g;

            g.modifier = jg["modifier"];
            g.prim_attribute = jg["prim_attribute"];
            g.prim0 = jg["prim0"];
            g.prim1 = jg["prim1"];
            g.prim2 = jg["prim2"];
            g.prim3 = jg["prim3"];

            s.group_modifiers.push_back(g);
        }

        for (int i = j["group_modifiers"].size(); i < COUNT_GROUP_MODIFIER; i++) {
            SceneGroupModifier g;

            g.modifier = Primitive::GroupModifierType::NONE_GROUP;
            g.prim_attribute = 0;

            s.group_modifiers.push_back(g);
        }

        return s;
    }

    RMImGui::ImGuiData convertScene(Scene scene) {
        RMImGui::ImGuiData d;
        for (int i = 0; i < scene.group_modifiers.size(); i++) {
            Primitive::ShaderGroupPrimitive g;
            g.modifier = scene.group_modifiers[i].modifier;
            g.prim0 = scene.group_modifiers[i].prim0;
            g.prim1 = scene.group_modifiers[i].prim1;
            g.prim2 = scene.group_modifiers[i].prim2;
            g.prim3 = scene.group_modifiers[i].prim3;
            g.primAttribute = scene.group_modifiers[i].prim_attribute;

            d.groupPrimitives[i] = g;
        }


        for (int i = 0; i < scene.objects.size(); i++) {
            Primitive::ShaderPrimitive p;
            for (int j = 0; j < 10; j++) {
                p.values[j] = scene.objects[i].values[j];
            }

            for (int j = 0; j < scene.objects[i].modifiers.size(); j++) {
                Primitive::Modifier m;
                m.attribute0 = scene.objects[i].modifiers[j].attribute0;
                m.attribute1 = scene.objects[i].modifiers[j].attribute1;
                m.attribute2 = scene.objects[i].modifiers[j].attribute2;
                m.attribute3 = scene.objects[i].modifiers[j].attribute3;
                m.attribute4 = scene.objects[i].modifiers[j].attribute4;
                m.modifier = scene.objects[i].modifiers[j].modifier;

                p.modifiers[j] = m;

                if (m.modifier != Primitive::ModifierType::NONE_MOD) {
                    p.mod_count += 1;
                }
            }

            p.transformation.position = glm::vec3(scene.objects[i].position[0], scene.objects[i].position[1], scene.objects[i].position[2]);
            p.transformation.rotation = glm::vec3(scene.objects[i].rotation[0], scene.objects[i].rotation[1], scene.objects[i].rotation[2]);
            p.transformation.scale = glm::vec3(scene.objects[i].scale[0], scene.objects[i].scale[1], scene.objects[i].scale[2]);
            Primitive::updateTransformation(&p);
            
            p.name = scene.objects[i].name;

            p.prim_type = scene.objects[i].prim_type;
            
            d.primitives[i] = p;
        }
        return d;
    }
}