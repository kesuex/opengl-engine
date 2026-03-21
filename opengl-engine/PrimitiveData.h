#pragma once
#include "Mesh.h"

namespace PrimitiveData
{
    namespace Cube
    {
		float vertices[] = {
			// Front face (normal: 0, 0, 1)
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  // 0
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,  // 1
			 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  // 2
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,  // 3

			// Back face (normal: 0, 0, -1)
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f,  // 4
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,  // 5
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f,  // 6
			-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,  // 7

			// Left face (normal: -1, 0, 0)
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // 8
			-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // 9
			-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // 10
			-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // 11

			// Right face (normal: 1, 0, 0)
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,  // 12
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // 13
			 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // 14
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,  // 15

			 // Bottom face (normal: 0, -1, 0)
			 -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f,  // 16
			  0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f,  // 17
			  0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f,  // 18
			 -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,  // 19

			 // Top face (normal: 0, 1, 0)
			 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // 20
			  0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,  // 21
			  0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  // 22
			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  // 23
		};
		int size = sizeof(vertices) / sizeof(float);
		std::vector<unsigned int> indices = {
		  0, 1, 2,     2, 3, 0,
		  4, 5, 6,     6, 7, 4,
		  8, 9, 10,    10, 11, 8,
		  12, 13, 14,  14, 15, 12,
		  16, 17, 18,  18, 19, 16,
		  20, 21, 22,  22, 23, 20
		};
    }

	namespace Plane
	{
		float vertices[] = {
			// texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
			 5.0f, -0.5f,  5.0f,    0.0f, 1.0f, 0.0f,    2.0f, 0.0f,
			-5.0f, -0.5f,  5.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f,
			-5.0f, -0.5f, -5.0f,    0.0f, 1.0f, 0.0f,    0.0f, 2.0f,
			 5.0f, -0.5f, -5.0f,    0.0f, 1.0f, 0.0f,    2.0f, 2.0f
		};
		int size = sizeof(vertices) / sizeof(float);
		std::vector<unsigned int> indices = {
			0, 1, 2,   2, 3, 0
		};
	}
	
}


