#include "inputCallBackForSpecificWindow.h"

inputCallBackForSpecificWindow::inputCallBackForSpecificWindow()
{

}

inputCallBackForSpecificWindow::inputCallBackForSpecificWindow(GLFWwindow* windowFromMain, camera* cameraWithWindow)
{
	window = windowFromMain;
	cameraForThisWindowInput = cameraWithWindow;
}


inputCallBackForSpecificWindow::~inputCallBackForSpecificWindow()
{
}
void inputCallBackForSpecificWindow::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraForThisWindowInput->moveCameraPositionForward();
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraForThisWindowInput->moveCameraPositionBackwards();
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraForThisWindowInput->moveCameraPositionLeft();
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
		cameraForThisWindowInput->moveCameraPositionRight();

}
void inputCallBackForSpecificWindow::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mouseButtonOnePressed = true;
	}
	else
		mouseButtonOnePressed = false;
}
void inputCallBackForSpecificWindow::mouse_callback(GLFWwindow* window, double xPosition, double yPosition)
{
	if (isFirstMousePosition)
	{
		lastX = xPosition;
		lastY = yPosition;
		isFirstMousePosition = false;
	}
	if (mouseButtonOnePressed)
	{
		float xOffset = xPosition - lastX;
		float yOffset = lastY - yPosition;
		lastX = xPosition;
		lastY = yPosition;

		float sensitivity = 0.5f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		vec3 target = vec3(0.0f, 0.0f, -3.0f);
		target.x = cos(radians(yaw)) * cos(radians(pitch));
		target.y = sin(radians(pitch));
		target.z = sin(radians(yaw)) * cos(radians(pitch));

		cameraForThisWindowInput->updateCameraTarget(normalize(target));

	}
}

void inputCallBackForSpecificWindow::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 90.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 90.0f)
		fov = 90.0f;
}

void inputCallBackForSpecificWindow::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

float inputCallBackForSpecificWindow::getFieldOfView()
{
	return fov;
}

