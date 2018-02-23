#version 450

in vec3 FragNormal;
out vec4 FragColor;

void main()
{
	FragColor = vec4(FragNormal, 1.0f);
	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}