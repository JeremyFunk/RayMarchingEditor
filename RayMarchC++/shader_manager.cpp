#pragma once
#include "fglsl.h"
#include <GL/glew.h>
#include "shader.h"
#include "RMIO.h"
namespace ShaderManager {
	struct ShaderData {
		// Full VF Shader
		GLuint realtimeVFShader;
		// Quad shader to display compute output
		GLuint computeVFShader;
		GLuint computeCS;
		GLuint computeResetCS;
		Shader::ShaderUniforms realtimeUniforms;
		Shader::ComputeShaderUniforms computeUniforms;
		Shader::ComputeShaderFragmentUniforms computeVFUniforms;
		unsigned int ssbo_prims, ssbo_gm, ssbo_lights, ssbo_materials;

		// For final quad render
		GLuint texbuffer;
		GLuint vertexbuffer;
	};
	ShaderData shaderData;

	bool tiling = false;
	int TILE_WIDTH = GetWindowData().screenWidth, TILE_HEIGHT = GetWindowData().screenHeight;
	int SAMPLES_PER_ITER = 1;
	int render_sample = 0;
	int render_tile_x = 0;
	int render_tile_y = 0;


	GLint recompileShader(Shader::ComputeShaderUniforms& uniforms) {
		auto fglsl = FGLSL::LoadFGLSL("cs.comp");
		fglsl.SetCondition("LIGHTS", true);
		fglsl.SetCondition("LIGHTS", false);
		fglsl.SetValue("MAX_PRIM_COUNT", "10");

		auto shaders = FGLSL::GenerateShaders(fglsl);
		shaders.SetCondition("LIGHTS", true);
		shaders.SetValue("MAX_PRIM_COUNT", "10");
		auto shader_code = shaders.GetShader();

		RMIO::Save("cs.compiled.comp", shader_code->code);
		auto computeCS = Shader::LoadComputeShader(shader_code->code);
		uniforms = Shader::LoadComputeShaderUniforms(computeCS);
		return computeCS;
	}

