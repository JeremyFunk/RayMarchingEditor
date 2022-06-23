#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include <string>
#include <vector>
#include "constants.h"
#include "animator.h"
#include <glm/glm.hpp> 
#include <sstream>
#include <glm/gtx/string_cast.hpp>

namespace Primitive {

	struct Selectable {
	public:
		bool isSelected() {
			return selected;
		}
		void setSelected(bool p_selected) {
			selected = p_selected;
		}
		std::vector<int> getKeyframes(){}
	private:
		bool selected = false;
	};


	enum ModifierType {
		NONE_MOD, DISTORT, TWIST, BEND, REPETITION, REPETITION_LIMITED, ROUND
	};

	enum GroupModifierType {
		NONE_GROUP = 0, UNION = 1, SUBTRACTION = 2, INTERSECTION = 3, SMOOTH_UNION = 4, SMOOTH_SUBTRACTION = 5, SMOOTH_INTERSECTION = 6
	};

	struct Transformation {
		AnimatedFloatVec3 position;
		AnimatedFloatVec3 rotation;
		AnimatedFloatVec3 scale;
		glm::mat3x3 matrix;

		void animate(int frame);
		void getKeyframes(std::vector<int>* p_keyframes) {
			position.getKeyframes(p_keyframes);
			rotation.getKeyframes(p_keyframes);
			scale.getKeyframes(p_keyframes);
		}

		std::string toString() {
			std::stringstream ss;
			ss << "{position: " + position.toString() << ", rotation: " + rotation.toString() << ", scale: " + scale.toString() << ", matrix: " << glm::to_string(matrix) << "}";
			return ss.str();
		}
	};

	struct Modifier : virtual public Selectable {
		AnimatedFloat attribute0;
		AnimatedFloat attribute1;
		AnimatedFloat attribute2;
		AnimatedFloat attribute3;
		AnimatedFloat attribute4;
		ModifierType modifier;

		void Animate(int frame) {
			attribute0.Recalculate(frame);
			attribute1.Recalculate(frame);
			attribute2.Recalculate(frame);
			attribute3.Recalculate(frame);
			attribute4.Recalculate(frame);
		}

		void getKeyframes(std::vector<int>* p_keyframes) {
			attribute0.getKeyframes(p_keyframes);
			attribute1.getKeyframes(p_keyframes);
			attribute2.getKeyframes(p_keyframes);
			attribute3.getKeyframes(p_keyframes);
			attribute4.getKeyframes(p_keyframes);
		}
	};

	struct ShaderGroupPrimitive: virtual public Selectable {
		GroupModifierType modifier;
		int prim0;
		int prim1;
		int prim2;
		int prim3;
		AnimatedFloat primAttribute;

		void getKeyframes(std::vector<int>* p_keyframes) {
			primAttribute.getKeyframes(p_keyframes);
		}

		void animate(int frame) {
			primAttribute.Recalculate(frame);
		}

		std::string name() {
			if (modifier == GroupModifierType::UNION)
				return "Union";
			if (modifier == GroupModifierType::SUBTRACTION)
				return "Subtraction";
			if (modifier == GroupModifierType::INTERSECTION)
				return "Intersection";
			if (modifier == SMOOTH_UNION)
				return "Smooth Union";
			if (modifier == SMOOTH_SUBTRACTION)
				return "Smooth Subtraction";
			if (modifier == SMOOTH_INTERSECTION)
				return "Smooth Intersection";
		}
	};

	struct ShaderPrimitive : virtual public Selectable {
		AnimatedFloat values[10];
		Modifier modifiers[COUNT_PRIMITIVE_MODIFIER];
		int mod_count = 0;
		int prim_type;
		std::string name;
		Transformation transformation;

		std::string to_string() {
			return "{prim_type: " + std::to_string(prim_type) + ", name:" + std::string(name);
		}

		void getKeyframes(std::vector<int>* p_keyframes) {
			for (int i = 0; i < 10 - 1; i++) {
				values[i].getKeyframes(p_keyframes);
			}
			transformation.getKeyframes(p_keyframes);
		}

		void animate(int frame) {
			for (int i = 0; i < 10 - 1; i++) {
				values[i].Recalculate(frame);
			}
			for (int i = 0; i < COUNT_PRIMITIVE_MODIFIER - 1; i++ ) {
				modifiers[i].Animate(frame);
			}
			transformation.animate(frame);
		}

		void removeModifier(int index) {
			modifiers[index].modifier = ModifierType::NONE_MOD;
			mod_count -= 1;
			for (int i = 0; i < COUNT_PRIMITIVE_MODIFIER - 1; i++) {
				if (modifiers[i].modifier == 0) {
					for (int j = i + 1; j < COUNT_PRIMITIVE_MODIFIER; j++) {
						if (modifiers[j].modifier != 0) {
							modifiers[i] = modifiers[j];
							modifiers[j].modifier = ModifierType::NONE_MOD;
							break;
						}
					}
				}
			}
		}

		void addDistortModifier(glm::vec3 offset, float factor, float freq) {
			modifiers[mod_count].modifier = DISTORT;
			modifiers[mod_count].attribute0 = offset.x;
			modifiers[mod_count].attribute1 = offset.y;
			modifiers[mod_count].attribute2 = offset.z;
			modifiers[mod_count].attribute3 = factor;
			modifiers[mod_count].attribute4 = freq;

			mod_count += 1;
		}
		void addTwistModifier(float power) {
			modifiers[mod_count].modifier = TWIST;
			modifiers[mod_count].attribute0 = power;

			mod_count += 1;
		}
		void addBendModifier(float power) {
			modifiers[mod_count].modifier = BEND;
			modifiers[mod_count].attribute0 = power;

			mod_count += 1;
		}
		void addRepetitionModifier(float repetition_period) {
			modifiers[mod_count].modifier = REPETITION;
			modifiers[mod_count].attribute0 = repetition_period;

			mod_count += 1;
		}
		void addRepetitionLimitedModifier(float repetition_period, glm::vec3 limiter) {
			modifiers[mod_count].modifier = REPETITION_LIMITED;
			modifiers[mod_count].attribute0 = repetition_period;
			modifiers[mod_count].attribute1 = limiter.x;
			modifiers[mod_count].attribute2 = limiter.y;
			modifiers[mod_count].attribute3 = limiter.z;

			mod_count += 1;
		}
		void addRoundModifier(float thickness) {
			modifiers[mod_count].modifier = ROUND;
			modifiers[mod_count].attribute0 = thickness;

			mod_count += 1;
		}
	};
	ShaderGroupPrimitive opIntersection(int p1, int p2);
	ShaderGroupPrimitive opUnion(int p1, int p2);
	ShaderGroupPrimitive opSubtraction(int p1, int p2);
	ShaderGroupPrimitive opIntersectionSmooth(int p1, int p2, float factor);
	ShaderGroupPrimitive opUnionSmooth(int p1, int p2, float factor);
	ShaderGroupPrimitive opSubtractionSmooth(int p1, int p2, float factor);
	void updateTransformation(ShaderPrimitive* primitive);
	ShaderPrimitive getCubePrimitive(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    ShaderPrimitive getMandelbulbPrimitive(float power, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	ShaderPrimitive getSpherePrimitive(float radius, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	ShaderPrimitive getTorusPrimitive(float radius, float ring_radius, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
}
#endif