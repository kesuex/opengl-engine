
#version 330 core
out vec4 FragColor;

//"in vec4 vertexColor;\n" //принимаем переменную для цвета из вертексного шейдера
//"uniform vec4 ourColor;\n" // юниформ переменная - является глобальной, позволяет задавать их значение на любом этапе шейдера, поэтому незачем определять их в вершинном шейдере//если обьявить униформ переменную и не использовать ее компилятор удалит ее без предупреждения из скомпилированной версии
uniform sampler2D texture1;
uniform sampler2D texture2;



in vec3 Normal;  //нормаль текущего пикселя с которым раболтаем и так для каждого пикселя
in vec3 FragPos;
in vec2 TexCoord;

struct Material {
	//vec3 ambient; 
	//vec3 diffuse; 
	//vec3 specular;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light { 
	vec3 position;
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;


void main(){

	//фоновое(ambient) освещение, умножение означает поглощение всех цветовых лучей кроме цвета обьекта, объект отражает только свой цвет, остальные лучи поглощаются
	//vec3 ambient = material.ambient * light.ambient;
	vec3 ambient = vec3(texture(material.diffuse, TexCoord)) * light.ambient;

	//фоновое + рассеянное (ambient + diffuse) освещение
	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);     // Вычитание векторов даёт направление от второго к первому: light.position - FragPos = вектор ОТ фрагмента К свету
	float diff = max(dot(norm, lightDir), 0.0); 
	//vec3 diffuse = (diff * material.diffuse)  * light.diffuse;  
	vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord)))  * light.diffuse; 

	//фоновое + рассеянное + зеркальное (ambient + diffuse + specular) освещение
	vec3 viewDir = normalize(viewPos - FragPos); //вектор от фрагмента к камере
	vec3 reflectDir = reflect(-lightDir, norm); //вектор направления отрадженного луча(для этого инвертируем lightDir чтобы направление было от источника света к фрагменту)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //размер пятна, дот - это насколько камера совпадает с направлением отражённого луча, max отсекает некорректные углы
	vec3 specular = (spec * vec3(texture(material.specular, TexCoord))) * light.specular; // цвет и сила блика, попал ли луч в камеру  
	
	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0);

	//FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); //смешиваем цвет текстуры с цветом вершин
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord), 0.2); //смешиваем две текстуры с помощью тексутрных единиц - 0,2 вернет 80% первого входного цвета и 20% второго входного цвета,

}