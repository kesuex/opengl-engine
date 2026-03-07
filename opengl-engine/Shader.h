#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

class Shader {
public:
	unsigned int shaderProgram;

	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void IsCompileVertexShaderSuccessfully(unsigned int vertexShader);
	void IsCompileFragmentShaderSuccessfully(unsigned int fragmentShader);
	void IsLinkingShaderProgramSuccessfully(unsigned int shaderProgram);
	void UseShaderProgram();
	void SetUniformVec4(const std::string& name, float x, float y, float z, float w) const;
	void SetUniformInt(const std::string& name, int value) const;
};