#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
class triangle
{
public:
	triangle(float newScale);
	~triangle();
	vector<vec3> getVertices();
	vector<vec3> getNormals();
	vector<unsigned int> getIndices();
private:
	vector<vec3> vertices, normal;
	vector<unsigned int> indices;
	float scale = 1.0f;
};

