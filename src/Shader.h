#pragma once
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Shader
{
	uint32_t program = 0;

	// render shader ctor
	Shader(const std::string& vertFile, const std::string& fragFile)
	{
		std::ifstream fVert(vertFile, std::ios::in);
		assert(fVert.is_open());

		std::stringstream vShaderStream;
		vShaderStream << fVert.rdbuf();
		fVert.close();

		std::string vertexCode = vShaderStream.str();
		const char* vertCode = vertexCode.c_str();


		std::ifstream fFrag(fragFile);
		assert(fFrag.is_open());

		std::stringstream fShaderStream;
		fShaderStream << fFrag.rdbuf();
		fFrag.close();

		std::string fragmentCode = fShaderStream.str();
		const char* fragCode = fragmentCode.c_str();


		int success;
		char infoLog[512];

		uint32_t shaderVert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(shaderVert, 1, &vertCode, nullptr);
		glCompileShader(shaderVert);
		glGetShaderiv(shaderVert, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderVert, 512, nullptr, infoLog);
			std::cerr << "Vertex Shader Error: " << infoLog << std::endl;
			assert(false);
		}

		uint32_t shaderFrag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(shaderFrag, 1, &fragCode, nullptr);
		glCompileShader(shaderFrag);
		glGetShaderiv(shaderFrag, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderFrag, 512, nullptr, infoLog);
			std::cerr << "Fragment Shader Error: " << infoLog << std::endl;
			assert(false);
		}


		program = glCreateProgram();
		glAttachShader(program, shaderVert);
		glAttachShader(program, shaderFrag);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			std::cerr << "Shader Linking Error: " << infoLog << std::endl;
			assert(false);
		}

		glDeleteShader(shaderVert);
		glDeleteShader(shaderFrag);
	}

	// compute shader ctor
	Shader(const std::string& path)
	{
		std::ifstream file(path, std::ios::in);
		assert(file.is_open());

		std::stringstream vStream;
		vStream << file.rdbuf();
		file.close();

		std::string codeStr = vStream.str();
		const char* code = codeStr.c_str();


		int success;
		char infoLog[512];

		uint32_t shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &code, nullptr);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cerr << "Compute Shader Error: " << infoLog << std::endl;
			assert(false);
		}


		program = glCreateProgram();
		glAttachShader(program, shader);
		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program, 512, nullptr, infoLog);
			std::cerr << "Shader Linking Error: " << infoLog << std::endl;
			assert(false);
		}

		glDeleteShader(shader);
	}

	~Shader()
	{
		glDeleteProgram(program);
	}

	template <class T>
	inline void setValue(const std::string& name, T value) const
	{
		assert(false);
	}

	void bind() const
	{
		glUseProgram(program);
	}
};

template<> void Shader::setValue(const std::string& name, int val) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), val);
}
template<> void Shader::setValue(const std::string& name, uint32_t val) const
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), val);
}
template<> void Shader::setValue(const std::string& name, glm::ivec2 val) const
{
	glUniform2i(glGetUniformLocation(program, name.c_str()), val.x, val.y);
}
template<> void Shader::setValue(const std::string& name, glm::ivec3 val) const
{
	glUniform3i(glGetUniformLocation(program, name.c_str()), val.x, val.y, val.z);
}
template<> void Shader::setValue(const std::string& name, glm::ivec4 val) const
{
	glUniform4i(glGetUniformLocation(program, name.c_str()), val.x, val.y, val.z, val.w);
}

template<> void Shader::setValue(const std::string& name, float val) const
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), val);
}
template<> void Shader::setValue(const std::string& name, glm::vec2 val) const
{
	glUniform2f(glGetUniformLocation(program, name.c_str()), val.x, val.y);
}
template<> void Shader::setValue(const std::string& name, glm::vec3 val) const
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), val.x, val.y, val.z);
}
template<> void Shader::setValue(const std::string& name, glm::vec4 val) const
{
	glUniform4f(glGetUniformLocation(program, name.c_str()), val.x, val.y, val.z, val.w);
}

template<> void Shader::setValue(const std::string& name, glm::mat3 val) const
{
	glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}
template<> void Shader::setValue(const std::string& name, glm::mat4 val) const
{
	glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}