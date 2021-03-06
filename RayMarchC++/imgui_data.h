#pragma once
#include "primitive.h"
#include <boost/array.hpp>
#include "LuaContext.hpp"
#include <chrono>
#include <vector>
namespace RMImGui {

    enum class TimelineMode {
        Play, Reverse, Pause
    };

    enum class TimelineDisplay {
        Overview, Detailed,
    };
    enum class GameEngineState {
        Start, Engine,
    };

    struct TexturesTimeline {
        int play, pause, stop, next, previous, reverse_play, loop;
    };

    struct Textures {
        TexturesTimeline timeline;
    };

    struct TimelineData {

        TimelineMode mode = TimelineMode::Pause;
        TimelineDisplay display = TimelineDisplay::Overview;
        int frame = 0;
        float time = 0.0f;
        float fps = 30.0f;
        float offset = 0;
        int min_frame = 0, max_frame = 200;
        bool loop = false;

        void update(float dt) {
            if (frame < min_frame) {
                frame = min_frame;
            }
            if (frame > max_frame) {
                frame = max_frame;
            }

            if (mode == TimelineMode::Play) {
                time += dt;
                while (time > 1.0 / fps) {
                    frame += 1;
                    time -= 1.0f / fps;
                }

                if (frame >= max_frame) {
                    if (!loop) {
                        mode = TimelineMode::Pause;
                        frame = max_frame;
                    }
                    else {
                        frame -= (max_frame - min_frame);
                    }
                }
            }
            if (mode == TimelineMode::Reverse) {
                time += dt;
                while (time > 1.0f / fps) {
                    frame -= 1;
                    time -= 1.0f / fps;
                }

                if (frame <= min_frame) {
                    if (!loop) {
                        mode = TimelineMode::Pause;
                        frame = min_frame;
                    }
                    else {
                        frame += (max_frame - min_frame);
                    }
                }
            }
        }
    };

    struct SelectedPrimitive {
        Primitive::ShaderPrimitive* primitive;
        std::vector<Primitive::Modifier*> modifiers;

        void getKeyframes(std::vector<int>* frames) {
            for (auto a : modifiers) {
                a->getKeyframes(frames);
            }
            primitive->getKeyframes(frames);
        }
    };

    struct AnimationData {
        std::vector<Primitive::ShaderGroupPrimitive*> group_primitivies;
        std::vector<SelectedPrimitive> selected_primitives;

        AnimatedFloatVec3 cam_pos;
        AnimatedFloatVec2 cam_py;

        bool camSelected = false;

        std::vector<int> getKeyframes() {
            std::vector<int> frames = std::vector<int>();
            for (auto a : group_primitivies) {
                a->getKeyframes(&frames);
            }
            for (auto a : selected_primitives) {
                a.getKeyframes(&frames);
            }
            if (camSelected) {
                cam_pos.getKeyframes(&frames);
                cam_py.getKeyframes(&frames);
            }
            return frames;
        }
    };

    struct GlobalVariable {
        std::string name; 
        AnimatedFloat f;
    };

    enum class DragStart {
        None, TopBar, Timeline, BezierPoint, BezierTimeline, KeyframeBar, KeyframeBarFrame
    };

    struct AnimationWindow {
        AnimatedFloat* f;
        std::string name;
        std::string temp_name = "";
        float x_offset, y_offset, size_x, size_y;
        AnimationWindow(AnimatedFloat* f, std::string name): f(f), name(name) {
            x_offset = 0;
            y_offset = 0;
            size_x = 1;
            size_y = 1;
        }
        friend bool operator==(const AnimationWindow& a1, const AnimationWindow& a2) {
            return a1.f == a2.f;
        }
    };

    struct ScriptData {
        boost::array<char, SCRIPT_SIZE> script = boost::array<char, SCRIPT_SIZE>();
        bool recompile;
        std::string name;
        std::function<float(float)> eval;

        ScriptData(std::string name) {
            std::string default_val = "evaluate = function(t)\n  return t\nend";
            for (int i = 0; i < default_val.size(); i++) {
                script[i] = default_val[i];
            }
            this->name = name;
        }

        void rename(std::string name) {

        }

        void compile() {
            recompile = true;
        }

