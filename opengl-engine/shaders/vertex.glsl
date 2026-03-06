
#version 330 core
layout (location = 0) in vec3 aPos; //указатель расположения атрибутов вершин
layout (location = 1) in vec3 aColor;

out vec4 vertexColor; //обьефвляем переменную для цвета и последуюющей перадчи во фрагментный ешейдер
out vec3 ourColor;

void main(){
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);  //vertexColor = vec4(0.5, 0.0, 0.0, 1.0); }"; - ранний пример передачи vertexColor в фрагментный шейдер
  ourColor = aColor;
} 