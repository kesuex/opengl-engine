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

void IsCompileVertexShaderSuccessfully(unsigned int vertexShader) {
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		std::cout << "Error shader vertex compilation failed "; 
		std::cout << infolog;
	}
}

void IsCompileFragmentShaderSuccessfully(unsigned int fragmentShader) {
	int success;
	char infolog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		std::cout << "Error shader fragment compilation failed ";
		std::cout << infolog;
	}
}

void IsLinkingShaderProgramSuccessfully(unsigned int shaderProgram) {
	int success;
	char infolog[512];
	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		std::cout << "Error shader program compilation failed ";
		std::cout << infolog;
	}
}

unsigned int WIDTH = 800;
unsigned int HEIGHT = 600;
unsigned int VBO;
unsigned int EBO;
unsigned int VAO;
unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int shaderProgram;

float vertices_tr[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f,0.5f, 0.0f
};

float vertices[] = {
  0.5f,  0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
 -0.5f, -0.5f, 0.0f,
 -0.5f,  0.5f, 0.0f
};

unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3
};


const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}";



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

	
	//Создание вершинного шейдера
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//создаем обьект шейдера вместе с айди
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // прикрепляем код шейдера, к обьекту шейдера
	glCompileShader(vertexShader); // компилируем шейдер
	IsCompileVertexShaderSuccessfully(vertexShader);// првоерка компиляции

	//Создание фрагментного шейдера
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	IsCompileFragmentShaderSuccessfully(fragmentShader);

	//Создание шейдерной программы
	shaderProgram = glCreateProgram(); //создает программу и возвращает ссылку ID на созданный объект программы
	glAttachShader(shaderProgram, vertexShader); //присоединяем шейдеры к обьекту шейдерной программы 
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); // связываем шейдеры
	IsLinkingShaderProgramSuccessfully(shaderProgram);

	//Удаляем обьекты шейдеров после связывания с программой
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	
	//Создание оббекта буфера данных VBO, обьекта буфера элементов EBO и обьекта массива вершин VAO в видеопамяти
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO); //генерируем уникальный идентификатор буфера-обьекта вершин Опенгл и создаем этот обьект
	glGenBuffers(1, &EBO); // генерируем уникальный идентификатор буфера-обьекта индексов Опенгл и создаем этот обьект
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копируем данные вершин из массива оперативной памяти в буфер видеопамяти видеокарты
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// копируем данные индексов из массива оперативной памяти в буфер видеопамяти видеокарты

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);// устанавливаем указатели атрибутов вершин, чтобы сказать опенгл как он должен интерпритировать данные вершины
	glEnableVertexAttribArray(0);// включаем атрибут вершины

	//отвязываем буферы кроме EBO иначе VAO потеряет доступ к нему
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	while (!glfwWindowShouldClose(window)) {

		processInput(window);	

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //режим рисования каркаса если надо, а это чтобы вернуть в исходный режим рисования - glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).
		glUseProgram(shaderProgram);//активируем программу шейдеров
		glBindVertexArray(VAO); // связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	

		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}