#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <sstream>

struct KeyframeInterpolation {
    glm::vec2 p1;
    glm::vec2 p2;
};
struct FloatKeyframe {
    int frame;
    float value;
    FloatKeyframe(int frame, float value): frame(frame), value(value) {}
};
struct less_than_key
{
    inline bool operator() (const FloatKeyframe& struct1, const FloatKeyframe& struct2)
    {
        return (struct1.frame < struct2.frame);
    }
};


struct AnimatedFloat {
    float value;
    std::vector<FloatKeyframe> keyframes;
    std::vector<KeyframeInterpolation> interpolation;

    AnimatedFloat(float value) : value(value) {
        keyframes = std::vector<FloatKeyframe>();
    }
    AnimatedFloat() : value(0.0) {
        keyframes = std::vector<FloatKeyframe>();
    }

public:
    int firstFrame() {
        if (keyframes.size() == 0) {
            return 0;
        }
        return keyframes[0].frame;
    }

    int CanFrameMove(int frame_count)
    {
        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].frame + frame_count < 0) {
                return -keyframes[i].frame;
            }
        }
        return frame_count;
    }
    void FrameMove(int frame_count)
    {
        for (int i = 0; i < keyframes.size(); i++) {
            keyframes[i].frame += frame_count;
        }
    }

    int lastFrame() {
        if (keyframes.size() == 0) {
            return 0;
        }
        return keyframes[keyframes.size() - 1].frame;
    }

    void getKeyframes(std::vector<int>* p_keyframes) {
        for (auto f : keyframes) {
            p_keyframes->push_back(f.frame);
        }
    }
    bool ContainsKeyframes() {
        return !keyframes.empty();
    }

    void AddKeyframe(int frame, float value) {
        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].frame == frame) {
                keyframes[i].value = value;
                return;
            }
        }
        keyframes.push_back(FloatKeyframe(frame, value));

        std::sort(keyframes.begin(), keyframes.end(), [](FloatKeyframe a, FloatKeyframe b) {
            return a.frame < b.frame;
        });
    }

    bool ContainsKeyframe(int frame) {
        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].frame == frame) {
                return true;
            }
        }
        return false;
    }

    void Recalculate(int frame) {
        if (keyframes.size() == 0)
            return;
        if (keyframes.size() == 1)
        {
            value = keyframes[0].value;
            return;
        }
        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].frame == frame) {
                value = keyframes[i].value;
                return;
            }
        }

        FloatKeyframe min = keyframes[0], max = keyframes[keyframes.size()-1];

        if (min.frame > frame) {
            value = min.value;
            return;
        }

        if (max.frame < frame) {
            value = max.value;
            return;
        }

        for (int i = 0; i < keyframes.size(); i++) {
            if (keyframes[i].frame < frame) {
                min = keyframes[i];
            }
            else {
                max = keyframes[i];
                break;
            }
        }

        float frameDiff = max.frame - min.frame;
        float factor = (frame - min.frame) / frameDiff;
        value = min.value * (1.0f - factor) + max.value * factor;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "{value: " << value << "}";
        return ss.str();
    }
};


struct AnimatedFloatVec3 {
    AnimatedFloat values[3];
    AnimatedFloat operator [] (int i) const { return values[i]; }
    AnimatedFloat& operator [] (int i) { return values[i]; }

    void Recalculate(int frame) {
        values[0].Recalculate(frame);
        values[1].Recalculate(frame);
        values[2].Recalculate(frame);
    }
    void getKeyframes(std::vector<int>* p_keyframes) {
        values[0].getKeyframes(p_keyframes);
        values[1].getKeyframes(p_keyframes);
        values[2].getKeyframes(p_keyframes);
    }
    bool containsKeyframes() {
        return values[0].ContainsKeyframes() || values[1].ContainsKeyframes() || values[2].ContainsKeyframes();
    }
    glm::vec3 toVec() {
        return glm::vec3(values[0].value, values[1].value, values[2].value);
    }

    int firstFrame() {
        int min = 10000;
        if (values[0].ContainsKeyframes()) {
            min = values[0].firstFrame();
        }
        if (values[1].ContainsKeyframes()) {
            min = std::min(min, values[1].firstFrame());
        }
        if (values[2].ContainsKeyframes()) {
            min = std::min(min, values[2].firstFrame());
        }
        return min;
    }

    int lastFrame() {
        int max = -1;
        if (values[0].ContainsKeyframes()) {
            max = values[0].lastFrame();
        }
        if (values[1].ContainsKeyframes()) {
            max = std::max(max, values[1].lastFrame());
        }
        if (values[2].ContainsKeyframes()) {
            max = std::max(max, values[2].lastFrame());
        }
        return max;
    }
    
    AnimatedFloatVec3() {

    }
    AnimatedFloatVec3(glm::vec3 vec) {
        values[0] = vec.x;
        values[1] = vec.y;
        values[2] = vec.z;
    }
    AnimatedFloatVec3(AnimatedFloat f1, AnimatedFloat f2, AnimatedFloat f3) {
        values[0] = f1;
        values[1] = f2;
        values[2] = f3;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "{x: " << values[0].toString() << "y: " << values[1].toString() << "z: " << values[2].toString() << "}";
        return ss.str();
    }
};



struct AnimatedFloatVec2 {
    AnimatedFloat values[2];
    AnimatedFloat operator [] (int i) const { return values[i]; }
    AnimatedFloat& operator [] (int i) { return values[i]; }

    void Recalculate(int frame) {
        values[0].Recalculate(frame);
        values[1].Recalculate(frame);
    }
    void getKeyframes(std::vector<int>* p_keyframes) {
        values[0].getKeyframes(p_keyframes);
        values[1].getKeyframes(p_keyframes);
    }

    AnimatedFloatVec2() {

    }
    AnimatedFloatVec2(glm::vec2 vec) {
        values[0] = vec.x;
        values[1] = vec.y;
    }
    AnimatedFloatVec2(AnimatedFloat f1, AnimatedFloat f2) {
        values[0] = f1;
        values[1] = f2;
    }

    std::string toString() {
        std::stringstream ss;
        ss << "{x: " << values[0].toString() << "y: " << values[1].toString() << "}";
        return ss.str();
    }
};

