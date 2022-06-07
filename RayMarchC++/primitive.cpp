
#include "helpers.h"
#include <glm/glm.hpp>
#include <iostream>
#include "primitive.h"

namespace Primitive {
	void updateTransformation(ShaderPrimitive* primitive) {
		(*primitive).transformation.matrix = transformationMatrix(primitive->transformation.rotation, primitive->transformation.scale);
	}

	ShaderGroupPrimitive opSubtraction(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = 2;

		return p;
	}
	ShaderGroupPrimitive opUnion(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = 1;

		return p;
	}
	ShaderGroupPrimitive opIntersection(int p1, int p2) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.modifier = 3;

		return p;
	}

	ShaderGroupPrimitive opUnionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = 4;

		return p;
	}

	ShaderGroupPrimitive opSubtractionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = 5;

		return p;
	}

	ShaderGroupPrimitive opIntersectionSmooth(int p1, int p2, float factor) {
		ShaderGroupPrimitive p;
		p.prim0 = p1;
		p.prim1 = p2;
		p.prim2 = -1;
		p.prim3 = -1;
		p.primAttribute = factor;
		p.modifier = 6;

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
}