	void LoadShaders() {
		shaderData.realtimeVFShader = Shader::LoadShaders("vertex.vs", "fragment.fs");
		shaderData.computeVFShader = Shader::LoadShaders("vertex.vs", "cs.fs");
		Shader::ComputeShaderUniforms computeUniforms;
		shaderData.computeCS = recompileShader(shaderData.computeUniforms);
		shaderData.computeResetCS = Shader::LoadComputeShaderByPath("reset.comp");

		shaderData.realtimeUniforms = Shader::LoadUniforms(shaderData.realtimeVFShader);
		shaderData.computeVFUniforms = Shader::LoadComputeShaderFragmentUniforms(shaderData.computeVFShader);

		glGenBuffers(1, &shaderData.ssbo_prims);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_prims);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOPrimitive) * COUNT_PRIMITIVE, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shaderData.ssbo_prims);

		glGenBuffers(1, &shaderData.ssbo_gm);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_gm);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOGroupModifier) * COUNT_GROUP_MODIFIER, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, shaderData.ssbo_gm);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &shaderData.ssbo_lights);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_lights);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOLight) * COUNT_LIGHTS, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, shaderData.ssbo_lights);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glGenBuffers(1, &shaderData.ssbo_materials);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_materials);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Shader::SSBOMaterial) * COUNT_MATERIALS, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, shaderData.ssbo_materials);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



		static const GLfloat vertices[] = {
			 -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
		};
		static const GLfloat tex_coords[] = {
			 0.0f,  1.0f,
			 1.0f,  1.0f,
			 1.0f, 0.0f,
			 0.0f, 0.0f,
		};

		glGenBuffers(1, &shaderData.vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, shaderData.vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glGenBuffers(1, &shaderData.texbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, shaderData.texbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
	}

	void Recompile() {
		glDeleteShader(shaderData.computeCS);
		shaderData.computeCS = recompileShader(shaderData.computeUniforms);
	}

	void ReloadSSBOs(RMImGui::ImGuiData& data) {
		Shader::SSBOLight lights[COUNT_LIGHTS];
		for (int i = 0; i < IM_ARRAYSIZE(data.lights.values); i++) {
			if (!(data.lights[i].type == 1 || data.lights[i].type == 2)) {
				lights[i].attribute0 = 0.0;
				lights[i].attribute1 = 0.0;
				lights[i].attribute2 = 0.0;
				lights[i].colorR = 0.0;
				lights[i].colorG = 0.0;
				lights[i].colorB = 0.0;
				lights[i].type = 0;
				continue;
			}

			if (data.lights[i].type == 2) {
				auto temp_vec = glm::normalize(glm::vec3(data.lights[i].attribute0.value, data.lights[i].attribute1.value, data.lights[i].attribute2.value));
				lights[i].attribute0 = temp_vec.x;
				lights[i].attribute1 = temp_vec.y;
				lights[i].attribute2 = temp_vec.z;
			}
			else {
				lights[i].attribute0 = data.lights[i].attribute0.value;
				lights[i].attribute1 = data.lights[i].attribute1.value;
				lights[i].attribute2 = data.lights[i].attribute2.value;
			}
			lights[i].colorR = data.lights[i].color.values[0].value * data.lights[i].intensity.value;
			lights[i].colorG = data.lights[i].color.values[1].value * data.lights[i].intensity.value;
			lights[i].colorB = data.lights[i].color.values[2].value * data.lights[i].intensity.value;
			lights[i].type = data.lights[i].type;
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_lights);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(lights), &lights[0]);


		Shader::SSBOMaterial materials[COUNT_MATERIALS];
		for (int i = 0; i < IM_ARRAYSIZE(data.materials.values); i++) {
			materials[i].albedoR = data.materials[i].albedo[0].value;
			materials[i].albedoG = data.materials[i].albedo[1].value;
			materials[i].albedoB = data.materials[i].albedo[2].value;

			materials[i].transmission = data.materials[i].transmission.value;
			materials[i].ior = data.materials[i].ior.value;
			materials[i].roughness = data.materials[i].roughness.value;
			materials[i].metallic = data.materials[i].metallic.value;
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_materials);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(materials), &materials[0]);


		Shader::SSBOPrimitive prims[COUNT_PRIMITIVE];
		int i;
		for (i = 0; i < IM_ARRAYSIZE(data.primitives); i++) {
			prims[i].material = data.primitives[i].material;

			prims[i].attribute0 = data.primitives[i].values[0].value;
			prims[i].attribute1 = data.primitives[i].values[1].value;
			prims[i].attribute2 = data.primitives[i].values[2].value;
			prims[i].attribute3 = data.primitives[i].values[3].value;
			prims[i].attribute4 = data.primitives[i].values[4].value;

			prims[i].m00 = data.primitives[i].transformation.matrix[0][0];
			prims[i].m01 = data.primitives[i].transformation.matrix[0][1];
			prims[i].m02 = data.primitives[i].transformation.matrix[0][2];
			prims[i].m10 = data.primitives[i].transformation.matrix[1][0];
			prims[i].m11 = data.primitives[i].transformation.matrix[1][1];
			prims[i].m12 = data.primitives[i].transformation.matrix[1][2];
			prims[i].m20 = data.primitives[i].transformation.matrix[2][0];
			prims[i].m21 = data.primitives[i].transformation.matrix[2][1];
			prims[i].m22 = data.primitives[i].transformation.matrix[2][2];

			prims[i].position[0] = data.primitives[i].transformation.position[0].value;
			prims[i].position[1] = data.primitives[i].transformation.position[1].value;
			prims[i].position[2] = data.primitives[i].transformation.position[2].value;
			prims[i].prim_type = data.primitives[i].prim_type;
			for (int j = 0; j < IM_ARRAYSIZE(prims[i].modifiers); j++)
			{
				prims[i].modifiers[j].modifier = data.primitives[i].modifiers[j].modifier;
				prims[i].modifiers[j].modifierAttribute0 = data.primitives[i].modifiers[j].attribute0.value;
				prims[i].modifiers[j].modifierAttribute1 = data.primitives[i].modifiers[j].attribute1.value;
				prims[i].modifiers[j].modifierAttribute2 = data.primitives[i].modifiers[j].attribute2.value;
				prims[i].modifiers[j].modifierAttribute3 = data.primitives[i].modifiers[j].attribute3.value;
				prims[i].modifiers[j].modifierAttribute4 = data.primitives[i].modifiers[j].attribute4.value;
			}
		}
		for (i; i < COUNT_PRIMITIVE; i++) {
			prims[i].prim_type = 0;
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_prims);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(prims), &prims[0]);

		Shader::SSBOGroupModifier groups[COUNT_GROUP_MODIFIER];
		for (i = 0; i < IM_ARRAYSIZE(data.groupPrimitives); i++) {
			groups[i].modifier = data.groupPrimitives[i].modifier;
			groups[i].prim0 = data.groupPrimitives[i].prim0;
			groups[i].prim1 = data.groupPrimitives[i].prim1;
			groups[i].prim2 = data.groupPrimitives[i].prim2;
			groups[i].prim3 = data.groupPrimitives[i].prim3;
			groups[i].primAttribute = data.groupPrimitives[i].primAttribute.value;
		}
		for (i; i < COUNT_GROUP_MODIFIER; i++) {
			groups[i].modifier = 0;
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, shaderData.ssbo_gm);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(groups), &groups[0]);
	}

	void ResetFramebuffers() {
		glUseProgram(shaderData.computeResetCS);
		glDispatchCompute(
			ceil((unsigned int)std::min(TILE_WIDTH, GetWindowData().screenWidth - render_tile_x * TILE_WIDTH) / 8),
			ceil((unsigned int)std::min(TILE_HEIGHT, GetWindowData().screenHeight - render_tile_y * TILE_HEIGHT) / 4),
			1
		);

		// make sure writing to image has finished before read
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		render_sample = 0;
		render_tile_x = render_tile_y = 0;
	}

	void RenderCS(RMImGui::ImGuiData& data, CameraData& camera, glm::mat4 view, float t) {
		glUseProgram(shaderData.computeCS);

		glUniformMatrix4fv(shaderData.computeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
		glUniform3f(shaderData.computeUniforms.camera_pos, camera.camera.Position.x, camera.camera.Position.y, camera.camera.Position.z);
		glUniform2f(shaderData.computeUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));
		int prim_count = 0;
		for (auto p : data.primitives) {
			if (p.prim_type != 0) {
				prim_count += 1;
			}
		}

		glUniform3f(shaderData.computeUniforms.camera_pos_render, data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value);
		glUniform1i(shaderData.computeUniforms.u_prim_count, prim_count);
		glUniform1i(shaderData.computeUniforms.render_mode, (int)data.renderMode);
		glUniform1i(shaderData.computeUniforms.show_bounce, data.show_bounce);
		glUniform1f(shaderData.computeUniforms.render_mode_data1, data.minDepth);
		glUniform1f(shaderData.computeUniforms.render_mode_data2, data.maxDepth);
		glFinish();
		auto start = std::chrono::high_resolution_clock::now();

		if (render_sample < data.samples) {
			Shader::PrepareComputeShader(shaderData.computeUniforms, data.primCount(), data.groupModifierCount(), render_tile_x * TILE_WIDTH, render_tile_y * TILE_HEIGHT, t, data.samples, SAMPLES_PER_ITER, render_sample, data.cam_data);
			if (tiling) {
				glDispatchCompute(
					ceil((unsigned int)std::min(TILE_WIDTH, GetWindowData().screenWidth - render_tile_x * TILE_WIDTH) / 8),
					ceil((unsigned int)std::min(TILE_HEIGHT, GetWindowData().screenHeight - render_tile_y * TILE_HEIGHT) / 4),
					1
				);
			}
			else {
				glDispatchCompute(
					ceil((unsigned int)GetWindowData().screenWidth / 8),
					ceil((unsigned int)GetWindowData().screenHeight / 4),
					1
				);
			}

			// make sure writing to image has finished before read
			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			if (tiling) {
				render_tile_x++;
				if (TILE_WIDTH >= GetWindowData().screenWidth || render_tile_x * TILE_WIDTH > GetWindowData().screenWidth) {
					render_tile_x = 0;
					render_tile_y++;
				}
				if (TILE_HEIGHT >= GetWindowData().screenHeight || render_tile_y * TILE_HEIGHT > GetWindowData().screenHeight) {
					render_tile_y = 0;
					render_sample += SAMPLES_PER_ITER;
				}
			}
			else {
				render_sample++;
			}
		}

		glFinish();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
		if (ms != 0)
			std::cout << ms << "ms\n";
		glUseProgram(shaderData.computeVFShader);
		Shader::PrepareComputeShaderFragment(shaderData.computeVFUniforms, render_sample, data.samples);
		glUniform2f(shaderData.computeVFUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));
	}

	void RenderVS(RMImGui::ImGuiData& data, CameraData& camera, glm::mat4 view) {
		glUseProgram(shaderData.realtimeVFShader);

		glUniformMatrix4fv(shaderData.realtimeUniforms.camera_rot, 1, GL_FALSE, &view[0][0]);
		glUniform3f(shaderData.realtimeUniforms.camera_pos, camera.camera.Position.x, camera.camera.Position.y, camera.camera.Position.z);
		glUniform2f(shaderData.realtimeUniforms.u_resolution, float(GetWindowData().screenWidth), float(GetWindowData().screenHeight));
		glUniform1i(shaderData.realtimeUniforms.shading_mode, (int)data.shading_mode);

		bool render_cam = glm::length(camera.camera.Position - data.cam_pos.toVec()) > 0.5;
		glUniform1i(shaderData.realtimeUniforms.render_cam, render_cam);
		int prim_count = 0;
		for (auto p : data.primitives) {
			if (p.prim_type != 0) {
				prim_count += 1;
			}
		}

		glUniform3f(shaderData.realtimeUniforms.camera_pos_render, data.cam_pos[0].value, data.cam_pos[1].value, data.cam_pos[2].value);
		glUniform1i(shaderData.realtimeUniforms.u_prim_count, prim_count);

		Shader::PrepareShader(data.primCount(), data.groupModifierCount(), data.cam_data.focal_length.value, shaderData.realtimeUniforms);

	}

	void RenderQuad() {

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, shaderData.vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, shaderData.texbuffer);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void CleanUp() {
		glDeleteBuffers(1, &shaderData.vertexbuffer);
		glDeleteProgram(shaderData.realtimeVFShader);
		glDeleteProgram(shaderData.computeCS);
		glDeleteProgram(shaderData.computeVFShader);
	}
}