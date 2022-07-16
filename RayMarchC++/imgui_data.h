#pragma once
#include "primitive.h"
#include <boost/array.hpp>
#include "LuaContext.hpp"
namespace RMImGui {

    enum class TimelineMode {
        Play, Reverse, Pause
    };

    enum class TimelineDisplay {
        Overview, Detailed,
    };

    struct TexturesTimeline {
        int play, pause, stop, next, previous, reverse_play;
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
                    mode = TimelineMode::Pause;
                    frame = max_frame;
                }
            }
            if (mode == TimelineMode::Reverse) {
                time += dt;
                while (time > 1.0f / fps) {
                    frame -= 1;
                    time -= 1.0f / fps;
                }

                if (frame <= min_frame) {
                    mode = TimelineMode::Pause;
                    frame = min_frame;
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

    enum class DragStart {
        None, TopBar, Timeline, BezierPoint, BezierTimeline, KeyframeBar, KeyframeBarFrame
    };

    struct BezierAnimationWindow {
        AnimatedFloat* f;
        std::string name;
        float x_offset, y_offset, size_x, size_y;
        BezierAnimationWindow(AnimatedFloat* f, std::string name): f(f), name(name) {
            x_offset = 0;
            y_offset = 0;
            size_x = 1;
            size_y = 1;
        }
        friend bool operator==(const BezierAnimationWindow& a1, const BezierAnimationWindow& a2) {
            return a1.f == a2.f;
        }
    };

    struct ScriptWindow {
        AnimatedFloat* f;
        int index;
        std::string name;

        ScriptWindow(AnimatedFloat* f, int index, std::string name) : f(f), index(index), name(name) {

        }
    };

    struct ScriptData {
        boost::array<char, SCRIPT_SIZE> script = boost::array<char, SCRIPT_SIZE>();

        float evaluate(float t) {
            try {
                LuaContext lua;
                std::string data(script.begin(), script.end());
                lua.executeCode(data);
                //evaluate = function(t) return t * 2.0 end
                const auto eval = lua.readVariable<std::function<float(float)>>("evaluate");
                std::cout << eval(t) << std::endl;
                return 0.0;
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
        std::vector<BezierAnimationWindow> bezier_animation_windows = std::vector<BezierAnimationWindow>();
        DragStart drag;
        int dragId, dragSubId, dragSubSubId;
        float dragData;
        Textures textures;
        TimelineData timeline;
        std::vector<ScriptData> scripts = std::vector<ScriptData>();
        std::vector<ScriptWindow> open_scripts = std::vector<ScriptWindow>();

        void addBezier(AnimatedFloat* f, std::string name) {
            if (f->keyframes.size() < 2)
                return;

            bool open = false;
            for (int i = 0; i < bezier_animation_windows.size(); i++) {
                if (bezier_animation_windows[i].f == f) {
                    open = true;
                }
            }
            if (!open) {
                bezier_animation_windows.push_back(BezierAnimationWindow(f, name));
            }
        }

        int addScript(AnimatedFloat* f) {
            ScriptData d;
            scripts.push_back(d);
            return scripts.size() - 1;
        }

        void openScriptWindow(AnimatedFloat* f, std::string name) {
            bool open = false;
            for (int i = 0; i < open_scripts.size(); i++) {
                if (open_scripts[i].f == f) {
                    open = true;
                }
            }
            if (!open) {
                open_scripts.push_back(RMImGui::ScriptWindow(f, f->script, name));
            }
        }

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
            for (int i = 0; i < COUNT_PRIMITIVE; i++) {
                primitives[i].animate(frame);
            }
            for (int i = 0; i < COUNT_GROUP_MODIFIER - 1; i++) {
                groupPrimitives[i].animate(frame);
            }
            cam_pos.Recalculate(frame);
            cam_py.Recalculate(frame);
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