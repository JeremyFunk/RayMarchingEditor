#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "primitive.h"
namespace Shader {
	struct GroupModifier {
		GLuint modifier;
		GLuint prim0;
		GLuint prim1;
		GLuint prim2;
		GLuint prim3;
		GLuint primAttribute;
	};

	struct Modifiers {
		GLuint modifier;
		GLuint modifierAttribute0;
		GLuint modifierAttribute1;
		GLuint modifierAttribute2;
		GLuint modifierAttribute3;
		GLuint modifierAttribute4;
	};

	struct PrimitiveUniforms {
		GLuint prim_type;
		GLuint transformation;
		GLuint position;
		GLuint attribute0;
		GLuint attribute1;
		GLuint attribute2;
		GLuint attribute3;
		GLuint attribute4;
		GLuint attribute5;
		GLuint attribute6;
		GLuint attribute7;
		GLuint attribute8;
		GLuint attribute9;
		Modifiers modifiers[3];
	};

	struct ShaderUniforms {
		GLuint camera_pos;
		GLuint camera_rot;
		GLuint u_resolution;
		GLuint shading_mode;
		GLuint render_cam;
		GLuint u_prim_count;
		GLuint camera_pos_render;
		GLuint camera_dir_render;
		PrimitiveUniforms primitives[20];
		GroupModifier u_group_modifier[2];
	};

	struct ComputeShaderFragmentUniforms {
		GLuint u_texture;
		GLuint samples, total_samples;
	};

	struct ComputeShaderUniforms {
		GLuint offsetX, offsetY, t, samples, total_samples;
		GLuint camera_pos_render;
		GLuint camera_dir_render;
		PrimitiveUniforms primitives[20];
		GroupModifier u_group_modifier[2];
		GLuint camera_pos;
		GLuint camera_rot;
		GLuint u_resolution;
		GLuint shading_mode;
		GLuint render_cam;
		GLuint u_prim_count;
	};

	void PrepareComputeShader(const ComputeShaderUniforms uniforms, const Primitive::ShaderPrimitive primitives[], const Primitive::ShaderGroupPrimitive modifiers[], float offsetX, float offsetY, float t, int total_samples, int samples);
	GLuint LoadComputeShader(const char* compute_file_path);

	ShaderUniforms LoadUniforms(const GLuint program);
	ComputeShaderUniforms LoadComputeShaderUniforms(const GLuint program);
	GLuint LoadUniform(const GLuint program, const char* uniform);
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	void PrepareShader(ShaderUniforms uniforms, int samples, int total_samples);
}