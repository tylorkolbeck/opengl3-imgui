#pragma once
#include <GL/glew.h>
#include <unordered_map>
#include <string>
#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class ShaderProgram {
public:
	std::string filePath;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint id;
	GLuint m_RendererID;
	GLuint shader;
	ShaderProgram(const std::string& path);
	~ShaderProgram();
	ShaderProgramSource ParseShader();
	GLuint GetId();
	ShaderProgramSource shaderSource;
	static GLuint CompileShader(GLuint type, const std::string& source);
	unsigned int CreateShader();
	void Bind() const;
	void UnBind();

	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
	int GetUniformLocation(const std::string& name);

private:
	static GLuint s_id;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};
