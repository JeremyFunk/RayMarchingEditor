#include "shader.h"
#include <sstream>
#include <fstream>
#include <string>
#include "constants.h"
#include <imgui.h>

namespace Shader {
	GLuint LoadUniform(const GLuint program, const char* uniform) {
		return glGetUniformLocation(program, uniform);
	}

	ComputeShaderUniforms LoadComputeShaderUniforms(const GLuint program) {
		ComputeShaderUniforms u;
		u.lens_size = LoadUniform(program, "lens_size");
		u.fp_dist = LoadUniform(program, "fp_dist");

		u.offsetX = LoadUniform(program, "xOffset");
		u.offsetY = LoadUniform(program, "yOffset");
		u.t = LoadUniform(program, "t");
		u.total_samples = LoadUniform(program, "total_samples");
		u.samples = LoadUniform(program, "samples");
		u.current_sample = LoadUniform(program, "current_sample");
		u.camera_pos = LoadUniform(program, "camera_pos");
		u.camera_rot = LoadUniform(program, "camera_rot");
		u.shading_mode = LoadUniform(program, "shading_mode");
		u.u_resolution = LoadUniform(program, "u_resolution");
		u.u_prim_count = LoadUniform(program, "u_prim_count");
		u.u_group_count = LoadUniform(program, "u_group_count");
		u.camera_pos_render = LoadUniform(program, "camera_pos_render");
		u.render_cam = LoadUniform(program, "render_cam");
		u.camera_dir_render = LoadUniform(program, "camera_dir_render");
		return u;
	}

	ComputeShaderFragmentUniforms LoadComputeShaderFragmentUniforms(const GLuint program) {
		ComputeShaderFragmentUniforms u;
		u.u_texture = LoadUniform(program, "u_texture");
		u.total_samples = LoadUniform(program, "total_samples");
		u.samples = LoadUniform(program, "samples");
		u.u_resolution = LoadUniform(program, "u_resolution");
		return u;
	}

	ShaderUniforms LoadUniforms(const GLuint program) {
		ShaderUniforms u;
		u.camera_pos = LoadUniform(program, "camera_pos");
		u.camera_rot = LoadUniform(program, "camera_rot");
		u.shading_mode = LoadUniform(program, "shading_mode");
		u.u_resolution = LoadUniform(program, "u_resolution");
		u.u_prim_count = LoadUniform(program, "u_prim_count");
		u.u_group_count = LoadUniform(program, "u_group_count");
		u.camera_pos_render = LoadUniform(program, "camera_pos_render");
		u.render_cam = LoadUniform(program, "render_cam");
		u.camera_dir_render = LoadUniform(program, "camera_dir_render");
		return u;
	}

	void PrepareComputeShader(const ComputeShaderUniforms uniforms, int prim_count, int mod_count, float offsetX, float offsetY, float t, int total_samples, int samples, int current_sample, float lens_size, float fp_dist) {
		glUniform1f(uniforms.lens_size, lens_size);
		glUniform1f(uniforms.fp_dist, fp_dist);

		glUniform1f(uniforms.offsetX, offsetX);
		glUniform1f(uniforms.offsetY, offsetY);
		glUniform1f(uniforms.t, t);
		glUniform1i(uniforms.samples, samples);
		glUniform1i(uniforms.total_samples, total_samples);
		glUniform1i(uniforms.current_sample, current_sample);
		glUniform1i(uniforms.u_prim_count, prim_count);
		glUniform1i(uniforms.u_group_count, mod_count);
	}

	void PrepareComputeShaderFragment(ComputeShaderFragmentUniforms uniforms, int samples, int total_samples) {
		glUniform1i(uniforms.samples, samples);
		glUniform1i(uniforms.total_samples, total_samples);
	}

	void PrepareShader(int prim_count, int mod_count, ShaderUniforms uniforms)
	{
		glUniform1i(uniforms.u_prim_count, prim_count);
		glUniform1i(uniforms.u_group_count, mod_count);
	}


	GLuint LoadComputeShader(const char* compute_shader_file_path) {

		// Create the shaders
		GLuint ID = glCreateShader(GL_COMPUTE_SHADER);

		// Read the Vertex Shader code from the file
		std::string ComputeShaderCode;
		std::ifstream ComputeShaderStream(compute_shader_file_path, std::ios::in);
		if (ComputeShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << ComputeShaderStream.rdbuf();
			ComputeShaderCode = sstr.str();
			ComputeShaderStream.close();
		}
		else {
			printf("Cannot open %s!\n", compute_shader_file_path);
			getchar();
			return 0;
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;


		// Compile Vertex Shader
		printf("Compiling compute shader : %s\n", compute_shader_file_path);
		char const* ComputeSourcePointer = ComputeShaderCode.c_str();
		glShaderSource(ID, 1, &ComputeSourcePointer, NULL);
		glCompileShader(ID);

		// Check Vertex Shader
		glGetShaderiv(ID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(ID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
			printf("%s\n", &ComputeShaderErrorMessage[0]);
		}


		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, ID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}


		glDetachShader(ProgramID, ID);
		glDeleteShader(ID);

		return ProgramID;
	}

	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

		// Create the shaders
		GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		// Read the Vertex Shader code from the file
		std::string VertexShaderCode;
		std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
		if (VertexShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << VertexShaderStream.rdbuf();
			VertexShaderCode = sstr.str();
			VertexShaderStream.close();
		}
		else {
			printf("Cannot open %s!\n", vertex_file_path);
			getchar();
			return 0;
		}

		// Read the Fragment Shader code from the file
		std::string FragmentShaderCode;
		std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
		if (FragmentShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << FragmentShaderStream.rdbuf();
			FragmentShaderCode = sstr.str();
			FragmentShaderStream.close();
		}

		GLint Result = GL_FALSE;
		int InfoLogLength;


		// Compile Vertex Shader
		printf("Compiling shader : %s\n", vertex_file_path);
		char const* VertexSourcePointer = VertexShaderCode.c_str();
		glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
		glCompileShader(VertexShaderID);

		// Check Vertex Shader
		glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
			printf("%s\n", &VertexShaderErrorMessage[0]);
		}



		// Compile Fragment Shader
		printf("Compiling shader : %s\n", fragment_file_path);
		char const* FragmentSourcePointer = FragmentShaderCode.c_str();
		glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
		glCompileShader(FragmentShaderID);

		// Check Fragment Shader
		glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
		glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
			glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
			printf("%s\n", &FragmentShaderErrorMessage[0]);
		}



		// Link the program
		printf("Linking program\n");
		GLuint ProgramID = glCreateProgram();
		glAttachShader(ProgramID, VertexShaderID);
		glAttachShader(ProgramID, FragmentShaderID);
		glLinkProgram(ProgramID);

		// Check the program
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
		glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
		if (InfoLogLength > 0) {
			std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
			glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
			printf("%s\n", &ProgramErrorMessage[0]);
		}


		glDetachShader(ProgramID, VertexShaderID);
		glDetachShader(ProgramID, FragmentShaderID);

		glDeleteShader(VertexShaderID);
		glDeleteShader(FragmentShaderID);

		return ProgramID;
	}
}