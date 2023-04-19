#include <string>
#include <sstream>
#include <fstream>
#include "ShaderProgram.h"
#include <GL/glew.h>
#include <iostream>

#include "Renderer.h"

GLuint ShaderProgram::s_id = 0;

ShaderProgram::ShaderProgram(const std::string& path)
{
	filePath = path;
	s_id++;
	id;
	shaderSource = ParseShader();
	vertexShader = CompileShader(GL_VERTEX_SHADER, shaderSource.VertexSource);
	fragmentShader = CompileShader(GL_FRAGMENT_SHADER, shaderSource.FragmentSource);
	m_RendererID = CreateShader();
}

ShaderProgram::~ShaderProgram()
{
	GLCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource ShaderProgram::ParseShader() {
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

GLuint ShaderProgram::GetId()
{
	return GLuint();
}

GLuint ShaderProgram::CompileShader(GLuint type, const std::string& source)
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
unsigned int ShaderProgram::CreateShader() {
	shader = glCreateProgram();

	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);
	glValidateProgram(shader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shader;

	//glUseProgram(shader);
}

void ShaderProgram::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}
void ShaderProgram::UnBind()
{
	GLCall(glUseProgram(0));
}

void ShaderProgram::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void ShaderProgram::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void ShaderProgram::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}


void ShaderProgram::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	GLCall(glUniform4f(GetUniformLocation(name), f0, f1, f2, f3));
}

int ShaderProgram::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	

	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));

	if (location == -1)
		std::cout << "Warning: uniform " << name << "does not exist" << std::endl;
	
	m_UniformLocationCache[name] = location;
	return location;

}
