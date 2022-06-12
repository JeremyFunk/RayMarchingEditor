#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#include "glm/glm.hpp"
#include <string>
#include "constants.h"
namespace Primitive {
	enum ModifierType {
		NONE_MOD, DISTORT, TWIST, BEND, REPETITION, REPETITION_LIMITED, ROUND
	};

	enum GroupModifierType {
		NONE_GROUP = 0, UNION = 1, SUBTRACTION = 2, INTERSECTION = 3, SMOOTH_UNION = 4, SMOOTH_SUBTRACTION = 5, SMOOTH_INTERSECTION = 6
	};

	struct Transformation {
		glm::vec3 position;
		glm::vec3 rotation;
		glm::vec3 scale;
		glm::mat3x3 matrix;
	};

	struct Modifier {
		float attribute0;
		float attribute1;
		float attribute2;
		float attribute3;
		float attribute4;
		ModifierType modifier;
	};

	struct ShaderGroupPrimitive {
		GroupModifierType modifier;
		int prim0;
		int prim1;
		int prim2;
		int prim3;
		float primAttribute;

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

	struct ShaderPrimitive {
		float values[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		Modifier modifiers[COUNT_PRIMITIVE_MODIFIER];
		int mod_count = 0;
		int prim_type;
		std::string name;
		Transformation transformation;

		std::string to_string() {
			return "{prim_type: " + std::to_string(prim_type) + ", name:" + std::string(name);
		}

		void remove_modifier(int index) {
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

		void add_distort_modifier(glm::vec3 offset, float factor, float freq) {
			modifiers[mod_count].modifier = DISTORT;
			modifiers[mod_count].attribute0 = offset.x;
			modifiers[mod_count].attribute1 = offset.y;
			modifiers[mod_count].attribute2 = offset.z;
			modifiers[mod_count].attribute3 = factor;
			modifiers[mod_count].attribute4 = freq;

			mod_count += 1;
		}
		void add_twist_modifier(float power) {
			modifiers[mod_count].modifier = TWIST;
			modifiers[mod_count].attribute0 = power;

			mod_count += 1;
		}
		void add_bend_modifier(float power) {
			modifiers[mod_count].modifier = BEND;
			modifiers[mod_count].attribute0 = power;

			mod_count += 1;
		}
		void add_repetition_modifier(float repetition_period) {
			modifiers[mod_count].modifier = REPETITION;
			modifiers[mod_count].attribute0 = repetition_period;

			mod_count += 1;
		}
		void add_repetition_limited_modifier(float repetition_period, glm::vec3 limiter) {
			modifiers[mod_count].modifier = REPETITION_LIMITED;
			modifiers[mod_count].attribute0 = repetition_period;
			modifiers[mod_count].attribute1 = limiter.x;
			modifiers[mod_count].attribute2 = limiter.y;
			modifiers[mod_count].attribute3 = limiter.z;

			mod_count += 1;
		}
		void add_round_modifier(float thickness) {
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