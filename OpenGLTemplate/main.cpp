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


using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint vertexShader, fragmentShader, vertexShaderID, fragmentShaderID;

int EXIT = -1;

camera mainCamera;

inputCallBackForSpecificWindow inputForWindow;


struct VertexBuffers {
	enum { VERTICES = 0, NORMALS, INDICES, COUNT };

	GLuint id[COUNT];
};

bool initVaoVbo(GLuint& vao, VertexBuffers& vbo);
bool loadBuffer(const VertexBuffers& vbo, const vector<vec3>& points, const vector<vec3> normals, const vector<unsigned int>& indices);
bool CheckGLErrors(string location);
bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, vector<vec3> vertices, vector<vec3> normal, vector<unsigned int> indices, mat4 perspectiveMatrix);
void setupOptionsOpenGL();
void loadProjectionModelViewUniforms(GLuint shaderProgram, mat4 projectionMatrix, mat4 modelMatrix, mat4 view);
bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view);
bool setModelMatrixForShaders(GLuint shaderProgram, mat4 modelMatrix);
bool setProjectionMatrixForShaders(GLuint shaderProgram, mat4 projectionMatrix);
void printVec3(vec3 vecToPrint, string vecName);
void createCube(vector<vec3>& vertices, vector<unsigned int>& indices, vector<vec3>& normal);
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

	createCube(vertices, indices, normal);

	shader shader("Shaders/vertex.glsl", "Shaders/frag.glsl");

	while (!glfwWindowShouldClose(window))
	{
		perspectiveMatrix = updatePerspectiveMatrix();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		mainCamera.updateCameraView();
		loadProjectionModelViewUniforms(shader.getShaderProgram(), perspectiveMatrix, modelMatrix, mainCamera.getCameraView());
		render(shader.getShaderProgram(), vao, vbo, vertices, normal, indices, perspectiveMatrix);

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
	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}

void createTriangle(vector<vec3>& vertices, vector<unsigned int>& indices, vector<vec3>& normal)
{
	vertices.push_back(vec3(0.5f, 0.5f, 0.0f));
	vertices.push_back(vec3(0.5f, -0.5f, 0.0f));
	vertices.push_back(vec3(-0.5f, 0.5f, 0.0f));

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	normal.push_back(vec3(1.0f, 0.0f, 0.0f));
	normal.push_back(vec3(0.0f, 1.0f, 0.0f));
	normal.push_back(vec3(0.0f, 0.0f, 1.0f));
}

void createCube(vector<vec3>& vertices, vector<unsigned int>& indices, vector<vec3>& normal)
{
	/* points for generating the cube*/
	vertices.push_back(vec3(1.0f, 1.0f, 1.f)); //0
	vertices.push_back(vec3(1.0f, -1.0f, 1.f)); //1
	vertices.push_back(vec3(-1.0f, -1.0f, 1.f)); //2
	vertices.push_back(vec3(-1.0f, 1.0f, 1.f)); //3
	vertices.push_back(vec3(-1.0f, -1.0f, -1.f)); //4
	vertices.push_back(vec3(-1.0f, 1.0f, -1.f)); //5
	vertices.push_back(vec3(1.0f, 1.0f, -1.f)); //6
	vertices.push_back(vec3(1.0f, -1.0f, -1.f)); //7


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
	return !CheckGLErrors("setProjectionMatrixForShaders");

}
bool setModelMatrixForShaders(GLuint shaderProgram, mat4 modelMatrix)
{
	glUseProgram(shaderProgram);
	GLuint modelLocationForShader = glGetUniformLocation(shaderProgram, "modelMatrix");
	glUniformMatrix4fv(modelLocationForShader, 1, false, &modelMatrix[0][0]);

	glUseProgram(0);
	return !CheckGLErrors("setModelMatrixForShaders");

}
bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view)
{
	glUseProgram(shaderProgram);
	GLuint cameraLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(cameraLocation, 1, false, &view[0][0]);

	glUseProgram(0);
	return !CheckGLErrors("setViewMatrixForShaders");
}

bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, vector<vec3> vertices, vector<vec3> normal, vector<unsigned int> indices, mat4 perspectiveMatrix)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	loadBuffer(vbo, vertices, normal, indices);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
	glUseProgram(0);
	glBindVertexArray(0);
	return !CheckGLErrors("Render");
}

bool loadBuffer(const VertexBuffers& vbo,
	const vector<vec3>& points,
	const vector<vec3> normals,
	const vector<unsigned int>& indices)
{

	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),		//Size of data in array (in bytes)
		&points[0],						//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
	);

	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*normals.size(),	//Size of data in array (in bytes)
		&normals[0],					//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW					//GL_DYNAMIC_DRAW if you're changing the data often
										//GL_STATIC_DRAW if you're changing seldomly
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*indices.size(),
		&indices[0],
		GL_STATIC_DRAW
	);

	return !CheckGLErrors("loadBuffer");
}

bool CheckGLErrors(string location)
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}
void printVec3(vec3 vecToPrint, string vecName)
{
	cout << vecName << ": " << "X: " << vecToPrint.x << "Y: " << vecToPrint.y << "Z: " << vecToPrint.z << endl;
}

