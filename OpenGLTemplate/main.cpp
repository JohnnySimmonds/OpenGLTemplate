
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


using namespace std;
using namespace glm;
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
GLuint vertexShader, fragmentShader, vertexShaderID, fragmentShaderID;
bool clearColor = false;
int EXIT = -1;
/*Stores information of the vbo to be used for the vao*/
struct VertexBuffers {
	enum { VERTICES = 0, NORMALS, INDICES, COUNT };

	GLuint id[COUNT];
};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xPosition, double yPosition);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexID, GLuint fragmentID);
GLuint initShader(string vertexShaderLoc, string fragmentShaderLoc);
bool initVaoVbo(GLuint& vao, VertexBuffers& vbo);
bool loadBuffer(const VertexBuffers& vbo, const vector<vec3>& points, const vector<vec3> normals, const vector<unsigned int>& indices);
bool CheckGLErrors(string location);
bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, vector<vec3> vertices, vector<vec3> normal, vector<unsigned int> indices);
void initGL();
bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view);
void printVec3(vec3 vecToPrint, string vecName);
void createCube(vector<vec3>& vertices, vector<unsigned int>& indices, vector<vec3>& normal);
GLFWwindow* createWindow();
camera mainCamera;
bool isFirstMousePosition = true;
bool mouseButtonOnePressed = false;
float lastX, lastY, yaw, pitch;

/*Loads the contents of the GLSL shader files*/
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}
GLuint LinkProgram(GLuint vertexID, GLuint fragmentID)
{
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexID);
	glAttachShader(shaderProgram, fragmentID);
	glLinkProgram(shaderProgram);

	return shaderProgram;
}
GLuint initShader(string vertexShaderLoc, string fragmentShaderLoc)
{
	vertexShader = GL_VERTEX_SHADER;
	string vertexShaderSource = LoadSource(vertexShaderLoc);

	vertexShaderID = CompileShader(vertexShader, vertexShaderSource);

	fragmentShader = GL_FRAGMENT_SHADER;
	string fragmentShaderSource = LoadSource(fragmentShaderLoc);

	fragmentShaderID = CompileShader(fragmentShader, fragmentShaderSource);

	GLuint shaderProgram = LinkProgram(vertexShaderID, fragmentShaderID);

	return shaderProgram;
}

void initGL()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
}
/*Checks for any opengl errors*/
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

/*Loads the buffer with the vbo buffer with the required data*/
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
GLFWwindow* createWindow()
{
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	return window;
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
bool initVaoVbo(GLuint& vao, VertexBuffers& vbo)
{
	glGenVertexArrays(1, &vao);

	glGenBuffers(VertexBuffers::COUNT, vbo.id);

	glBindVertexArray(vao);		//Set the active Vertex Array

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::VERTICES]);		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo.id[VertexBuffers::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.id[VertexBuffers::INDICES]);
	glBindVertexArray(0);
	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}
int main()
{
	vector<vec3> vertices, normal;
	vector<unsigned int> indices;
	GLuint vao;
	VertexBuffers vbo;
	GLuint shaderProgram;

	glfwInit();

	GLFWwindow* window = createWindow();
	
	if (window == NULL)
		return EXIT;
	

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//createTriangle(vertices, indices, normal);
	createCube(vertices, indices, normal);

	initVaoVbo(vao, vbo);

	shaderProgram = initShader("Shaders/vertex.glsl", "Shaders/frag.glsl");
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{

		if (clearColor == true)
		{
			glClearColor(0.2f, 0.5f, 0.3f, 1.0f);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render(shaderProgram, vao, vbo, vertices, normal, indices);

		glfwSwapBuffers(window);
		glfwPollEvents();
		//glfwWaitEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------	
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(shaderProgram);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
		mainCamera.moveCameraPositionForward();
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
		mainCamera.moveCameraPositionBackwards();
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
		mainCamera.moveCameraPositionRight();
	if (key == GLFW_KEY_D &&  (action == GLFW_REPEAT || action == GLFW_PRESS))
		mainCamera.moveCameraPositionLeft();
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		clearColor = false;
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
		clearColor = true;

	mainCamera.updateCameraView();
	
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if (button == GLFW_MOUSE_BUTTON_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		mouseButtonOnePressed = true;
	}
	else
		mouseButtonOnePressed = false;
}
void mouse_callback(GLFWwindow* window, double xPosition, double yPosition)
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

		float sensitivity = 0.05f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		vec3 target;
		target.x = cos(radians(pitch)) * cos(radians(yaw));
		target.y = sin(radians(pitch));
		target.z = cos(radians(pitch)) * sin(radians(yaw));

		mainCamera.updateCameraTarget(normalize(target));
		mainCamera.updateCameraView();
		printVec3(target, "Target");
	}
}
void printVec3 (vec3 vecToPrint, string vecName)
{
	cout << vecName << ": " << "X: " << vecToPrint.x << "Y: " << vecToPrint.y << "Z: " << vecToPrint.z << endl;
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

bool setViewMatrixForShaders(GLuint shaderProgram, mat4 view)
{
	glUseProgram(shaderProgram);
	GLuint cameraLocation = glGetUniformLocation(shaderProgram, "viewMatrix");
	glUniformMatrix4fv(cameraLocation, 1, false, &view[0][0]);

	glUseProgram(0);
	return !CheckGLErrors("loadUniforms");
}

bool render(GLuint shaderProgram, GLuint vao, VertexBuffers vbo, vector<vec3> vertices, vector<vec3> normal, vector<unsigned int> indices)
{
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 300.f);
	setViewMatrixForShaders(shaderProgram, perspectiveMatrix*mainCamera.getCameraView());
	glUseProgram(shaderProgram);
	glBindVertexArray(vao);
	loadBuffer(vbo, vertices, normal, indices);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);


	glUseProgram(0);
	glBindVertexArray(0);
	return !CheckGLErrors("Render");
}
