#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h";

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

/*
OpenGL ожидает, что все вершины, которые мы хотим сделать видимыми, будут
находиться в нормализованных координатах устройства после каждого запуска шейдера вершин. То есть координаты x,
y и z каждой вершины должны находиться в диапазоне от -1,0 до 1,0; координаты за пределами этого
диапазона не будут видны.

Всего существует 5 различных систем координат:
• Локальное пространство (или пространство объекта)
• Мировое пространство
• Пространство просмотра (или пространство глаза)
• Пространство клипа
• Пространство экрана

Все это различные состояния, в которые будут преобразованы наши вершины, прежде чем они окончательно
превратятся в фрагменты.

Сложение и вычитание определены только для матриц одинаковых
размеров. Матрица 3x2 и матрица 2x3 (или матрица 3x3 и матрица 4x4) не могут быть сложены или вычтены друг из
друга.
Можно умножать две матрицы только в том случае, если количество столбцов в матрице слева равно
количеству строк в матрице справа.
2. Умножение матриц не является коммутативным, то есть A · B ≠ B · A.

Вектор — это, по сути, матрица Nx1, где N — количество компонентов вектора (также известная как N-мерный вектор). 
Векторы, как и матрицы, представляют собой массив чисел, но только с одной колонкой. 
Если у нас есть матрица MxN, мы можем умножить эту матрицу на наш вектор Nx1,
поскольку колонки матрицы равны количеству строк вектора, таким образом определяется умножение матриц.
Существует множество интересных 2D/3Dпреобразований, которые можно поместить в матрицу, 
и умножение этой матрицы на вектор преобразует этот вектор.

В OpenGL мы обычно работаем с матрицами преобразования 4x4 по нескольким причинам, и одна из них
заключается в том, что большинство векторов имеют размер 4. Самая простая матрица преобразования, которую мы
можем себе представить, — это единичная матрица. Единичная матрица — это матрица NxN, в которой все
элементы, кроме диагональных, равны 0.
Кватернионы избавляют от гимбал локов

При умножении матриц сначала умножается крайняя правая матрица на вектор, поэтому умножение следует читать справа налево. При
комбинировании матриц рекомендуется сначала выполнять операции масштабирования, затем повороты и, наконец,
переносы, иначе они могут (негативно) влиять друг на друга.


//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //отрисовка с помощью массива индексов
//glDrawArrays(GL_TRIANGLES, 0, 36); //этот метод используется для отрисовки когда у нас только массив вершин и нет массива индексов
glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //режим рисования каркаса если надо, а это чтобы вернуть в исходный режим рисования - glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).

 OpenGL всегда работает с двумя буферами:

  Front Buffer → то что видит пользователь на экране
  Back Buffer  → то куда ты сейчас рисуешь

  Пока пользователь смотрит на front buffer — ты последовательно рисуешь всё в back buffer. Пользователь видит только результат 
  swapBuffers в конце цикла рендеринга — в этот момент в буфере уже всё нарисовано. Промежуточных состояний он не видит никогда.
  Все обьекты последовательно рисуются, одновременно показывается.

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  //                     ↑  ↑     ↑         ↑              ↑               ↑
  //                     1  2     3         4              5               6

  1 — index (0)
  Номер атрибута. В шейдере: layout (location = 0) in vec3 aPos; — должен совпадать.

  2 — size (3)
  Сколько компонентов у атрибута. Позиция = xyz = 3. UV = 2. Цвет = 3.

  3 — type (GL_FLOAT)
  Тип данных одного компонента.

  4 — normalized (GL_FALSE)
  Нужно ли нормализовать целочисленные значения в диапазон 0..1. Для float всегда GL_FALSE.

  5 — stride (8 * sizeof(float))
  Сколько байт от начала одной вершины до начала следующей. У тебя каждая вершина: 3 позиция + 3 цвет + 2 UV = 8 floats.

  6 — offset ((void*)0)
  С какого байта внутри вершины начинается этот атрибут:
  позиция: (void*)0                    ← с самого начала
  цвет:    (void*)(3 * sizeof(float))  ← после 3 floats позиции
  UV:      (void*)(6 * sizeof(float))  ← после 3+3 floats

glEnableVertexAttribArray(0);
Смысл в том что иногда нужно временно отключить атрибут не удаляя его настройки из VAO. Например лампочке не нужны
нормали — можно отключить этот атрибут не трогая конфигурацию.

VBO — VAO запоминает не сам VBO, а настройку glVertexAttribPointer.После того как настройка записана, VBO можно
отвязать — VAO уже знает что нужно.
EBO — VAO запоминает саму привязку GL_ELEMENT_ARRAY_BUFFER напрямую.Если отвязать EBO пока VAO активен — VAO потеряет
ссылку на него и glDrawElements сломается.
Кратко: VBO хранится через атрибут, EBO хранится напрямую в VAO.

Обращение матриц — это дорогостоящая операция для шейдеров, поэтому по возможности старайтесь
избегать обратных операций, поскольку они должны выполняться для каждой вершины вашей сцены.
Для обучения это нормально, но для эффективного приложения вам, вероятно, захочется вычислить
матрицу нормалей на CPU и отправить ее в шейдеры через униформ перед рисованием (так же, как
матрицу модели).
Освещение работает корректно, если не масштабировать объект, поэтому нет необходимости использовать матрицу нормалей, 
и можно умножить нормали на матрицу модели.
Если использовать неравномерное масштабирование, необходимо умножить векторы нормалей на
матрицу нормалей, иначе нормаль перестает быть перпендикулярной и свет падает неправильно искаженно.

Мы решили выполнять вычисления освещения в мировом пространстве, но большинство людей
предпочитают выполнять освещение в пространстве просмотра. Преимущество пространства просмотра
заключается в том, что положение зрителя всегда находится в точке (0,0,0), поэтому вы уже
получаете положение зрителя бесплатно. Однако я считаю, что расчет освещения в пространстве мира
более интуитивен для целей обучения. Если вы все же хотите рассчитывать освещение в пространстве
просмотра, вам необходимо также преобразовать все соответствующие векторы с помощью матрицы
просмотра (не забудьте изменить и матрицу нормали).

*/


