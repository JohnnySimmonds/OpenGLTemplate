#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
class inputCallBackForSpecificWindow
{
public:
	inputCallBackForSpecificWindow();
	inputCallBackForSpecificWindow(GLFWwindow* window, camera* cameraWithWindow);
	~inputCallBackForSpecificWindow();

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void mouse_callback(GLFWwindow* window, double xPosition, double yPosition);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	float getFieldOfView();
private:
	GLFWwindow* window;
	camera* cameraForThisWindowInput;
	bool mouseButtonOnePressed = false;
	float lastX, lastY, yaw, pitch, fov = 90.0f;
};