        float evaluate(float t) {
            try {
                //auto start = std::chrono::high_resolution_clock::now();
                //float result = eval(t);
                //auto finish = std::chrono::high_resolution_clock::now();

                //std::cout << result << std::endl;
                //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "ns\n";
                return eval(t);
            }
            catch (const std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    };

    struct ImGuiData {
        int* shading_mode;
        Primitive::ShaderPrimitive primitives[COUNT_PRIMITIVE];
        Primitive::ShaderGroupPrimitive groupPrimitives[COUNT_GROUP_MODIFIER];
        glm::vec3 cam_rot;
        AnimatedFloatVec3 cam_pos;
        AnimatedFloatVec2 cam_py;
        bool reposition_cam = false, camSelected = false;
        DragStart drag;
        int dragId, dragSubId, dragSubSubId;
        std::string project_path;
        float dragData;
        bool recalculate = false;
        Textures textures;
        TimelineData timeline;
        GameEngineState engine_state = GameEngineState::Start;
        std::vector<AnimationWindow> windows = std::vector<AnimationWindow>();
        std::vector<ScriptData> scripts = std::vector<ScriptData>();
        std::vector<GlobalVariable> globals = std::vector<GlobalVariable>();
        //std::vector<ScriptWindow> open_scripts = std::vector<ScriptWindow>();

        int addWindow(AnimatedFloat* f, std::string name) {
            for (int i = 0; i < windows.size(); i++) {
                if (windows[i].f == f) {
                    return i;
                }
            }
            windows.push_back(AnimationWindow(f, name));
            return windows.size() - 1;
        }

        std::vector<const char*> getScriptNames() {
            auto vec = std::vector<const char*>();
            for (auto& s : scripts) {
                vec.push_back(s.name.c_str());
            }
            return vec;
        }

        //void loadScripts()

        int addScript(AnimatedFloat* f, std::string& filename) {
            int count = 1;
            std::string assigned_name = std::string(filename);
            while(true) {
                bool found = false;
                for (auto &s : scripts) {
                    if (s.name == assigned_name) {
                        found = true;
                    }
                }
                if (!found) {
                    break;
                }
                assigned_name = filename + std::to_string(count);
                count += 1;
            }

            filename = assigned_name;

            ScriptData d = ScriptData(assigned_name);
            scripts.push_back(d);
            return scripts.size() - 1;
        }

        /*void openScriptWindow(AnimatedFloat* f, std::string name) {
            bool open = false;
            for (int i = 0; i < open_scripts.size(); i++) {
                if (open_scripts[i].f == f) {
                    open = true;
                }
            }
            if (!open) {
                open_scripts.push_back(RMImGui::ScriptWindow(f, f->script, name));
            }
        }*/

        AnimationData selectedAnimationData() {
            AnimationData d;
            d.group_primitivies = std::vector<Primitive::ShaderGroupPrimitive*>();
            d.selected_primitives = std::vector<SelectedPrimitive>();
            d.camSelected = camSelected;
            for (int i = 0; i < COUNT_PRIMITIVE; i++) {
                if (primitives[i].isSelected()) {
                    SelectedPrimitive s;
                    s.modifiers = std::vector<Primitive::Modifier*>();
                    s.primitive = &primitives[i];

                    for (int j = 0; j < COUNT_PRIMITIVE_MODIFIER; j++) {
                        if (primitives[i].modifiers[j].isSelected()) {
                            s.modifiers.push_back(&primitives[i].modifiers[j]);
                        }
                    }
                    d.selected_primitives.push_back(s);
                }
            }

            for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
                if (groupPrimitives[i].isSelected()) {
                    d.group_primitivies.push_back(&groupPrimitives[i]);
                }
            }

            if (camSelected) {
                d.cam_pos = cam_pos;
                d.cam_py = cam_py;
            }
            return d;
        }

        void animate(int frame) {
            auto vec = std::vector<AnimatedFloat*>();
            for (int i = 0; i < COUNT_PRIMITIVE; i++) {
                primitives[i].animate(frame, &vec);
            }
            for (int i = 0; i < COUNT_GROUP_MODIFIER - 1; i++) {
                groupPrimitives[i].animate(frame, &vec);
            }
            cam_pos.Recalculate(frame, &vec);
            cam_py.Recalculate(frame, &vec);

            for (auto p : vec) {
                if (p->script != -1) {
                    p->value = scripts[p->script].evaluate(frame / timeline.fps);
                }
            }
        }

        void addPrimitive(Primitive::ShaderPrimitive prim) {
            for (int i = 0; i < COUNT_PRIMITIVE; i++) {
                if (primitives[i].prim_type == 0) {
                    primitives[i] = prim;
                    break;
                }
            }
        }


        void removeGroupModifier(int index) {
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
}