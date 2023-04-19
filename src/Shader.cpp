#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.h"

#include "Renderer.h"

Shader::Shader(std::string& filepath)
    : m_filePath(filepath), m_RendererID(0)
{

}

Shader::~Shader()
{

}

void Shader::Bind() const
{
}

void Shader::Unbind() const
{
}

void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
}

unsigned int Shader::GetUniformLocation(const std::string& name) const
{
    return 0;
}

ShaderProgramSource Shader::ParseShader(std::string& filePath) {
	{
		std::ifstream stream(filePath);

		enum class ShaderType
		{
			NONE = -1,
			VERTEX = 0,
			FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;

		while (getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}
			}
			else
			{
				ss[(int)type] << line << '\n';
			}
		}

		return { ss[0].str(), ss[1].str() };
	}
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);

		char* failType = (char*)(type == GL_VERTEX_SHADER ? "vertex" : "fragment");

		std::cout << "Failed to compile shader" << failType << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

