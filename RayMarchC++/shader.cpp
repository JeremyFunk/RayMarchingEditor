#include "shader.h"
#include <sstream>
#include <fstream>
#include <string>
#include "constants.h"

namespace Shader {
	GLuint LoadUniform(const GLuint program, const char* uniform) {
		return glGetUniformLocation(program, uniform);
	}

	ShaderUniforms LoadUniforms(const GLuint program) {
		ShaderUniforms u;
		u.camera_pos = LoadUniform(program, "camera_pos");
		u.camera_rot = LoadUniform(program, "camera_rot");
		u.shading_mode = LoadUniform(program, "shading_mode");
		u.u_resolution = LoadUniform(program, "u_resolution");
		u.u_prim_count = LoadUniform(program, "u_prim_count");
		for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
			u.u_group_modifier[i].modifier = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].modifier").c_str());
			u.u_group_modifier[i].primAttribute = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].primAttribute").c_str());
			u.u_group_modifier[i].prim0 = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].prim0").c_str());
			u.u_group_modifier[i].prim1 = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].prim1").c_str());
			u.u_group_modifier[i].prim2 = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].prim2").c_str());
			u.u_group_modifier[i].prim3 = LoadUniform(program, std::string("u_group_modifier[" + std::to_string(i) + "].prim3").c_str());
		}

		for (int i = 0; i < COUNT_PRIMITIVE; i++) {
			u.primitives[i].prim_type = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].prim_type").c_str());
			u.primitives[i].position = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].position").c_str());
			u.primitives[i].transformation = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].transformation").c_str());
			u.primitives[i].attribute0 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute0").c_str());
			u.primitives[i].attribute1 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute1").c_str());
			u.primitives[i].attribute2 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute2").c_str());
			u.primitives[i].attribute3 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute3").c_str());
			u.primitives[i].attribute4 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute4").c_str());
			u.primitives[i].attribute5 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute5").c_str());
			u.primitives[i].attribute6 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute6").c_str());
			u.primitives[i].attribute7 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute7").c_str());
			u.primitives[i].attribute8 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute8").c_str());
			u.primitives[i].attribute9 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].attribute9").c_str());

			for (int j = 0; j < COUNT_PRIMITIVE_MODIFIER; j++) {
				u.primitives[i].modifiers[j].modifier = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifier").c_str());
				u.primitives[i].modifiers[j].modifierAttribute0 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifierAttribute0").c_str());
				u.primitives[i].modifiers[j].modifierAttribute1 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifierAttribute1").c_str());
				u.primitives[i].modifiers[j].modifierAttribute2 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifierAttribute2").c_str());
				u.primitives[i].modifiers[j].modifierAttribute3 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifierAttribute3").c_str());
				u.primitives[i].modifiers[j].modifierAttribute4 = LoadUniform(program, std::string("u_primitives[" + std::to_string(i) + "].modifiers[" + std::to_string(j) + "].modifierAttribute4").c_str());
			}
		}
		return u;
	}

	void PrepareShader(const Primitive::ShaderPrimitive primitives[], const Primitive::ShaderGroupPrimitive modifiers[], ShaderUniforms uniforms)
	{
		for (int i = 0; i < COUNT_GROUP_MODIFIER; i++) {
			glUniform1i(uniforms.u_group_modifier[i].modifier, modifiers[i].modifier);
			glUniform1i(uniforms.u_group_modifier[i].prim0, modifiers[i].prim0);
			glUniform1i(uniforms.u_group_modifier[i].prim1, modifiers[i].prim1);
			glUniform1i(uniforms.u_group_modifier[i].prim2, modifiers[i].prim2);
			glUniform1i(uniforms.u_group_modifier[i].prim3, modifiers[i].prim3);
			glUniform1f(uniforms.u_group_modifier[i].primAttribute, modifiers[i].primAttribute);
		}

		for (int i = 0; i < COUNT_PRIMITIVE; i++) {
			auto v = &primitives[i];

			if (v->prim_type == 0) {
				glUniform1i(uniforms.primitives[i].prim_type, 0);
				continue;
			}
			glUniformMatrix3fv(uniforms.primitives[i].transformation, 1, GL_FALSE, &(*v).transformation.matrix[0][0]);

			glUniform1i(uniforms.primitives[i].prim_type, (*v).prim_type);
			glUniform3f(uniforms.primitives[i].position, (*v).transformation.position.x, (*v).transformation.position.y, (*v).transformation.position.z);
			glUniform1f(uniforms.primitives[i].attribute0, (*v).values[0]);
			glUniform1f(uniforms.primitives[i].attribute1, (*v).values[1]);
			glUniform1f(uniforms.primitives[i].attribute2, (*v).values[2]);
			glUniform1f(uniforms.primitives[i].attribute3, (*v).values[3]);
			glUniform1f(uniforms.primitives[i].attribute4, (*v).values[4]);
			glUniform1f(uniforms.primitives[i].attribute5, (*v).values[5]);
			glUniform1f(uniforms.primitives[i].attribute6, (*v).values[6]);
			glUniform1f(uniforms.primitives[i].attribute7, (*v).values[7]);
			glUniform1f(uniforms.primitives[i].attribute8, (*v).values[8]);
			glUniform1f(uniforms.primitives[i].attribute9, (*v).values[9]);

			for (int j = 0; j < COUNT_PRIMITIVE_MODIFIER; j++) {
				glUniform1i(uniforms.primitives[i].modifiers[j].modifier, (*v).modifiers[j].modifier);
				glUniform1f(uniforms.primitives[i].modifiers[j].modifierAttribute0, (*v).modifiers[j].attribute0);
				glUniform1f(uniforms.primitives[i].modifiers[j].modifierAttribute1, (*v).modifiers[j].attribute1);
				glUniform1f(uniforms.primitives[i].modifiers[j].modifierAttribute2, (*v).modifiers[j].attribute2);
				glUniform1f(uniforms.primitives[i].modifiers[j].modifierAttribute3, (*v).modifiers[j].attribute3);
				glUniform1f(uniforms.primitives[i].modifiers[j].modifierAttribute4, (*v).modifiers[j].attribute4);
			}
		}
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
			printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
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