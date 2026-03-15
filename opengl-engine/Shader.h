#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
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
	void SetUniformVec4fv(const std::string& name, int сount, glm::vec4 vec4) const;
	void SetUniformVec3fv(const std::string& name, int сount, glm::vec3 vec3) const;
	void SetUniformMatrix4fv(const std::string& name, int сount, glm::mat4 mat4) const;
	void SetUniformInt(const std::string& name, int value) const;
	void SetUniformFloat(const std::string& name, float value) const;
};