#include "Camera.h"


void Camera::processInput(GLFWwindow* window) {

	currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float cameraSpeed = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}



void Camera::ProcessMouseMovement(double xpos, double ypos) {

	float sensitivity = 0.1f;
	float xoffset;
	float yoffset;
	glm::vec3 direction;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	xoffset = xpos - lastX;
	yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;


	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}



 void Camera::ProcessScrolling(double xoffset, double yoffset) {

	zoom -= (float)yoffset;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}



 glm::mat4 Camera::GetProjectionMatrix(float width, float height) {
	 return glm::perspective(glm::radians(zoom), width / height, Near, Far);
 }

 glm::mat4 Camera::GetViewMatrix() {
	 return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
 }

 void Camera::ApplyUniformsView(Shader& shader, float width, float height) {
	 shader.SetUniformMatrix4fv("view", 1, GetViewMatrix());
	 shader.SetUniformMatrix4fv("projection", 1, GetProjectionMatrix(width, height));
	 shader.SetUniformVec3fv("viewPos", 1, cameraPosition);
 }
