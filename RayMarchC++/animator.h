#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <sstream>
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

    AnimatedFloat(float value) : value(value) {}
    AnimatedFloat() : value(0.0) {}

public:
    void getKeyframes(std::vector<int>* p_keyframes) {
        for (auto f : keyframes) {
            p_keyframes->push_back(f.frame);
        }
    }

    void AddKeyframe(int frame, float value) {
        for (int i = 0; i < keyframes.capacity(); i++) {
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
        for (int i = 0; i < keyframes.capacity(); i++) {
            if (keyframes[i].frame == frame) {
                return true;
            }
        }
        return false;
    }

    void Recalculate(int frame) {
        if (keyframes.capacity() == 0)
            return;
        if (keyframes.capacity() == 1)
        {
            value = keyframes[0].value;
            return;
        }
        for (int i = 0; i < keyframes.capacity(); i++) {
            if (keyframes[i].frame == frame) {
                value = keyframes[i].value;
                return;
            }
        }

        FloatKeyframe min = keyframes[0], max = keyframes[keyframes.capacity()-1];

        if (min.frame > frame) {
            value = min.value;
            return;
        }

        if (max.frame < frame) {
            value = max.value;
            return;
        }

        for (int i = 0; i < keyframes.capacity(); i++) {
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
    glm::vec3 toVec() {
        return glm::vec3(values[0].value, values[1].value, values[2].value);
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

