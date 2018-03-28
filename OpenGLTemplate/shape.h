#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
class shape
{
public:
	shape();
	~shape();
	void virtual create(float newScale) = 0;
	vector<vec3> virtual getVertices() = 0;
	vector<vec3> virtual getNormals() = 0;
	vector<unsigned int> virtual getIndices() = 0;
};

