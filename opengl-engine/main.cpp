#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h";

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


unsigned int scrWIDTH = 800;
unsigned int scrHEIGHT = 600;


glm::mat4 projection;
glm::mat4 view;


unsigned int VBO;	
unsigned int EBO; 						
unsigned int VAO; 

unsigned int lightVAO;



//данные о позиции и цвете вершин и координаты текстур
float vertices[] = {
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


unsigned int indices[] = {
  0, 1, 2,     2, 3, 0,
  4, 5, 6,     6, 7, 4,
  8, 9, 10,    10, 11, 8,
  12, 13, 14,  14, 15, 12,
  16, 17, 18,  18, 19, 16,
  20, 21, 22,  22, 23, 20
};



int texWidth, texHeight, texnrChannels;
unsigned int texture1;
unsigned int texture2;
unsigned char* texData;



glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 objPos(0.0f, 0.0f, -3.0f);



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
	objShader.SetUniformVec3fv("light.position", 1, lightPos);
	objShader.SetUniformVec3fv("light.ambient", 1, glm::vec3(0.2f, 0.2f, 0.2f));
	objShader.SetUniformVec3fv("light.diffuse", 1, glm::vec3(0.5f, 0.5f, 0.5f));
	objShader.SetUniformVec3fv("light.specular", 1, glm::vec3(1.0f, 1.0f, 1.0f));
	
	objShader.SetUniformVec3fv("material.ambient", 1, glm::vec3(1.0f, 0.5f, 0.31f));
	objShader.SetUniformVec3fv("material.diffuse", 1, glm::vec3(1.0f, 0.5f, 0.31f));
	objShader.SetUniformVec3fv("material.specular", 1, glm::vec3(0.5f, 0.5f, 0.5f));
	objShader.SetUniformFloat("material.shininess", 32.0f);

	glm::mat4 model = glm::mat4(1.0f); //инициализация единичной матрицы
	model = glm::translate(model, objPos);
	objShader.SetUniformMatrix4fv("model", 1, model);


	Shader lightShader("shaders/cube_vs.glsl", "shaders/light_fs.glsl");
	lightShader.UseShaderProgram();
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightShader.SetUniformVec3fv("lightColor", 1, lightColor);
	lightModel = glm::translate(lightModel, lightPos);
	lightShader.SetUniformMatrix4fv("model", 1, lightModel);
	

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
	
	//objShader.SetUniformInt("texture1", 0); //отправляем текстуры в фрагментный шейдер
	//objShader.SetUniformInt("texture2", 1);



	//Создание объекта буфера  VAO, VBO, EBO в видеопамяти
	glGenVertexArrays(1, &VAO); //генерируем уникальный идентификатор конфигурации атрибутов VAO и создаем этот обьект
	glGenBuffers(1, &VBO); //генерируем уникальный идентификатор буфера-обьекта вершин VBO и создаем этот обьект
	glGenBuffers(1, &EBO); // генерируем уникальный идентификатор буфера-обьекта индексов EBO и создаем этот обьект

	//Активациия VAO и связывание буферов VBO, EBO
	glBindVertexArray(VAO); // активируем обьект VAO для дальнейшей настройки
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем созданныё буфферный обьект с типом буфера вершин - GL_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // связываем созданныё буфферный обьект с типом буфера индексов - GL_ELEMENT_ARRAY_BUFFER, запись в VAO

	//Копирование данных в GPU память
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // копируем данные вершин из массива оперативной памяти в буфер видеопамяти видеокарты
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);// копируем данные индексов из массива оперативной памяти в буфер видеопамяти видеокарты
	
	//Записываем конфигурацию атрибута GL_ARRAY_BUFFER (к которому привязан текущий VBO) в активный VAO, после этого можно отвязать VBO - glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// устанавливаем указатели атрибутов вершин
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));// устанавливаем указатели атрибутов цвета
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));// устанавливаем указатели атрибутов тексур
	
	//Включаем атрибуты
	glEnableVertexAttribArray(0);// включаем атрибут вершины
	glEnableVertexAttribArray(1);// включаем атрибут цвета
	//glEnableVertexAttribArray(2);// включаем атрибут текстур
	
	//Деактивация VAO куба
	glBindVertexArray(0);

	//Создание буфера отрисовки для светового обьекта используя той же формы куба
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);// устанавливаем указатели атрибутов вершин
	glEnableVertexAttribArray(0);// включаем атрибут вершины
	
	//Деактивация VAO светового куба и отвязывание общего буфера VBO. EBO отвязывать не нужно. Если отвязывать то только после деактивации VAO, иначе VAO потеряет доступ к нему
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	/*
	glm::mat4 trans = glm::mat4(1.0f); //инициализация единичной матрицы
	trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)); //2 шаг - создание матрицы преобразования где мы передаем единичную матрицу и вектор оси поворота по z и угла поворота в радианах
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5)); //1 шаг - создание матрицы преобразования где мы передаем единичную матрицу и вектор масштабирования
	glm::mat4 savedTrans = trans;
	shader.SetUniformMatrix4fv("transform", 1, trans); //отправляем матрицу в вершинны шейдер
	 // перемещаем сцену в обратном направлении //view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	*/

  				

	glEnable(GL_DEPTH_TEST); // включаем буфер глубины чтобы передние и задние обьекты не перезаписывали друг друга
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //настройка, не колбек. Говорит GLFW спрятать и заблокировать курсор.
	glfwSetCursorPosCallback(window, mouse_callback); // регистрация функций-обработчиков. Вы говорите GLFW: "когда мышь
	glfwSetScrollCallback(window, scroll_callback);	  // двигается — вызови вот эту функцию". Регистрируется тоже один раз.
	

	while (!glfwWindowShouldClose(window)) {	

		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очищаем буферы глубины и цвета, иначе предыдущая информация от предыдщего кадра останется в буфере
		
		camera.processInput(window);

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		objShader.UseShaderProgram();
		objShader.SetUniformMatrix4fv("view", 1, view);
		objShader.SetUniformMatrix4fv("projection", 1, projection);
		objShader.SetUniformVec3fv("viewPos", 1, camera.cameraPosition);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		glBindVertexArray(VAO); //связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);// 36 - количесвто индексов
		glBindVertexArray(0);

		lightShader.UseShaderProgram();
		lightShader.SetUniformMatrix4fv("view", 1, view);
		lightShader.SetUniformMatrix4fv("projection", 1, projection);

		glBindVertexArray(lightVAO); //связывание с VAO автоматически связываает EBO, всегда отвязываем EBO  после отвязки VAO
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);// 36 - количесвто индексов

		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}