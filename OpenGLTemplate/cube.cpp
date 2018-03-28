#include "cube.h"



cube::cube(float newScale)
{
	scale = newScale;
	/* points for generating the cube*/
	vertices.push_back(scale*vec3(1.0f, 1.0f, 1.f)); //0
	vertices.push_back(scale*vec3(1.0f, -1.0f, 1.f)); //1
	vertices.push_back(scale*vec3(-1.0f, -1.0f, 1.f)); //2
	vertices.push_back(scale*vec3(-1.0f, 1.0f, 1.f)); //3
	vertices.push_back(scale*vec3(-1.0f, -1.0f, -1.f)); //4
	vertices.push_back(scale*vec3(-1.0f, 1.0f, -1.f)); //5
	vertices.push_back(scale*vec3(1.0f, 1.0f, -1.f)); //6
	vertices.push_back(scale*vec3(1.0f, -1.0f, -1.f)); //7


	/* front of cube*/
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(3);

	/* left side of cube*/
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(4);

	indices.push_back(3);
	indices.push_back(4);
	indices.push_back(5);

	/* back of cube*/
	indices.push_back(6);
	indices.push_back(7);
	indices.push_back(4);

	indices.push_back(4);
	indices.push_back(5);
	indices.push_back(6);

	/* right side of cube*/
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(7);

	indices.push_back(0);
	indices.push_back(7);
	indices.push_back(6);

	/* top of cube*/
	indices.push_back(0);
	indices.push_back(3);
	indices.push_back(5);

	indices.push_back(0);
	indices.push_back(5);
	indices.push_back(6);

	/* bottom of cube*/
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(4);

	indices.push_back(1);
	indices.push_back(4);
	indices.push_back(7);

	/*colors of each point*/

	normal.push_back(vec3(0.9f, 0.0f, 0.f));
	normal.push_back(vec3(0.7f, 0.0f, 0.f));
	normal.push_back(vec3(0.5f, 0.0f, 0.f));
	normal.push_back(vec3(0.3f, 0.0f, 0.f));

	normal.push_back(vec3(0.9f, 0.0f, 0.f));
	normal.push_back(vec3(0.7f, 0.0f, 0.f));
	normal.push_back(vec3(0.5f, 0.0f, 0.f));
	normal.push_back(vec3(0.3f, 0.0f, 0.f));
}

cube::~cube()
{

}

vector<vec3> cube::getVertices()
{
	return vertices;
}

vector<vec3> cube::getNormals()
{
	return normal;
}

vector<unsigned int> cube::getIndices()
{
	return indices;
}
