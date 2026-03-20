#include "Model.h"

Model::Model(std::string path) {

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {

		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);

}


void Model::Draw(Shader& shader, Material& material) {

	for (int i = 0; i < meshes.size(); ++i) {
		meshes[i].Draw(shader, material);
	}
}
	

void Model::processNode(aiNode* node, const aiScene* scene) {

	// обработать все сетки узла (если есть) 
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// затем проделать то же самое для каждого из его дочерних элементов
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}


Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		glm::vec3 vector;
		glm::vec3 normal;
		glm::vec2 vec;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		vertex.Normal = normal;

		if (mesh->mTextureCoords[0]) {	
			vec.x = mesh->mTextureCoords[0][i].x; 
			vec.y = mesh->mTextureCoords[0][i].y; 
			vertex.TexCoords= vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i){

		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) { 

		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex]; 
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); 
		textures.insert(textures.end(),diffuseMaps.begin(), diffuseMaps.end()); 
		std::vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular"); 
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}


	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {

	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for ( int j = 0; j < textures_loaded.size(); ++j) {

			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]); 
				skip = true;
				break;
			}
		}
		if (!skip) {

			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
		
	}
	return textures;
}


unsigned int TextureFromFile(const char* textureName, const std::string& directory) {

	unsigned int textureID;
	unsigned char* textureData;
	int textureWidth, textureHeight, texturenrChannels;

	std::string textureFullPath = directory + '/' + std::string(textureName);
	/*
	Первый аргумент указывает цель текстуры; установка значения GL_TEXTURE_2D означает, что эта
	операция сгенерирует текстуру на текущем связанном объекте текстуры с той же целью (поэтому любые
	текстуры, связанные с целями GL_TEXTURE_1D или GL_TEXTURE_3D, не будут затронуты).
	• Второй аргумент указывает уровень мипмапа, для которого мы хотим создать текстуру, если вы хотите
	установить каждый уровень мипмапа вручную, но мы оставим его на базовом уровне, который равен 0.
	• Третий аргумент указывает OpenGL, в каком формате мы хотим сохранить текстуру. Наше изображение
	имеет только значения RGB, поэтому мы также сохраним текстуру со значениями RGB.
	• Четвертый и пятый аргументы задают ширину и высоту результирующей текстуры. Мы сохранили их ранее
	при загрузке изображения, поэтому будем использовать соответствующие переменные.
	• Следующий аргумент всегда должен быть равен 0 (некоторые устаревшие вещи).
	• 7-й и 8-й аргументы определяют формат и тип данных исходного изображения. Мы загрузили изображение со
	значениями RGB и сохранили их в виде символов (байт), поэтому передадим соответствующие значения.
	• Последний аргумент — это фактические данные изображения.
	*/
	textureData = stbi_load(textureFullPath.c_str(), &textureWidth, &textureHeight, &texturenrChannels, 0);
	//Создаем текстуру
	glGenTextures(1, &textureID); // функция принимает количество текстур и сохраняет в переменной texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);//привязка обьекта к типу

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureData) {
		//GL_RGBA или GL_RGB  - параметр влияющий на формат пнг или джейпег 
		GLenum GLformat;
		if (texturenrChannels == 4) GLformat = GL_RGBA;
		else GLformat = GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GLformat, GL_UNSIGNED_BYTE, textureData); //привязка связанного обьекта текстуры к изображению 
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << " Cant load a texture";
	}
	stbi_image_free(textureData); //освобождаем память

	return textureID;
}
