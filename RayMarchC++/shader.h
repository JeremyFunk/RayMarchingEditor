#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "primitive.h"
#include "imgui_data.h"
namespace Shader {

	struct SSBOModifier {
		GLfloat modifierAttribute0, modifierAttribute1, modifierAttribute2, modifierAttribute3, modifierAttribute4;
		GLint modifier;
	};

	struct SSBOPrimitive {
		GLfloat attribute0;
		GLfloat attribute1;
		GLfloat attribute2;
		GLfloat attribute3;
		GLfloat attribute4;
		GLfloat attribute5;
		SSBOModifier modifiers[3];
		GLfloat m00;
		GLfloat m01;
		GLfloat m02;
		GLfloat m03;
		GLfloat m10;
		GLfloat m11;
		GLfloat m12;
		GLfloat m13;
		GLfloat m20;
		GLfloat m21;
		GLfloat m22;
		GLfloat m23;
		GLfloat position[3];
		GLint prim_type;
	};

	struct SSBOGroupModifier {
		GLint prim0;
		GLint prim1;
		GLint prim2;
		GLint prim3;
		GLfloat primAttribute;
		GLint modifier;
	};

	struct SSBOLight {
		GLfloat attribute0;
		GLfloat attribute1;
		GLfloat attribute2;
		GLfloat colorR, colorG, colorB;
		GLint type;
	};

	struct ShaderUniforms {
		GLuint camera_pos;
		GLuint camera_rot;
		GLuint u_resolution;
		GLuint shading_mode;
		GLuint render_cam;
		GLuint u_prim_count, u_group_count;
		GLuint camera_pos_render;
		GLuint camera_dir_render;
		GLuint focal_length;
	};

	struct ComputeShaderFragmentUniforms {
		GLuint u_texture;
		GLuint u_resolution;
		GLuint samples, total_samples;
	};

	struct ComputeShaderUniforms {
		GLuint offsetX, offsetY, t, number_samples, total_samples, current_sample;
		GLuint camera_pos_render; 
		GLuint camera_dir_render; 
		GLuint camera_pos;
		GLuint camera_rot;
		GLuint u_resolution;
		GLuint u_prim_count, u_group_count;
		GLuint apeture_size, focus_dist, focal_length;
	};

	GLuint LoadUniform(const GLuint program, const char* uniform);

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	GLuint LoadComputeShader(const char* compute_file_path);

	ShaderUniforms LoadUniforms(const GLuint program);
	ComputeShaderUniforms LoadComputeShaderUniforms(const GLuint program);
	ComputeShaderFragmentUniforms LoadComputeShaderFragmentUniforms(const GLuint program);

	void PrepareShader(int prim_count, int mod_count, float focal_length, ShaderUniforms uniforms);
	void PrepareComputeShaderFragment(ComputeShaderFragmentUniforms uniforms, int samples, int total_samples);
	void PrepareComputeShader(const ComputeShaderUniforms uniforms, int prim_count, int mod_count, float offsetX, float offsetY, float t, int total_samples, int samples, int current_sample, RMImGui::CameraData cam_data);
}