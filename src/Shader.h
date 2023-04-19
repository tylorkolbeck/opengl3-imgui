#pragma once
#include <string>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_filePath;
	unsigned int m_RendererID;
	// caching for uniforms
public:
	Shader(std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);

private:
	unsigned int GetUniformLocation(const std::string& name) const;
	ShaderProgramSource ParseShader(std::string& filePath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};

