#pragma once
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h> 
#include <assimp/postprocess.h>
#include "stb_image.h";

#include "Mesh.h"
#include "Transform.h"

class Model {

public:
	Model(std::string path);
	void Draw(Shader& shader, Material& material);

	Transform transform;
private:
	
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene); 
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<Texture> textures_loaded;
};

unsigned int TextureFromFile(const char* path, const std::string& directory);