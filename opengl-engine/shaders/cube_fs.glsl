
#version 330 core
out vec4 FragColor;

//"in vec4 vertexColor;\n" //принимаем переменную для цвета из вертексного шейдера
//"uniform vec4 ourColor;\n" // юниформ переменная - является глобальной, позволяет задавать их значение на любом этапе шейдера, поэтому незачем определять их в вершинном шейдере//если обьявить униформ переменную и не использовать ее компилятор удалит ее без предупреждения из скомпилированной версии

//in vec3 ourColor;
//in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 lightColor;
uniform vec3 objectColor;

void main(){

	FragColor = vec4(lightColor * objectColor, 1.0);
	//FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0); //смешиваем цвет текстуры с цветом вершин
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2,TexCoord), 0.2); //смешиваем две текстуры с помощью тексутрных единиц - 0,2 вернет 80% первого входного цвета и 20% второго входного цвета,

}