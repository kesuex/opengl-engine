#include "Transform.h"

glm::mat4 Transform::GetMatrix() {

    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::translate(mat, Position);
    mat = glm::rotate(mat, glm::radians(Rotation.x), glm::vec3(1, 0, 0));
    mat = glm::rotate(mat, glm::radians(Rotation.y), glm::vec3(0, 1, 0));
    mat = glm::rotate(mat, glm::radians(Rotation.z), glm::vec3(0, 0, 1));
    mat = glm::scale(mat, Scale);
    return mat;
}

void Transform::ApplyUniformTransform(Shader& shader, Transform transform) {

    shader.SetUniformMatrix4fv("model", 1, transform.GetMatrix());
}
