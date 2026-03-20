#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"
#include "Camera.h"


struct PointLight {
	glm::vec3 position;
	glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	void ApplyUniformInit(Shader& shader, glm::vec3 position) {
		shader.SetUniformVec3fv("pointlight.position", 1, position);
		shader.SetUniformVec3fv("pointlight.ambient", 1, ambient);
		shader.SetUniformVec3fv("pointlight.diffuse", 1, diffuse);
		shader.SetUniformVec3fv("pointlight.specular", 1, specular);
		shader.SetUniformFloat("pointlight.constant", constant);
		shader.SetUniformFloat("pointlight.linear", linear);
		shader.SetUniformFloat("pointlight.quadratic", quadratic);
	}

	void ApplyUniformArrayInit(Shader& shader, std::string uniformArrayName, int arraySize, glm::vec3* positionArray) {

		for (int i = 0; i < arraySize; ++i) {
			std::string convertedName = uniformArrayName + "[" + std::to_string(i) + "].";
			shader.SetUniformVec3fv((convertedName + "position").c_str(), 1, positionArray[i]);
			shader.SetUniformVec3fv((convertedName + "ambient").c_str(), 1, ambient);
			shader.SetUniformVec3fv((convertedName + "diffuse").c_str(), 1, diffuse);
			shader.SetUniformVec3fv((convertedName + "specular").c_str(), 1, specular);
			shader.SetUniformFloat((convertedName + "constant").c_str(), constant);
			shader.SetUniformFloat((convertedName + "linear").c_str(), linear);
			shader.SetUniformFloat((convertedName + "quadratic").c_str(), quadratic);

		}
		
	}
};




struct DirectLight {

	glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
	glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

	void ApplyUniformInit(Shader& shader) {
		shader.SetUniformVec3fv("directlight.direction", 1, direction);
		shader.SetUniformVec3fv("directlight.ambient", 1, ambient);
		shader.SetUniformVec3fv("directlight.diffuse", 1, diffuse);
		shader.SetUniformVec3fv("directlight.specular", 1, specular);
	}
	
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	float cutOff = 12.5f;
	float outerCutOff = 17.5f;

	void ApplyUniformInit(Shader& shader) {
		shader.SetUniformVec3fv("spotlight.ambient", 1, ambient);
		shader.SetUniformVec3fv("spotlight.diffuse", 1, diffuse);
		shader.SetUniformVec3fv("spotlight.specular", 1, specular);
		shader.SetUniformFloat("spotlight.constant", constant);
		shader.SetUniformFloat("spotlight.linear", linear);
		shader.SetUniformFloat("spotlight.quadratic", quadratic);
		shader.SetUniformFloat("spotlight.cutOff", glm::cos(glm::radians(cutOff)));
		shader.SetUniformFloat("spotlight.outerCutOff", glm::cos(glm::radians(outerCutOff)));
	}

	void ApplyUniformRunTime(Shader& shader, Camera& camera) {
		shader.SetUniformVec3fv("spotlight.position", 1, camera.cameraPosition);
		shader.SetUniformVec3fv("spotlight.direction", 1, camera.cameraFront);		
	}

};

