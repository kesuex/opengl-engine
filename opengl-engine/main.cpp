#include "Shader.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //задаем размер отображения для опенгл в нашем окне GLFW
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;

/*
VBO — VAO запоминает не сам VBO, а настройку glVertexAttribPointer.После того как настройка записана, VBO можно
отвязать — VAO уже знает что нужно.

EBO — VAO запоминает саму привязку GL_ELEMENT_ARRAY_BUFFER напрямую.Если отвязать EBO пока VAO активен — VAO потеряет
ссылку на него и glDrawElements сломается.

Кратко: VBO хранится через атрибут, EBO хранится напрямую в VAO.
*/
unsigned int VBO;	//храналище вершин
unsigned int EBO;  //позволяет переиспользовать вершины из VBO, указывая на них по индексу.							
unsigned int VAO; //контейнер/конфигурация, который запоминает: -какой VBO привязан
															 // -какой EBO привязан
															// - как интерпретировать данные(glVertexAttribPointer

float vertices_tr[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f,0.5f, 0.0f
};

//данные о позиции и цвете вершин
float vertices[] = {
  0.5f,  0.5f, 0.0f, 0.3f, 0.5f, 0.0f, 
  0.5f, -0.5f, 0.0f, 0.5f, 0.4f, 1.0f,
 -0.5f, -0.5f, 0.0f, 0.2f, 0.8f, 0.0f,
 -0.5f,  0.5f, 0.0f, 0.1f, 0.7f, 1.0f
};

unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};


int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
	if (window == NULL) {
		std::cout << "Error creating window";
		glfwTerminate();
		return -1;
	}	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // функция обратного вызова когда окно расширяется


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "cant initialize GLAD";
		return -1;
	}

	//Создаем шейдерную программу
	Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");


	//Создание оббекта буфера данных VBO, обьекта буфера элементов EBO и обьекта массива вершин VAO в видеопамяти
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //генерируем уникальный идентификатор буфера-обьекта вершин Опенгл и создаем этот обьект
	glGenBuffers(1, &EBO); // генерируем уникальный идентификатор буфера-обьекта индексов Опенгл и создаем этот обьект
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копируем данные вершин из массива оперативной памяти в буфер видеопамяти видеокарты
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// копируем данные индексов из массива оперативной памяти в буфер видеопамяти видеокарты

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);// устанавливаем указатели атрибутов вершин, чтобы сказать опенгл как он должен интерпритировать данные вершины
	glEnableVertexAttribArray(0);// включаем атрибут вершины
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));// устанавливаем указатели атрибутов цвета
	glEnableVertexAttribArray(1);// включаем атрибут цвета

	//отвязываем буферы кроме EBO иначе VAO потеряет доступ к нему
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	while (!glfwWindowShouldClose(window)) {

		processInput(window);	

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //режим рисования каркаса если надо, а это чтобы вернуть в исходный режим рисования - glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).
		
		shader.UseShaderProgram();

		//Uniform значения
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//shader.SetUniformVec4("ourColor", 0.0f, greenValue, 0.0f, 1.0f);

		glBindVertexArray(VAO); //связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //отрисовка 
	
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}