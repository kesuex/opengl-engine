#pragma once
#include <iostream>
#include "stb_image.h";

#include "Mesh.h"
#include "Transform.h"


class Primitive {

public:
	Primitive(float* vertices, int size, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void Draw(Shader& shader); 

	Transform transform;
private:

	//std::vector<Texture> loadMaterialTextures(std::string typeName);

	//std::string directory;
	Mesh mesh;
	//std::vector<Texture> textures_loaded;
};

//unsigned int TextureFromFile(const char* path, const std::string& directory);