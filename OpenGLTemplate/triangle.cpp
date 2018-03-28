#include "triangle.h"



triangle::triangle(float newScale)
{
	scale = newScale;
	vertices.push_back(scale*vec3(0.5f, 0.5f, 0.0f));
	vertices.push_back(scale*vec3(0.5f, -0.5f, 0.0f));
	vertices.push_back(scale*vec3(-0.5f, 0.5f, 0.0f));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	normal.push_back(vec3(1.0f, 0.0f, 0.0f));
	normal.push_back(vec3(0.0f, 1.0f, 0.0f));
	normal.push_back(vec3(0.0f, 0.0f, 1.0f));
}


triangle::~triangle()
{
}

vector<vec3> triangle::getVertices()
{
	return vertices;
}

vector<vec3> triangle::getNormals()
{
	return normal;
}

vector<unsigned int> triangle::getIndices()
{
	return indices;
}