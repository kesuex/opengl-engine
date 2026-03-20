#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"


class Camera {

public:
		void processInput(GLFWwindow* window);
		void ProcessMouseMovement(double xpos, double ypos);
		void ProcessScrolling(double xoffset, double yoffset);
		void ApplyUniformsView(Shader& shader, float width, float height);

		glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
		//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
private:
		glm::mat4 GetProjectionMatrix(float width, float height);
		glm::mat4 GetViewMatrix();
		
		bool firstMouse = true;
		double lastX;
		double lastY;
		float yaw = -90.f;
		float pitch = 0;

		float zoom = 45.0f;
		float Near = 0.1f;
		float Far = 100.0f;	

		float deltaTime = 0.0f; // Время между текущим кадром и последним кадром 
		float lastFrame = 0.0f; // Время последнего кадра
		float currentFrame = 0.0f;
};