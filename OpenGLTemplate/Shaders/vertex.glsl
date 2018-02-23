#version 450

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNorm;
out vec3 FragNormal;
void main()
{
	FragNormal = vertexNorm;
	gl_Position = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);
}
