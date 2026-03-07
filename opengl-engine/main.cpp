#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h";

#include "Shader.h"


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

//данные о позиции и цвете вершин и координаты текстур
float vertices[] = {

  0.5f,  0.5f, 0.0f,   0.3f, 0.5f, 0.0f,   1.0f, 1.0f,
  0.5f, -0.5f, 0.0f,   0.5f, 0.4f, 1.0f,   1.0f, 0.0f,
 -0.5f, -0.5f, 0.0f,   0.2f, 0.8f, 0.0f,   0.0f, 0.0f,
 -0.5f,  0.5f, 0.0f,   0.1f, 0.7f, 1.0f,   0.0f, 1.0f

};

unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};

int texWidth, texHeight, texnrChannels;
unsigned int texture1;
unsigned int texture2;
unsigned char* texData;


float texCoords[] = {
  0.0f, 0.0f,
  1.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f
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

	stbi_set_flip_vertically_on_load(true);
	texData = stbi_load("textures/container.jpg", &texWidth, &texHeight, &texnrChannels, 0);
	//Создаем текстуру
	glGenTextures(1, &texture1); // функция принимает количество текстур и сохраняет в переменной texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1);//привязка обьекта к типу

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/*
	Первый аргумент указывает цель текстуры; установка значения GL_TEXTURE_2D означает, что эта
	операция сгенерирует текстуру на текущем связанном объекте текстуры с той же целью (поэтому любые
	текстуры, связанные с целями GL_TEXTURE_1D или GL_TEXTURE_3D, не будут затронуты).
	• Второй аргумент указывает уровень мипмапа, для которого мы хотим создать текстуру, если вы хотите
	установить каждый уровень мипмапа вручную, но мы оставим его на базовом уровне, который равен 0.
	• Третий аргумент указывает OpenGL, в каком формате мы хотим сохранить текстуру. Наше изображение
	имеет только значения RGB, поэтому мы также сохраним текстуру со значениями RGB.
	• Четвертый и пятый аргументы задают ширину и высоту результирующей текстуры. Мы сохранили их ранее
	при загрузке изображения, поэтому будем использовать соответствующие переменные.
	• Следующий аргумент всегда должен быть равен 0 (некоторые устаревшие вещи).
	• 7-й и 8-й аргументы определяют формат и тип данных исходного изображения. Мы загрузили изображение со
	значениями RGB и сохранили их в виде символов (байт), поэтому передадим соответствующие значения.
	• Последний аргумент — это фактические данные изображения.
	*/
	
	if (texData) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData); //привязка связанного обьекта текстуры к изображению 
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout<<" Cant load a texture";
	}
	stbi_image_free(texData); //освобождаем память



	texData = stbi_load("textures/awesomeface.png", &texWidth, &texHeight, &texnrChannels, 0);
	//Создаем текстуру
	glGenTextures(1, &texture2); // функция принимает количество текстур и сохраняет в переменной texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture2);//привязка обьекта к типу

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	if (texData) {

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData); //привязка связанного обьекта текстуры к изображению 
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << " Cant load a texture";
	}
	stbi_image_free(texData); //освобождаем память





	//Создание оббекта буфера данных VBO, обьекта буфера элементов EBO и обьекта массива вершин VAO в видеопамяти
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //генерируем уникальный идентификатор буфера-обьекта вершин Опенгл и создаем этот обьект
	glGenBuffers(1, &EBO); // генерируем уникальный идентификатор буфера-обьекта индексов Опенгл и создаем этот обьект
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копируем данные вершин из массива оперативной памяти в буфер видеопамяти видеокарты
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// копируем данные индексов из массива оперативной памяти в буфер видеопамяти видеокарты

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// устанавливаем указатели атрибутов вершин, чтобы сказать опенгл как он должен интерпритировать данные вершины
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));// устанавливаем указатели атрибутов цвета
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));// устанавливаем указатели атрибутов тексур
	
	glEnableVertexAttribArray(0);// включаем атрибут вершины
	glEnableVertexAttribArray(1);// включаем атрибут цвета
	glEnableVertexAttribArray(2);// включаем атрибут текстур

	//отвязываем буферы кроме EBO иначе VAO потеряет доступ к нему
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader.UseShaderProgram();
	shader.SetUniformInt("texture1", 0);
	shader.SetUniformInt("texture2", 1);

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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO); //связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //отрисовка 
	
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}