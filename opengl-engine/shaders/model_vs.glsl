
#version 330 core
layout (location = 0) in vec3 aPos; //указатель расположения атрибутов вершин
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

																				//out vec4 vertexColor; //обьефвляем переменную для цвета и последуюющей перадчи во фрагментный ешейдер
out vec3 Normal;
out vec3 FragPos; 
out vec2 TexCoord;
                                                                               //uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

  gl_Position = projection * view * model * vec4(aPos, 1.0);  //vertexColor = vec4(0.5, 0.0, 0.0, 1.0); - ранний пример передачи vertexColor в фрагментный шейдер
  Normal = mat3(transpose(inverse(model))) * aNormal;	//расчет нормали таким образом чтобы масштабирование обьекта не изменило угол нормали					    // gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0); - ранний пример перемещения обьекта в сцене
  FragPos = vec3(model * vec4(aPos, 1.0)); //расчте позиции вершины для какждой вершины и перевод их координат в мировое простратнстов
  TexCoord = aTexCoord;

} 