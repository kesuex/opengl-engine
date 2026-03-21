#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"



class Transform {

public:
    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 Scale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::mat4 GetMatrixSaved();
    void ApplyUniformSaved(Shader& shader, Transform transform);

    glm::mat4 GetMatrix(glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale);
    void ApplyUniform(Shader& shader, glm::vec3 Position, glm::vec3 Rotation, glm::vec3 Scale);

    
};

