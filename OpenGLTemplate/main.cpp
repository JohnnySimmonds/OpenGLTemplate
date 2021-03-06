/*
OpenGL template
Features:
Built in camera
VBO with ability to send vertices, normals and indicies to the shaders
uniforms to shaders for model, view, and projection matrices.

Created By:
Johnny Simmonds

*/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "camera.h"
#include "inputCallBackForSpecificWindow.h"
#include "shader.h"
#include "errorCheckingGL.h"
#include "cube.h"
#include "triangle.h"


using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int EXIT = -1;

camera mainCamera;

inputCallBackForSpecificWindow inputForWindow;

errorCheckingGL error;


struct VertexBuffers {
	enum { VERTICES = 0, NORMALS, INDICES, COUNT };

	GLuint id[COUNT];
};

bool initVaoVbo(GLuint& vao, VertexBuffers& vbo);
bool loadBuffer(const VertexBuffers& vbo, shape* newShape);
bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, shape* newShape, mat4 perspectiveMatrix);
void setupOptionsOpenGL();
void loadProjectionModelViewUniforms(GLuint shaderProgram, mat4 projectionMatrix, mat4 modelMatrix, mat4 view);
bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view);
bool setModelMatrixForShaders(GLuint shaderProgram, mat4 modelMatrix);
bool setProjectionMatrixForShaders(GLuint shaderProgram, mat4 projectionMatrix);
void printVec3(vec3 vecToPrint, string vecName);
GLFWwindow* createWindow();
void setupInputForWindow(GLFWwindow* window);
int getWidthFromViewport();
int getHeightFromViewport();
GLint* getWindowDimensions();
mat4 updatePerspectiveMatrix();


int main()
{
	vector<vec3> vertices, normal;
	vector<unsigned int> indices;
	GLuint vao;
	VertexBuffers vbo;
	mat4 perspectiveMatrix;
	mat4 modelMatrix = mat4(1.0f);
	glfwInit();


	shape* newCube;
	newCube = new cube;
	newCube->create(1.0f);

	shape* newTriangle;
	newTriangle = new triangle;
	newTriangle->create(1.0f);

	GLFWwindow* window = createWindow();
	
	if (window == NULL)
		return EXIT;
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	setupOptionsOpenGL();

	initVaoVbo(vao, vbo);

	shader shader("Shaders/vertex.glsl", "Shaders/frag.glsl");

	while (!glfwWindowShouldClose(window))
	{
		perspectiveMatrix = updatePerspectiveMatrix();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		mainCamera.updateCameraView();
		loadProjectionModelViewUniforms(shader.getShaderProgram(), perspectiveMatrix, modelMatrix, mainCamera.getCameraView());
		render(shader.getShaderProgram(), vao, vbo, newCube, perspectiveMatrix);

		render(shader.getShaderProgram(), vao, vbo, newTriangle, perspectiveMatrix);

		glfwSwapBuffers(window);
		glfwPollEvents();
	
	}
	
	glfwTerminate();

	return 0;
}

GLFWwindow* createWindow()
{
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	setupInputForWindow(window);

	return window;
}

void setupInputForWindow(GLFWwindow* window)
{
	inputForWindow = inputCallBackForSpecificWindow(window, &mainCamera);

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, &inputForWindow);

	auto frameBufferCallback = [](GLFWwindow* w, int width, int height)
	{
		static_cast<inputCallBackForSpecificWindow*>(glfwGetWindowUserPointer(w))->framebuffer_size_callback(w, width, height);
	};

	auto keyCallback = [](GLFWwindow* w, int key, int scancode, int action, int mods)
	{
		static_cast<inputCallBackForSpecificWindow*>(glfwGetWindowUserPointer(w))->key_callback(w, key, scancode, action, mods);
	};
	auto cursorPosCallback = [](GLFWwindow* w, double xPosition, double yPosition)
	{
		static_cast<inputCallBackForSpecificWindow*>(glfwGetWindowUserPointer(w))->mouse_callback(w, xPosition, yPosition);
	};

	auto mouseButtonCallback = [](GLFWwindow* w, int button, int action, int mods)
	{
		static_cast<inputCallBackForSpecificWindow*>(glfwGetWindowUserPointer(w))->mouse_button_callback(w, button, action, mods);
	};
	auto scrollCallback = [](GLFWwindow* w, double xoffset, double yoffset)
	{
		static_cast<inputCallBackForSpecificWindow*>(glfwGetWindowUserPointer(w))->scroll_callback(w, xoffset, yoffset);
	};

	glfwSetFramebufferSizeCallback(window, frameBufferCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
}

void setupOptionsOpenGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.3f, 0.4f, 0.5f, 0.f);
}

bool initVaoVbo(GLuint& vao, VertexBuffers& vbo)
{
	glGenVertexArrays(1, &vao);

	glGenBuffers(VertexBuffers::COUNT, vbo.id);

	glBindVertexArray(vao);		

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);

	glBindVertexArray(0);
	return error.openGlErrorCheck("initVAO");		//Check for errors in initialize
}

int getHeightFromViewport()
{
	GLint* dimensions = getWindowDimensions();
	GLint currWindowHeight = dimensions[3];
	return (int)currWindowHeight;

}

int getWidthFromViewport()
{
	GLint* dimensions = getWindowDimensions();
	GLint currWindowWidth = dimensions[2];
	return (int)currWindowWidth;
}

GLint* getWindowDimensions()
{
	GLint windowDimensions[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, windowDimensions);
	return windowDimensions;
}

mat4 updatePerspectiveMatrix()
{
	int height, width;
	mat4 newPerspectiveMatrix;

	height = getHeightFromViewport();
	width = getWidthFromViewport();

	newPerspectiveMatrix = perspective(radians(inputForWindow.getFieldOfView()), (float)width / (float)height, 0.1f, 300.f);

	return newPerspectiveMatrix;
}

void loadProjectionModelViewUniforms(GLuint shaderProgram, mat4 projectionMatrix, mat4 modelMatrix, mat4 view)
{
	setViewMatrixForShaders(shaderProgram, view);
	setModelMatrixForShaders(shaderProgram, modelMatrix);
	setProjectionMatrixForShaders(shaderProgram, projectionMatrix);
}
bool setProjectionMatrixForShaders(GLuint shaderProgram, mat4 projectionMatrix)
{
	glUseProgram(shaderProgram);
	GLuint modelLocationForShader = glGetUniformLocation(shaderProgram, "projectionMatrix");
	glUniformMatrix4fv(modelLocationForShader, 1, false, &projectionMatrix[0][0]);

	glUseProgram(0);
	return error.openGlErrorCheck("setProjectionMatrixForShaders");

}
bool setModelMatrixForShaders(GLuint shaderProgram, mat4 modelMatrix)
{
	glUseProgram(shaderProgram);
	GLuint modelLocationForShader = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(modelLocationForShader, 1, false, &modelMatrix[0][0]);

	glUseProgram(0);
	return error.openGlErrorCheck("setModelMatrixForShaders");

}
bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view)
{
	glUseProgram(shaderProgram);
	GLuint cameraLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(cameraLocation, 1, false, &view[0][0]);

	glUseProgram(0);
	return error.openGlErrorCheck("setViewMatrixForShaders");
}

bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, shape* newShape, mat4 perspectiveMatrix)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	loadBuffer(vbo, newShape);
	glDrawElements(GL_TRIANGLES, newShape->getIndices().size(), GL_UNSIGNED_INT, (void*)0);
	glUseProgram(0);
	glBindVertexArray(0);
	return error.openGlErrorCheck("Render");
}


bool loadBuffer(const VertexBuffers& vbo, shape* newShape)
{

	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*newShape->getVertices().size(),		//Size of data in array (in bytes)
		&newShape->getVertices()[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*newShape->getNormals().size(),	//Size of data in array (in bytes)
		&newShape->getNormals()[0],					//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*newShape->getIndices().size(),
		&newShape->getIndices()[0],
		GL_STATIC_DRAW
	);

	return error.openGlErrorCheck("loadBuffer");
}

void printVec3(vec3 vecToPrint, string vecName)
{
	cout << vecName << ": " << "X: " << vecToPrint.x << "Y: " << vecToPrint.y << "Z: " << vecToPrint.z << endl;
}

