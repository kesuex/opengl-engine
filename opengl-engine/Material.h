#pragma once

#include "Shader.h"


struct Material {

	virtual void ApplyUniformForTextures(Shader& shader, std::string name, int number) = 0;
	virtual void ApplyUniformForParameters(Shader& shader) = 0;
	virtual ~Material() {};

	float shininess = 32.0f;
};

struct Phong: public Material {

	virtual void ApplyUniformForTextures(Shader& shader, std::string name, int number) override {
		shader.SetUniformInt(("material." + name).c_str(), number);
	}

	virtual void ApplyUniformForParameters(Shader& shader) override {
		shader.SetUniformFloat("material.shininess", shininess);
	}
	
	
};