Camera camera;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height); //задаем размер отображения для опенгл в нашем окне GLFW
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	camera.ProcessMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessScrolling(xoffset, yoffset);
}

unsigned int LoadTexture(const char* path)
{
	unsigned int textureID;
	unsigned char* textureData;
	int textureWidth, textureHeight, texturenrChannels;
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

	textureData = stbi_load(path, &textureWidth, &textureHeight, &texturenrChannels, 0);
	//Создаем текстуру
	glGenTextures(1, &textureID); // функция принимает количество текстур и сохраняет в переменной texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);//привязка обьекта к типу

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureData) {
		//GL_RGBA или GL_RGB  - параметр влияющий на формат пнг или джейпег 
		GLenum GLformat;
		if (texturenrChannels == 4) GLformat = GL_RGBA;
		else GLformat = GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GLformat, GL_UNSIGNED_BYTE, textureData); //привязка связанного обьекта текстуры к изображению 
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << " Cant load a texture";
	}
	stbi_image_free(textureData); //освобождаем память

	return textureID;
}


unsigned int scrWIDTH = 800;
unsigned int scrHEIGHT = 600;

glm::mat4 projection;
glm::mat4 view;

unsigned int diffuseMap;
unsigned int specularMap;

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);



//данные о позиции и цвете вершин и координаты текстур
float arr_vertices[] = {
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


std::vector<unsigned int> indices = {
  0, 1, 2,     2, 3, 0,
  4, 5, 6,     6, 7, 4,
  8, 9, 10,    10, 11, 8,
  12, 13, 14,  14, 15, 12,
  16, 17, 18,  18, 19, 16,
  20, 21, 22,  22, 23, 20
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,   0.0f,   0.0f),
	glm::vec3(2.0f,   5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f,  -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f,  -0.4f,  -3.5f),
	glm::vec3(-1.7f,  3.0f,  -7.5f),
	glm::vec3(1.3f,  -2.0f,  -2.5f),
	glm::vec3(1.5f,   2.0f,  -2.5f),
	glm::vec3(1.5f,   0.2f,  -1.5f),
	glm::vec3(-1.3f,  1.0f,  -1.5f)
};

glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f, 0.2f, 2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f, 2.0f, -12.0f),
	glm::vec3(0.0f, 0.0f, -3.0f)
};




