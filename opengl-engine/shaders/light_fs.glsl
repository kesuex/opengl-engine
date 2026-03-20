
#version 330 core
out vec4 FragColor;

//"uniform vec4 ourColor;\n" // юниформ переменная - является глобальной, позволяет задавать их значение на любом этапе шейдера, поэтому незачем определять их в вершинном шейдере//если обьявить униформ переменную и не использовать ее компилятор удалит ее без предупреждения из скомпилированной версии

uniform sampler2D texture1;
uniform sampler2D texture2;



in vec3 Normal;  //нормаль текущего пикселя с которым раболтаем и так для каждого пикселя
in vec3 FragPos;
in vec2 TexCoord;

struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct PointLight { 
	vec3 position;
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirectLight { 
	vec3 direction;
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;
};

struct SpotLight { 
	vec3 position;
	vec3 direction;
	vec3 ambient; 
	vec3 diffuse; 
	vec3 specular;

	float constant;
	float linear;
	float quadratic;	
	float cutOff;
	float outerCutOff;
};

#define NR_POINT_LIGHTS 4
uniform PointLight pointlights[NR_POINT_LIGHTS];

uniform DirectLight directlight;
uniform SpotLight spotlight;

uniform Material material;
uniform vec3 viewPos;


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

	//фоновое(ambient) освещение, умножение означает поглощение всех цветовых лучей кроме цвета обьекта, объект отражает только свой цвет, остальные лучи поглощаются
	vec3 ambient = vec3(texture(material.texture_diffuse1, TexCoord)) * light.ambient;

	//фоновое + рассеянное (ambient + diffuse) освещение
    vec3 lightDir = normalize(light.position - fragPos);     // Вычитание векторов даёт направление от второго к первому: light.position - FragPos = вектор ОТ фрагмента К свету
	float diff = max(dot(normal, lightDir), 0.0);   
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse1, TexCoord)))  * light.diffuse; 

	//фоновое + рассеянное + зеркальное (ambient + diffuse + specular) освещение
	vec3 reflectDir = reflect(-lightDir, normal); //вектор направления отрадженного луча(для этого инвертируем lightDir чтобы направление было от источника света к фрагменту)
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); //размер пятна, дот - это насколько камера совпадает с направлением отражённого луча, max отсекает некорректные углы
	vec3 specular = (spec * vec3(texture(material.texture_specular1, TexCoord))) * light.specular; // цвет и сила блика, попал ли луч в камеру  
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;

	return result;

}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	
	vec3 ambient = vec3(texture(material.texture_diffuse1, TexCoord)) * light.ambient;

    vec3 lightDir = normalize(light.position - fragPos);    
	float diff = max(dot(normal, lightDir), 0.0);   
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse1, TexCoord)))  * light.diffuse; 

	vec3 reflectDir = reflect(-lightDir, normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 
	vec3 specular = (spec * vec3(texture(material.texture_specular1, TexCoord))) * light.specular;   
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float theta = dot(lightDir, normalize(-light.direction)); // theta — косинус угла между направлением прожектора и направлением к фрагменту
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	diffuse *= intensity;
	specular *= intensity;

	vec3 result = ambient + diffuse + specular;

	return result;
}


vec3 CalcDirectLight(DirectLight light, vec3 normal, vec3 viewDir) {

	vec3 ambient = vec3(texture(material.texture_diffuse1, TexCoord)) * light.ambient;

    vec3 lightDir = normalize(-light.direction);     
	float diff = max(dot(normal, lightDir), 0.0);   
	vec3 diffuse = (diff * vec3(texture(material.texture_diffuse1, TexCoord)))  * light.diffuse; 
 
	vec3 reflectDir = reflect(-lightDir, normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = (spec * vec3(texture(material.texture_specular1, TexCoord))) * light.specular;   
	
	vec3 result = ambient + diffuse + specular;

	return result;

}



void main(){

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos); //вектор от фрагмента к камере

	vec3 result =  CalcDirectLight(directlight, norm, viewDir);

	for(int i = 0; i < NR_POINT_LIGHTS; ++i) {
		
		result += CalcPointLight(pointlights[i], norm, FragPos, viewDir);
	
	}

	result += CalcSpotLight(spotlight, norm, FragPos, viewDir);
	FragColor = vec4(result, 1.0);

	//FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); //смешиваем цвет текстуры с цветом вершин
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord), 0.2); //смешиваем две текстуры с помощью тексутрных единиц - 0,2 вернет 80% первого входного цвета и 20% второго входного цвета,

}