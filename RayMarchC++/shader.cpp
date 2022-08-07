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
		u.apeture_size = LoadUniform(program, "apeture_size");
		u.focal_length = LoadUniform(program, "focal_length");
		u.focus_dist = LoadUniform(program, "focus_dist");

		u.offsetX = LoadUniform(program, "xOffset");
		u.offsetY = LoadUniform(program, "yOffset");
		u.t = LoadUniform(program, "t");
		u.total_samples = LoadUniform(program, "total_samples");
		u.number_samples = LoadUniform(program, "number_samples");
		u.current_sample = LoadUniform(program, "current_sample");
		u.camera_pos = LoadUniform(program, "camera_pos");
		u.camera_rot = LoadUniform(program, "camera_rot");
		u.u_resolution = LoadUniform(program, "u_resolution");
		u.u_prim_count = LoadUniform(program, "u_prim_count");
		u.u_group_count = LoadUniform(program, "u_group_count");
		u.camera_pos_render = LoadUniform(program, "camera_pos_render");
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
		u.focal_length = LoadUniform(program, "focal_length");
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

	void PrepareComputeShader(const ComputeShaderUniforms uniforms, int prim_count, int mod_count, float offsetX, float offsetY, float t, int total_samples, int samples, int current_sample, RMImGui::CameraData cam_data) {
		glUniform1f(uniforms.focal_length, cam_data.focal_length.value);
		glUniform1f(uniforms.focus_dist, cam_data.focus_dist.value);
		glUniform1f(uniforms.apeture_size, cam_data.apeture_size.value);

		glUniform1f(uniforms.offsetX, offsetX);
		glUniform1f(uniforms.offsetY, offsetY);
		glUniform1f(uniforms.t, t);
		glUniform1i(uniforms.number_samples, samples);
		glUniform1i(uniforms.total_samples, total_samples);
		glUniform1i(uniforms.current_sample, current_sample);
		glUniform1i(uniforms.u_prim_count, prim_count);
		glUniform1i(uniforms.u_group_count, mod_count);
	}

	void PrepareComputeShaderFragment(ComputeShaderFragmentUniforms uniforms, int samples, int total_samples) {
		glUniform1i(uniforms.samples, samples);
		glUniform1i(uniforms.total_samples, total_samples);
	}

	void PrepareShader(int prim_count, int mod_count, float focal_length, ShaderUniforms uniforms)
	{
		glUniform1f(uniforms.focal_length, focal_length);
		glUniform1i(uniforms.u_prim_count, prim_count);
		glUniform1i(uniforms.u_group_count, mod_count);
	}

	GLuint LoadComputeShader(std::string content, std::string name) {
		// Create the shaders
		GLuint ID = glCreateShader(GL_COMPUTE_SHADER);

		GLint Result = GL_FALSE;
		int InfoLogLength;

		// Compile Vertex Shader
		printf("Compiling compute shader : %s\n", name);
		char const* ComputeSourcePointer = content.c_str();
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

	GLuint LoadComputeShaderByPath(const char* compute_file_path) {
		std::string CompShaderCode;
		std::ifstream CompShaderStream(compute_file_path, std::ios::in);
		if (CompShaderStream.is_open()) {
			std::stringstream sstr;
			sstr << CompShaderStream.rdbuf();
			CompShaderCode = sstr.str();
			CompShaderStream.close();
		}
		else {
			printf("Cannot open %s!\n", compute_file_path);
			getchar();
			return 0;
		}

		LoadComputeShader(CompShaderCode, compute_file_path);
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