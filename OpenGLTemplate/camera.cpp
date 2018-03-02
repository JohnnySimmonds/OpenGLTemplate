#include "camera.h"

camera::camera()
{
	cameraPosition = vec3(0.0f, 0.0f, 3.0f);
	cameraTarget = vec3(0.0f, 0.0f, -1.0f);
	cameraUp = vec3(0.0f, 1.0f, 0.0f);
	cameraSpeed = 0.005f;
	view = lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
}

camera::~camera()
{
}

void camera::moveCameraPositionForward()
{
	cameraPosition += cameraSpeed * cameraTarget;
	view = lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
}
void camera::moveCameraPositionBackwards()
{
	cameraPosition -= cameraSpeed * cameraTarget;
	view = lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
}
void camera::moveCameraPositionLeft()
{
	cameraPosition -= cameraSpeed * normalize(cross(cameraTarget, cameraUp));
	view = lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
}
void camera::moveCameraPositionRight()
{
	cameraPosition += cameraSpeed * normalize(cross(cameraTarget, cameraUp));
	view = lookAt(cameraPosition, cameraPosition + cameraTarget, cameraUp);
}
mat4 camera::getCameraView()
{
	return view;
}