#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h";

#include "Model.h"
#include "Primitive.h"
#include "PrimitiveData.h"
#include "Light.h"
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

Важно помнить, что значения в буфере глубины не являются линейными в пространстве клипа (они линейны в пространстве просмотра
до применения матрицы проекции). Значение 0,5 в буфере глубины не означает, что значение z пикселя находится
на полпути в конусе обзора; значение z вершины на самом деле довольно близко к ближней плоскости!

Для каждого пикселя GPU проходит по цепочке:
фрагментный шейдер вычислил цвет
Stencil тест — проверяем ячейку stencil буфера, проходит или нет
Depth тест — проверяем ячейку depth буфера, этот фрагмент ближе к камере или нет
Если оба теста прошли — записываем цвет в color buffer и обновляем depth и stencil буферы
Если не прошли — фрагмент отбрасывается, пиксель не перезаписывается

фрагмент
	→ stencil тест (glStencilFunc) — пропустить или отбросить?
		→ depth тест (glDepthFunc) — пропустить или отбросить?
			→ glStencilOp — что хотим записать на основании результатов тестов?
				→ glStencilMask — разрешить или заблокировать запись?
					→ stencil буфер

Стенсил буфер - резервируем пиксели на экране с целью недопущения рисования других обьектов в этих местах экрана
Депс буфер - проверяет значение на оснвоании глубины простанства экрана и решает какие значения отрисовывать
Оба буфера хранят в себе информацию о пикселях экрана и записывают в себя фрагменты которые мы можем определить
Депс и стенсил буфер работают постоянно когда активируем их, мы только можем управлять значениями которые идут в них и отрисовываются
glEnable(GL_STENCIL_TEST); - активируем
glEnable(GL_DEPTH_TEST); - активируем
glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); - глобальное состояние, злдесь происходлит запись в стенсил буфер
sfail: действие, которое следует предпринять, если тест трафарета не прошел.
dpfail : действие, которое следует предпринять, если тест трафарета прошел успешно, но тест глубины провалился.
dppass : действие, которое необходимо выполнить, если тест трафарета и тест на глубину прошли успешно.

glStencilFunc(GL_NOTEQUAL, 1, 0xFF); - для каждого пикселя проводим тестирование, обращаясь к пикселям в стенсил буфере, если там значение 0 то тест успешен - отрисовываем
glStencilMask(0x00); - маска которая применяется к значениям которые прошли тетсирование для записи финального значения в буфер (в данном случае обнуляем значения - запрет записи)
glStencilOp - в итоге сюда попадают либо не попадают значения на основании тестирования и применения маски
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

glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

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

	//Шейдер со светом
	Shader objShader("shaders/model_vs.glsl", "shaders/light_fs.glsl");
	objShader.UseShaderProgram();
	PointLight pointlight;
	int positionArrSize = sizeof(pointLightPositions) / sizeof(glm::vec3);
	pointlight.ApplyUniformArrayInit(objShader,"pointlights", positionArrSize, pointLightPositions);
	DirectLight directlight;
	directlight.ApplyUniformInit(objShader);
	SpotLight spotlight;
	spotlight.ApplyUniformInit(objShader);

	//Шейдер с простым цветом
	Shader colorShader("shaders/model_vs.glsl", "shaders/color_fs.glsl");
	colorShader.UseShaderProgram();
	colorShader.SetUniformVec3fv("lightColor", 1, lightColor);

	stbi_set_flip_vertically_on_load(true);
	
	Primitive lighCube(PrimitiveData::Cube::vertices, PrimitiveData::Cube::size, PrimitiveData::Cube::indices, "", "");
	Primitive depthCube(PrimitiveData::Cube::vertices, PrimitiveData::Cube::size, PrimitiveData::Cube::indices, "textures/marble.jpg", "textures/marble.jpg");
	Primitive depthPlane(PrimitiveData::Plane::vertices, PrimitiveData::Plane::size, PrimitiveData::Plane::indices, "textures/metal.png", "textures/metal.png" );
	Model model("models/backpack/backpack.obj");
	Phong phong;
	
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //настройка, не колбек. Говорит GLFW спрятать и заблокировать курсор.
	glfwSetCursorPosCallback(window, mouse_callback); // регистрация функций-обработчиков. Вы говорите GLFW: "когда мышь
	glfwSetScrollCallback(window, scroll_callback);	  // двигается — вызови вот эту функцию". Регистрируется тоже один раз.

	glEnable(GL_STENCIL_TEST); // включаем буфер трафарета чтобы отбрасывать фрагменты до передачи в буфер глубины
	glEnable(GL_DEPTH_TEST); // включаем буфер глубины чтобы передние и задние обьекты не перезаписывали друг друга
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);//  GL_REPLACE — записать значение ref из glStencilFunc в буфер, в этой функции происходит запись в трафарет
	//glDepthFunc(GL_ALWAYS);//стандартная отрисовка (обьекты накладываются друг на друга)
	glDepthFunc(GL_LESS);//корректная отрисвока передних и задних обьектов

	while (!glfwWindowShouldClose(window)) {
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		camera.processInput(window);
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // очищаем буферы глубины, трафарета и цвета, иначе предыдущая информация от предыдщего кадра останется в буфере

		objShader.UseShaderProgram();
		camera.ApplyUniformsView(objShader, 800.0f, 600.0f);
		spotlight.ApplyUniformRunTime(objShader, camera);

		depthPlane.transform.ApplyUniform(objShader, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		depthPlane.Draw(objShader, phong);

		depthCube.outlined = true;
		depthCube.transform.ApplyUniform(objShader, glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		depthCube.Draw(objShader, phong);
		depthCube.transform.ApplyUniform(objShader, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
		depthCube.Draw(objShader, phong);
		depthCube.outlined = false;

		colorShader.UseShaderProgram();
		camera.ApplyUniformsView(colorShader, 800.0f, 600.0f);

		depthCube.transform.ApplyUniform(colorShader, glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec3(1.07f));
		depthCube.DrawOutline(colorShader, phong);
		depthCube.transform.ApplyUniform(colorShader, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.07f));
		depthCube.DrawOutline(colorShader, phong);
		
		

		//model.transform.Position = cubePositions[4];
		//model.transform.ApplyUniform(objShader, model.transform);
		//model.Draw(objShader, phong);


		/*
		for (int i = 0; i < 4; ++i) {

			lighCube.transform.Position = pointLightPositions[i];
			lighCube.transform.Scale = glm::vec3(0.4f);
			lighCube.transform.ApplyUniform(lightShader, lighCube.transform);
			lighCube.Draw(lightShader, phong);
			
		}
		*/

		glfwSwapBuffers(window);
		glfwPollEvents();
		
	}

	glfwTerminate();
	return 0;

}