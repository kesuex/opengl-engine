#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{

	const char* vertexShaderSource;
	const char* fragmentShaderSource;

	unsigned int vertexShader;
	unsigned int fragmentShader;

	std::string vertexShaderString, fragmentShaderString;
	std::ifstream vShaderFile, fShaderFile;
	std::stringstream vShaderStream, fShaderStream;

	//Проверка, что обьекты открыты
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


	try {
		//Открыть файлы
		vShaderFile.open(vertexShaderPath);
		fShaderFile.open(fragmentShaderPath);
		//Прочитать буфер в потоки
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//Закрыть файловые дескрипторы
		vShaderFile.close();
		fShaderFile.close();
		//Преобразовать поток в строку
		vertexShaderString = vShaderStream.str();
		fragmentShaderString = fShaderStream.str();

	}
	catch(std::ifstream::failure){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ";
	}

	//Получаем указатель на строку (сырая строка)
	vertexShaderSource = vertexShaderString.c_str();
	fragmentShaderSource = fragmentShaderString.c_str();

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


}

void Shader::IsCompileVertexShaderSuccessfully(unsigned int vertexShader) {
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		std::cout << "Error shader vertex compilation failed ";
		std::cout << infolog;
	}
}

void Shader::IsCompileFragmentShaderSuccessfully(unsigned int fragmentShader) {
	int success;
	char infolog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		std::cout << "Error shader fragment compilation failed ";
		std::cout << infolog;
	}
}

void Shader::IsLinkingShaderProgramSuccessfully(unsigned int shaderProgram) {
	int success;
	char infolog[512];
	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		std::cout << "Error shader program compilation failed ";
		std::cout << infolog;
	}
}


void Shader::UseShaderProgram() {
	glUseProgram(shaderProgram);//активируем программу шейдеров
}


void Shader::SetUniformVec4(const std::string& name, float x, float y, float z, float w) const {

	glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}


void Shader::SetUniformInt(const std::string& name, int value) const {

	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}


void Shader::SetUniformMatrix4fv(const std::string& name, int matrixCount, glm::mat4 matrix) const {

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), matrixCount, GL_FALSE, glm::value_ptr(matrix));
};
