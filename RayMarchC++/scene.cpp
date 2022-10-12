#include "scene.h"
#include "json.hpp"
#include <iostream>
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include "RMIO.h"
#include <regex>

using json = nlohmann::json;
namespace Scene {

    Scene createScene(RMImGui::ImGuiData& data) {
        Scene scene;
        scene.cam_pos = data.cam_pos;
        scene.cam_py = data.cam_py;
        scene.directory = data.project_path;
        scene.cam_data = data.cam_data;
        scene.samples = data.samples;

        for (int i = 1; i < data.materials.count; i++) {
            if (data.materials[i].active) {
                SceneMaterial sceneMat;
                sceneMat.albedo = data.materials[i].albedo;
                sceneMat.roughness = data.materials[i].roughness;
                sceneMat.metallic = data.materials[i].metallic;
                sceneMat.transmission = data.materials[i].transmission;
                sceneMat.ior = data.materials[i].ior;
                sceneMat.name = data.materials[i].name;
                scene.materials.push_back(sceneMat);
            }
        }

        for (int i = 0; i < data.lights.count; i++) {
            if (data.lights[i].type != 0) {
                SceneLight l;
                l.attribute0 = data.lights[i].attribute0;
                l.attribute1 = data.lights[i].attribute1;
                l.attribute2 = data.lights[i].attribute2;
                l.color = data.lights[i].color;
                l.intensity = data.lights[i].intensity;
                l.name = data.lights[i].name;
                l.type = data.lights[i].type;

                scene.lights.push_back(l);
            }
        }

        for (int i = 0; i < COUNT_PRIMITIVE; i++) {
            if (data.primitives[i].prim_type != 0) {
                SceneObject object;
                object.position = data.primitives[i].transformation.position;
                object.rotation = data.primitives[i].transformation.rotation;
                object.scale = data.primitives[i].transformation.scale;
                object.prim_type = data.primitives[i].prim_type;
                object.name = data.primitives[i].name;
                object.material = data.primitives[i].material;
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


        for (int i = 0; i < data.scripts.size(); i++) {
            SceneScript s;
            s.name = data.scripts[i].name;
            s.script = data.scripts[i].script;
            scene.scripts.push_back(s);
        }

        for (int i = 0; i < data.globals.size(); i++) {
            SceneGlobal g;
            g.name = data.globals[i].name;
            g.f = data.globals[i].f;

            scene.globals.push_back(g);
        }

        return scene;
    }

    void animatedFloatToJson(AnimatedFloat f, Scene &s, json& j) {
        try {
            for (int i = 0; i < f.keyframes.size(); i++) {
                j["keyframes"][i]["value"] = f.keyframes[i].value;
                j["keyframes"][i]["frame"] = f.keyframes[i].frame;
                j["keyframes"][i]["inter_x_in"] = f.keyframes[i].inter_x_in;
                j["keyframes"][i]["inter_x_out"] = f.keyframes[i].inter_x_out;
                j["keyframes"][i]["inter_y_in"] = f.keyframes[i].inter_y_in;
                j["keyframes"][i]["inter_y_out"] = f.keyframes[i].inter_y_out;
            }
            j["value"] = f.value;
            j["mode"] = f.mode;
            if(f.script != -1)
                j["script"] = s.scripts[f.script].name;
        }
        catch (std::exception e) {
            std::cout << "Could not convert float!" << std::endl;
        }
    }
    void animatedVectorToJson(AnimatedFloatVec3 f, Scene& s, json& j) {
        animatedFloatToJson(f[0], s, j[0]);
        animatedFloatToJson(f[1], s, j[1]);
        animatedFloatToJson(f[2], s, j[2]);
    }
    void animatedVector2ToJson(AnimatedFloatVec2 f, Scene& s, json& j) {
        animatedFloatToJson(f[0], s, j[0]);
        animatedFloatToJson(f[1], s, j[1]);
    }

    AnimatedFloat jsonToAnimatedFloat(json& j, Scene &s) {
        AnimatedFloat f;
        try {
            for (int i = 0; i < j["keyframes"].size(); i++) {
                f.AddKeyframe(j["keyframes"][i]["frame"], j["keyframes"][i]["value"], j["keyframes"][i]["inter_x_in"], j["keyframes"][i]["inter_x_out"], j["keyframes"][i]["inter_y_in"], j["keyframes"][i]["inter_y_out"]);
            }
            f.value = j["value"];
            f.mode = j["mode"];
            int index = -1;
            if (j.contains("script")) {
                for (int i = 0; i < s.scripts.size(); i++) {
                    if (s.scripts[i].name == j["script"]) {
                        index = i;
                        break;
                    }
                }

            }
            f.script = index;
        }
        catch (std::exception e) {
            std::cout << "Could not load float: " << j << std::endl;
        }
        
        return f;
    }
    AnimatedFloatVec3 jsonToAnimatedVector3(json& j, Scene &s) {
        return AnimatedFloatVec3(jsonToAnimatedFloat(j[0], s), jsonToAnimatedFloat(j[1], s), jsonToAnimatedFloat(j[2], s));
    }
    AnimatedFloatVec2 jsonToAnimatedVector2(json& j, Scene& s) {
        return AnimatedFloatVec2(jsonToAnimatedFloat(j[0], s), jsonToAnimatedFloat(j[1], s));
    }

    std::string toJson(Scene scene) {
        for (int i = 0; i < scene.scripts.size(); i++) {
            RMIO::Save(scene.directory + "\\scripts\\" + scene.scripts[i].name + ".lua", scene.scripts[i].script.data());
        }

        std::vector<json> objects = std::vector<json>();
        std::vector<json> group_modifiers = std::vector<json>();
        std::vector<json> globals = std::vector<json>();
        std::vector<json> lights = std::vector<json>();
        std::vector<json> materials = std::vector<json>();

        for (int i = 0; i < scene.materials.size(); i++) {
            json m;
            animatedFloatToJson(scene.materials[i].ior, scene, m["ior"]);
            animatedFloatToJson(scene.materials[i].transmission, scene, m["transmission"]);
            animatedFloatToJson(scene.materials[i].metallic, scene, m["metallic"]);
            animatedFloatToJson(scene.materials[i].roughness, scene, m["roughness"]);
            animatedVectorToJson(scene.materials[i].albedo, scene, m["albedo"]);
            m["name"] = scene.materials[i].name;

            materials.push_back(m);
        }

        for (int i = 0; i < scene.lights.size(); i++) {
            json l;
            animatedFloatToJson(scene.lights[i].attribute0, scene, l["attribute0"]);
            animatedFloatToJson(scene.lights[i].attribute1, scene, l["attribute1"]);
            animatedFloatToJson(scene.lights[i].attribute2, scene, l["attribute2"]);

            animatedVectorToJson(scene.lights[i].color, scene, l["color"]);
            animatedFloatToJson(scene.lights[i].intensity, scene, l["intensity"]);
            l["name"] = scene.lights[i].name;
            l["type"] = scene.lights[i].type;

            lights.push_back(l);
        }
        
        for (int i = 0; i < scene.objects.size(); i++) {
            json o;
            std::vector<json> modifiers = std::vector<json>();
            for (int j  = 0; j < scene.objects[i].modifiers.size(); j++) {
                json m;
                m["modifier"] = scene.objects[i].modifiers[j].modifier;
                animatedFloatToJson(scene.objects[i].modifiers[j].attribute0, scene, m["attribute0"]);
                animatedFloatToJson(scene.objects[i].modifiers[j].attribute1, scene, m["attribute1"]);
                animatedFloatToJson(scene.objects[i].modifiers[j].attribute2, scene, m["attribute2"]);
                animatedFloatToJson(scene.objects[i].modifiers[j].attribute3, scene, m["attribute3"]);
                animatedFloatToJson(scene.objects[i].modifiers[j].attribute4, scene, m["attribute4"]);
                modifiers.push_back(m);
            }
            animatedVectorToJson(scene.objects[i].position, scene, o["position"]);
            animatedVectorToJson(scene.objects[i].rotation, scene, o["rotation"]);
            animatedVectorToJson(scene.objects[i].scale, scene, o["scale"]);

            o["prim_type"] = scene.objects[i].prim_type;
            for (int j = 0; j < 10; j++) {
                animatedFloatToJson(scene.objects[i].values[j], scene, o["values"][j]);
            }

            o["modifiers"] = modifiers;
            o["name"] = scene.objects[i].name;
            o["material"] = scene.objects[i].material;

            objects.push_back(o);
        }

        for (int i = 0; i < scene.group_modifiers.size(); i++) {
            json g;
            
            g["modifier"] = scene.group_modifiers[i].modifier;
            g["prim0"] = scene.group_modifiers[i].prim0;
            g["prim1"] = scene.group_modifiers[i].prim1;
            g["prim2"] = scene.group_modifiers[i].prim2;
            g["prim3"] = scene.group_modifiers[i].prim3;
            animatedFloatToJson(scene.group_modifiers[i].prim_attribute, scene, g["prim_attribute"]);
            group_modifiers.push_back(g);
        }

        for (int i = 0; i < scene.globals.size(); i++) {
            json g;

            g["name"] = scene.globals[i].name;
            animatedFloatToJson(scene.globals[i].f, scene, g["f"]);

            globals.push_back(g);
        }

        json j;
        j["objects"] = objects;
        j["group_modifiers"] = group_modifiers;
        j["globals"] = globals;
        j["lights"] = lights;
        j["samples"] = scene.samples;
        j["materials"] = materials;
        animatedVectorToJson(scene.cam_pos, scene, j["camera"]["cam_pos"]);
        animatedVector2ToJson(scene.cam_py, scene, j["camera"]["cam_py"]);

        animatedFloatToJson(scene.cam_data.apeture_size, scene, j["camera"]["apeture_size"]);
        animatedFloatToJson(scene.cam_data.focal_length, scene, j["camera"]["focal_length"]);
        animatedFloatToJson(scene.cam_data.focus_dist, scene, j["camera"]["focus_dist"]);

        return j.dump();
    }

    Scene toScene(std::string str, std::string path) {
        std::regex newlines_re("\n+");

        str = std::regex_replace(str, newlines_re, "");

        json j = json::parse(str);
        Scene s;

        auto scripts = RMIO::GetFilesInDir(path + "\\scripts");
        for (auto script : scripts) {
            SceneScript ss;
            ss.name = RMIO::PathGetFilenamePart(script);

            std::string content = RMIO::Load(script);
            for (int i = 0; i < content.size(); i++) {
                ss.script[i] = content[i];
            }
            s.scripts.push_back(ss);
        }

        for (int i = 0; i < j["materials"].size(); i++) {
            SceneMaterial m;
            m.ior = jsonToAnimatedFloat(j["materials"][i]["ior"], s);
            m.transmission = jsonToAnimatedFloat(j["materials"][i]["transmission"], s);
            m.metallic = jsonToAnimatedFloat(j["materials"][i]["metallic"], s);
            m.roughness = jsonToAnimatedFloat(j["materials"][i]["roughness"], s);
            m.albedo = jsonToAnimatedVector3(j["materials"][i]["albedo"], s);
            m.name = j["materials"][i]["name"];
            s.materials.push_back(m);
        }

        for (int i = 0; i < j["lights"].size(); i++) {
            SceneLight l;
            l.attribute0 = jsonToAnimatedFloat(j["lights"][i]["attribute0"], s);
            l.attribute1 = jsonToAnimatedFloat(j["lights"][i]["attribute1"], s);
            l.attribute2 = jsonToAnimatedFloat(j["lights"][i]["attribute2"], s);
            l.intensity = jsonToAnimatedFloat(j["lights"][i]["intensity"], s);
            l.color = jsonToAnimatedVector3(j["lights"][i]["color"], s);
            l.name = j["lights"][i]["name"];
            l.type = j["lights"][i]["type"];

            s.lights.push_back(l);
        }

        s.cam_pos = jsonToAnimatedVector3(j["camera"]["cam_pos"], s);
        s.cam_py = jsonToAnimatedVector2(j["camera"]["cam_py"], s);
        s.cam_data.apeture_size = jsonToAnimatedFloat(j["camera"]["apeture_size"], s);
        s.cam_data.focal_length = jsonToAnimatedFloat(j["camera"]["focal_length"], s);
        s.cam_data.focus_dist = jsonToAnimatedFloat(j["camera"]["focus_dist"], s);
        s.samples = j["samples"];

        for (int i = 0; i < j["objects"].size(); i++) {
            json jo = j["objects"][i];
            SceneObject o;
            o.modifiers = std::vector<SceneModifier>();
            o.position = jsonToAnimatedVector3(jo["position"], s);
            o.rotation = jsonToAnimatedVector3(jo["rotation"], s);
            o.scale = jsonToAnimatedVector3(jo["scale"], s);

            o.name = jo["name"].get<std::string>();
            o.material = jo["material"].get<std::int32_t>();

            o.prim_type = jo["prim_type"];
            for (int n = 0; n < jo["values"].size(); n++) {
                o.values[n] = jsonToAnimatedFloat(jo["values"][n], s);
            }
            for (int n = 0; n < jo["modifiers"].size(); n++) {
                json jm = jo["modifiers"][n];
                SceneModifier m;
                m.modifier = jm["modifier"];
                m.attribute0 = jsonToAnimatedFloat(jm["attribute0"], s);
                m.attribute1 = jsonToAnimatedFloat(jm["attribute1"], s);
                m.attribute2 = jsonToAnimatedFloat(jm["attribute2"], s);
                m.attribute3 = jsonToAnimatedFloat(jm["attribute3"], s);
                m.attribute4 = jsonToAnimatedFloat(jm["attribute4"], s);

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
            g.prim_attribute = jsonToAnimatedFloat(jg["prim_attribute"], s);
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

        for (int i = 0; i < j["globals"].size(); i++) {
            SceneGlobal g;

            g.name = j["globals"][i]["name"];
            g.f = jsonToAnimatedFloat(j["globals"][i]["f"], s);

            s.globals.push_back(g);
        }

        

        return s;
    }

    RMImGui::ImGuiData convertScene(Scene scene) {
        RMImGui::ImGuiData d;

        for (int i = 0; i < scene.globals.size(); i++) {
            RMImGui::GlobalVariable g;
            g.name = scene.globals[i].name;
            g.f = scene.globals[i].f;

            d.globals.push_back(g);
        }

        auto defM = RMImGui::Material::DefaultMaterial();
        defM.name = "Default Material";
        d.materials.AddElement(defM);

        for (int i = 0; i < scene.materials.size(); i++) {
            RMImGui::Material m;
            m.name = scene.materials[i].name;
            m.active = true;
            m.albedo = scene.materials[i].albedo;
            m.roughness = scene.materials[i].roughness;
            m.metallic = scene.materials[i].metallic;
            m.transmission = scene.materials[i].transmission;
            m.ior = scene.materials[i].ior;

            d.materials.AddElement(m);
        }

        for (int i = 0; i < scene.lights.size(); i++) {
            RMImGui::Light l;
            l.name = scene.lights[i].name;
            l.attribute0 = scene.lights[i].attribute0;
            l.attribute1 = scene.lights[i].attribute1;
            l.attribute2 = scene.lights[i].attribute2;
            l.color = scene.lights[i].color;
            l.intensity = scene.lights[i].intensity;
            l.type = scene.lights[i].type;
            d.lights.AddElement(l);
        }

        for (int i = 0; i < scene.scripts.size(); i++) {
            RMImGui::ScriptData s = RMImGui::ScriptData(scene.scripts[i].name);
            s.script = scene.scripts[i].script;
            d.scripts.push_back(s);
        }

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

        d.cam_pos = scene.cam_pos;
        d.cam_py = scene.cam_py;
        d.cam_data = scene.cam_data;
        d.samples = scene.samples;

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

            p.transformation.position = scene.objects[i].position;
            p.transformation.rotation = scene.objects[i].rotation;
            p.transformation.scale = scene.objects[i].scale;
            Primitive::updateTransformation(&p);
            
            p.name = scene.objects[i].name;
            p.material = scene.objects[i].material;

            p.prim_type = scene.objects[i].prim_type;
            
            d.primitives[i] = p;
        }
        return d;
    }
}