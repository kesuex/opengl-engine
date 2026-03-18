#include "Mesh.h"
#include "stb_image.h";

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {

	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	//Создание объекта буфера  VAO, VBO, EBO в видеопамяти
	glGenVertexArrays(1, &VAO); //генерируем уникальный идентификатор конфигурации атрибутов VAO и создаем этот обьект
	glGenBuffers(1, &VBO); //генерируем уникальный идентификатор буфера-обьекта вершин VBO и создаем этот обьект
	glGenBuffers(1, &EBO); // генерируем уникальный идентификатор буфера-обьекта индексов EBO и создаем этот обьект

	//Активациия VAO и связывание буферов VBO, EBO
	glBindVertexArray(VAO); // активируем обьект VAO для дальнейшей настройки
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // связываем созданныё буфферный обьект с типом буфера индексов - GL_ELEMENT_ARRAY_BUFFER, запись в VAO

	//Копирование данных в GPU память
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); // копируем данные вершин из массива оперативной памяти в буфер видеопамяти видеокарты
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);// копируем данные индексов из массива оперативной памяти в буфер видеопамяти видеокарты

	//Записываем конфигурацию атрибута GL_ARRAY_BUFFER (к которому привязан текущий VBO) в активный VAO, после этого можно отвязать VBO - glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(Vertex), (void*)0);// устанавливаем указатели атрибутов вершин
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(Vertex), (void*)offsetof(Vertex, Normal));// устанавливаем указатели нормалей
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));// устанавливаем указатели атрибутов тексур

	//Включаем атрибуты
	glEnableVertexAttribArray(0);// включаем атрибут вершины
	glEnableVertexAttribArray(1);// включаем атрибут цвета
	glEnableVertexAttribArray(2);// включаем атрибут текстур


	//Деактивация VAO и отвязывание VBO. EBO отвязывать не нужно. Если отвязывать то только после деактивации VAO, иначе VAO потеряет доступ к нему
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}


void Mesh::Draw(Shader& shader) {

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // сначала активируем текстурный блок
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	
	glBindVertexArray(VAO); //связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