int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(scrWIDTH, scrHEIGHT, "Window", NULL, NULL);
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
	Shader objShader("shaders/cube_vs.glsl", "shaders/cube_fs.glsl");
	objShader.UseShaderProgram();

	for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); ++i) {

		std::string base = "pointlights[" + std::to_string(i) + "].";
		objShader.SetUniformVec3fv((base + "position").c_str(), 1, pointLightPositions[i]);
		objShader.SetUniformVec3fv((base + "ambient").c_str(), 1, glm::vec3(0.2f, 0.2f, 0.2f));
		objShader.SetUniformVec3fv((base + "diffuse").c_str(), 1, glm::vec3(0.5f, 0.5f, 0.5f));
		objShader.SetUniformVec3fv((base + "specular").c_str(), 1, glm::vec3(1.0f, 1.0f, 1.0f));
		objShader.SetUniformFloat((base + "constant").c_str(), 1.0f);
		objShader.SetUniformFloat((base + "linear").c_str(), 0.09f);
		objShader.SetUniformFloat((base + "quadratic").c_str(), 0.032f);
	
	}
	

	objShader.SetUniformVec3fv("directlight.direction", 1, glm::vec3(-0.2f, -1.0f, -0.3f));
	objShader.SetUniformVec3fv("directlight.ambient", 1, glm::vec3(0.05f, 0.05f, 0.05f));
	objShader.SetUniformVec3fv("directlight.diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f));
	objShader.SetUniformVec3fv("directlight.specular", 1, glm::vec3(1.0f, 1.0f, 1.0f));

	objShader.SetUniformVec3fv("spotlight.ambient", 1, glm::vec3(0.2f, 0.2f, 0.2f));
	objShader.SetUniformVec3fv("spotlight.diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f));
	objShader.SetUniformVec3fv("spotlight.specular", 1, glm::vec3(1.0f, 1.0f, 1.0f));
	objShader.SetUniformFloat("spotlight.constant", 1.0f);
	objShader.SetUniformFloat("spotlight.linear", 0.09f);
	objShader.SetUniformFloat("spotlight.quadratic", 0.032f);

	objShader.SetUniformInt("material.texture_diffuse1", 0);
	objShader.SetUniformInt("material.texture_specular1", 1); //отправляем текстуры в фрагментный шейдер
	objShader.SetUniformFloat("material.shininess", 32.0f);

	//glm::mat4 model = glm::mat4(1.0f); //инициализация единичной матрицы
	//model = glm::translate(model, objPos);
	//objShader.SetUniformMatrix4fv("model", 1, model);

	Shader lightShader("shaders/cube_vs.glsl", "shaders/light_fs.glsl");
	lightShader.UseShaderProgram();
	lightShader.SetUniformVec3fv("lightColor", 1, lightColor);


	std::vector<Vertex> vertices;

	for(int i = 0; i <= (sizeof(arr_vertices)/sizeof(float)) - 8; ++i) {
		Vertex vertex;
		vertex.Position.x = arr_vertices[i];
		vertex.Position.y = arr_vertices[i + 1];
		vertex.Position.z = arr_vertices[i + 2];
		vertex.Normal.x = arr_vertices[i + 3];
		vertex.Normal.y = arr_vertices[i + 4];
		vertex.Normal.z = arr_vertices[i + 5];
		vertex.TexCoords.x = arr_vertices[i + 6];
		vertex.TexCoords.y = arr_vertices[i + 7];

		vertices.push_back(vertex);
	}

	stbi_set_flip_vertically_on_load(true);

	std::vector<Texture> textures;

	Texture diffuseTex;
	diffuseMap = LoadTexture("textures/container2.png");
	diffuseTex.id = diffuseMap;
	diffuseTex.type = "texture_diffuse";
	textures.push_back(diffuseTex);

	Texture specularTex;
	specularMap = LoadTexture("textures/container2_specular.png");
	specularTex.id = specularMap;
	specularTex.type = "texture_specular";
	textures.push_back(specularTex);


	Mesh cube(vertices, indices, textures);
	Mesh lighcube(vertices, indices, {});
	

	glEnable(GL_DEPTH_TEST); // включаем буфер глубины чтобы передние и задние обьекты не перезаписывали друг друга
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //настройка, не колбек. Говорит GLFW спрятать и заблокировать курсор.
	glfwSetCursorPosCallback(window, mouse_callback); // регистрация функций-обработчиков. Вы говорите GLFW: "когда мышь
	glfwSetScrollCallback(window, scroll_callback);	  // двигается — вызови вот эту функцию". Регистрируется тоже один раз.
	

	while (!glfwWindowShouldClose(window)) {	

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очищаем буферы глубины и цвета, иначе предыдущая информация от предыдщего кадра останется в буфере
		
		camera.processInput(window);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		objShader.UseShaderProgram();
		objShader.SetUniformMatrix4fv("view", 1, view);
		objShader.SetUniformMatrix4fv("projection", 1, projection);
		objShader.SetUniformVec3fv("viewPos", 1, camera.cameraPosition);
		objShader.SetUniformVec3fv("spotlight.position", 1, camera.cameraPosition);
		objShader.SetUniformVec3fv("spotlight.direction", 1, camera.cameraFront);
		objShader.SetUniformFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
		objShader.SetUniformFloat("spotlight.outerCutOff", glm::cos(glm::radians(17.5f)));
		
		
		for (int i = 0; i < sizeof(cubePositions) / sizeof(glm::vec3); ++i) {

			float angle = 20.0f * i;

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));
			objShader.SetUniformMatrix4fv("model", 1, model);
			cube.Draw(objShader);
		}


		lightShader.UseShaderProgram();
		lightShader.SetUniformMatrix4fv("view", 1, view);
		lightShader.SetUniformMatrix4fv("projection", 1, projection);

		
		for (int i = 0; i < 4; ++i) {

			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.4f));
			lightShader.SetUniformMatrix4fv("model", 1, lightModel);
			lighcube.Draw(lightShader);
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}