#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

class shader
{
public:
	shader(string vertexShaderLoc, string fragmentShaderLoc);
	~shader();
	GLuint getShaderProgram();
	
private:
	void initShader();
	string LoadSource(const string &filename);
	GLuint CompileShader(GLenum shaderType, const string &source);
	GLuint LinkProgram(GLuint vertexID, GLuint fragmentID);


	string vertexShaderLocation, fragmentShaderLocation;
	GLuint vertexShader, fragmentShader, vertexShaderID, fragmentShaderID;
	GLuint shaderProgram;
};

