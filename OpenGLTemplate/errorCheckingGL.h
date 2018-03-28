#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
using namespace std;
class errorCheckingGL
{
public:
	errorCheckingGL();
	~errorCheckingGL();
	bool openGlErrorCheck(string location);
};

