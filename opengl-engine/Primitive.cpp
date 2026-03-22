#include "Primitive.h"

Primitive::Primitive(float* vertices, int size, std::vector<unsigned int> indices, const std::string& DMdirectory, const std::string& SMdirectory) {

	std::vector<Vertex> vec_vertices;
	for (int i = 0; i <= size - 8; i = i + 8) {

		Vertex vertex;
		vertex.Position.x = vertices[i];
		vertex.Position.y = vertices[i + 1];
		vertex.Position.z = vertices[i + 2];
		vertex.Normal.x = vertices[i + 3];
		vertex.Normal.y = vertices[i + 4];
		vertex.Normal.z = vertices[i + 5];
		vertex.TexCoords.x = vertices[i + 6];
		vertex.TexCoords.y = vertices[i + 7];
		vec_vertices.push_back(vertex);
	}

	std::vector<Texture> textures;

	std::vector<Texture> diffuseMap;
	std::vector<Texture> specularMap;
	Texture DMtexture;
	DMtexture.id = TextureFromFile(DMdirectory);
	DMtexture.type = "texture_diffuse";
	DMtexture.path = DMdirectory;
	textures.push_back(DMtexture);

	Texture PMtexture;
	PMtexture.id = TextureFromFile(SMdirectory);
	PMtexture.type = "texture_specular";
	PMtexture.path = SMdirectory;
	textures.push_back(PMtexture);

	mesh = Mesh(vec_vertices, indices, textures);

}


void Primitive::Draw(Shader& shader, Material& material) {

	if (!outlined) {
		glStencilMask(0x00);//все биты закрыты, запрет записи в стенсил буфер
		mesh.Draw(shader, material);
	}
		
	if (outlined) {
		glStencilFunc(GL_ALWAYS, 1, 0xFF); 
		glStencilMask(0xFF); //все биты открыты = разрешаем запись в stencil буфер // каждый бит записывается как есть
		mesh.Draw(shader, material);
	}
}

void Primitive::DrawOutline(Shader& shader, Material& material) {
	//проходим тест, результат которого отправится в глобальную фукнцию glStencilOp
	//если ref не равен (GL_NOTEQUAL в данном случае) значению в стенсил буфере то пропускать, если равен отбрасывать (и все это прмиеняется к маске 0xFF которую можно кастомизировать)
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); 
	glStencilMask(0x00);  // каждый бит становится равным 0 (отключение записи)
	glDisable(GL_DEPTH_TEST); // чтобы рисовались поверх других обьектов

	mesh.Draw(shader, material);

	/*
	glStencilMask(0xFF); нужна в конце
	Для того чтобы glClear в начале следующего кадра мог очистить stencil буфер.
	glClear(GL_STENCIL_BUFFER_BIT) записывает 0 во все ячейки — но если glStencilMask(0x00) закрыта,
	запись заблокирована и glClear не сработает.Буфер останется с данными от предыдущего кадра.
	*/
	glEnable(GL_DEPTH_TEST);
	glStencilMask(0xFF);
	
}



unsigned int TextureFromFile(const std::string& directory) {

	unsigned int textureID;
	unsigned char* textureData;
	int textureWidth, textureHeight, texturenrChannels;

	textureData = stbi_load(directory.c_str(), &textureWidth, &textureHeight, &texturenrChannels, 0);
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
