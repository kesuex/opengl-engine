#include "Primitive.h"

Primitive::Primitive(float* vertices, int size, std::vector<unsigned int> indices, std::vector<Texture> textures) {

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

	mesh = Mesh(vec_vertices, indices, {});

}

void Primitive::Draw(Shader& shader,Material& material) {
	mesh.Draw(shader, material);
}
