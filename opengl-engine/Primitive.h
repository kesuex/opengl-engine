#pragma once
#include <iostream>
#include "stb_image.h";

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"


class Primitive {

public:
	Primitive(float* vertices, int size, std::vector<unsigned int> indices, const std::string& DMdirectory, const std::string& SMdirectory);
	void Draw(Shader& shader, Material& material); 

	Transform transform;
private:


	std::string directory;
	Mesh mesh;
};

unsigned int TextureFromFile(const std::string& directory);