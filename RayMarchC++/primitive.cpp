
#include "helpers.h"
#include "helpers_glm.h"
#include <glm/glm.hpp>
#include <iostream>
#include "primitive.h"

namespace Primitive {
	void Transformation::animate(int frame, std::vector<AnimatedFloat*>* vec) {
		position.Recalculate(frame, vec);
		rotation.Recalculate(frame, vec);
		scale.Recalculate(frame, vec);
		matrix = transformationMatrix(glm::vec3(rotation[0].value, rotation[1].value, rotation[2].value), glm::vec3(scale[0].value, scale[1].value, scale[2].value));
	}

	void updateTransformation(ShaderPrimitive* primitive) {
		(*primitive).transformation.matrix = transformationMatrix(glm::vec3(primitive->transformation.rotation[0].value, primitive->transformation.rotation[1].value, primitive->transformation.rotation[2].value), glm::vec3(primitive->transformation.scale[0].value, primitive->transformation.scale[1].value, primitive->transformation.scale[2].value));
	}

	ShaderGroupPrimitive opSubtraction(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = GroupModifierType::SUBTRACTION;

		return p;
	}
	ShaderGroupPrimitive opUnion(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = GroupModifierType::UNION;

		return p;
	}
	ShaderGroupPrimitive opIntersection(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = GroupModifierType::INTERSECTION;

		return p;
	}

	ShaderGroupPrimitive opUnionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = GroupModifierType::SMOOTH_UNION;

		return p;
	}

	ShaderGroupPrimitive opSubtractionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = GroupModifierType::SMOOTH_SUBTRACTION;

		return p;
	}

	ShaderGroupPrimitive opIntersectionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = GroupModifierType::SMOOTH_INTERSECTION;

		return p;
	}

	ShaderPrimitive getCubePrimitive(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		ShaderPrimitive cube;
		cube.transformation.position = position;
		cube.transformation.rotation = rotation;
		cube.transformation.scale = scale;
		cube.transformation.matrix = transformationMatrix(rotation, scale);
		cube.prim_type = 3;
		cube.values[0] = 1.0;
		cube.values[1] = 1.0;
		cube.values[2] = 1.0;
		cube.name = "Cube";
		return cube;
	}
	ShaderPrimitive getMandelbulbPrimitive(float power, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		ShaderPrimitive cube;
		cube.transformation.position = position;
		cube.transformation.rotation = rotation;
		cube.transformation.scale = scale;
		cube.transformation.matrix = transformationMatrix(rotation, scale);
		cube.prim_type = 4;
		cube.values[0] = power;
		auto name = "Mandelbulb";
		cube.name = name;

		return cube;
	}
	ShaderPrimitive getSpherePrimitive(float radius, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		ShaderPrimitive cube;
		cube.transformation.position = position;
		cube.transformation.rotation = rotation;
		cube.transformation.scale = scale;
		cube.transformation.matrix = transformationMatrix(rotation, scale);
		cube.prim_type = 1;
		cube.values[0] = radius;
		cube.name = "Sphere";

		return cube;
	}
	ShaderPrimitive getTorusPrimitive(float radius, float ring_radius, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		ShaderPrimitive cube;
		cube.transformation.position = position;
		cube.transformation.rotation = rotation;
		cube.transformation.scale = scale;
		cube.transformation.matrix = transformationMatrix(rotation, scale);
		cube.prim_type = 2;
		cube.values[0] = radius;
		cube.values[1] = ring_radius;
		cube.name = "Torus";

		return cube;
	}
	ShaderPrimitive getJuliaPrimitive(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		ShaderPrimitive cube;
		cube.transformation.position = position;
		cube.transformation.rotation = rotation;
		cube.transformation.scale = scale;
		cube.transformation.matrix = transformationMatrix(rotation, scale);
		cube.prim_type = 5;
		cube.values[0] = 0.45*cos(0.5)-0.3;
		cube.values[1] = 0.45 * cos(3.9);
		cube.values[2] = 0.45 * cos(1.4);
		cube.values[3] = 0.45 * cos(1.1);
		cube.name = "Julia";

		return cube;
	}
}
