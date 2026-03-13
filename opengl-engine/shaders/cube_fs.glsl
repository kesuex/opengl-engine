
#version 330 core
out vec4 FragColor;

//"in vec4 vertexColor;\n" //принимаем переменную для цвета из вертексного шейдера
//"uniform vec4 ourColor;\n" // юниформ переменная - является глобальной, позволяет задавать их значение на любом этапе шейдера, поэтому незачем определять их в вершинном шейдере//если обьявить униформ переменную и не использовать ее компилятор удалит ее без предупреждения из скомпилированной версии

uniform sampler2D texture1;
uniform sampler2D texture2;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){

	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	//vec3 result = ambient * objectColor; //фоновое(ambient) освещение

	vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0); 
	vec3 diffuse = diff * lightColor;
	//vec3 result = (ambient + diffuse) * objectColor; //фоновое + рассеянное (ambient + diffuse) освещение 

	float specularStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos); 
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
	vec3 specular = specularStrength * spec * lightColor;
	vec3 result = (ambient + diffuse + specular) * objectColor; //фоновое + рассеянное + зеркальное (ambient + diffuse + specular) освещение 

	FragColor = vec4(result, 1.0);
	//FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); //смешиваем цвет текстуры с цветом вершин
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord), 0.2); //смешиваем две текстуры с помощью тексутрных единиц - 0,2 вернет 80% первого входного цвета и 20% второго входного цвета,

}