#include "shader.h"

shader::shader(string vertexShaderLoc, string fragmentShaderLoc)
{
	vertexShaderLocation = vertexShaderLoc;
	fragmentShaderLocation = fragmentShaderLoc;
	initShader();
}
shader::~shader()
{
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(shaderProgram);
}

void shader::initShader()
{
	vertexShader = GL_VERTEX_SHADER;
	string vertexShaderSource = LoadSource(vertexShaderLocation);

	vertexShaderID = CompileShader(vertexShader, vertexShaderSource);

	fragmentShader = GL_FRAGMENT_SHADER;
	string fragmentShaderSource = LoadSource(fragmentShaderLocation);

	fragmentShaderID = CompileShader(fragmentShader, fragmentShaderSource);

	shaderProgram = LinkProgram(vertexShaderID, fragmentShaderID);

	
}
string shader::LoadSource(const string &filename)
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
GLuint shader::CompileShader(GLenum shaderType, const string &source)
{
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

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
GLuint shader::LinkProgram(GLuint vertexID, GLuint fragmentID)
{
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexID);
	glAttachShader(shaderProgram, fragmentID);
	glLinkProgram(shaderProgram);

	return shaderProgram;
}

GLuint shader::getShaderProgram()
{
	return shaderProgram;
}
