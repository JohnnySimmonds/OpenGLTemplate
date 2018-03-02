#pragma once
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
class camera
{
public:
	camera();
	~camera();
	void camera::moveCameraPositionForward();
	void camera::moveCameraPositionBackwards();
	void camera::moveCameraPositionLeft();
	void camera::moveCameraPositionRight();
	mat4 getCameraView();
private:
	vec3 cameraPosition, cameraTarget, cameraUp;
	mat4 view;
	float cameraSpeed;
};

