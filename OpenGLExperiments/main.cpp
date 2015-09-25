#include <iostream>
#define FREEGLUT_STATIC
#define GLEW_STATIC
#include <gl\glew.h>
#include <gl\freeglut.h>


//Copyright (C) 2010-2012 by Jason L. McKesson
//This file is licensed under the MIT License.
#include <algorithm>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <fstream>

const float vertexPositions[] = {
	0.75f, 0.75f, 0.0f, 1.0f,
	0.75f, -0.75f, 0.0f, 1.0f,
	-0.75f, -0.75f, 0.0f, 1.0f,
};

GLuint positionBufferObject;
GLuint vao;


std::string ReadFileToString(const char* filePath)
{
	std::string content;
	std::ifstream fileStream(filePath, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint CreateShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

GLuint theProgram;


void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, ReadFileToString("VertexShader.glsl")));
	//shaderList.push_back(CreateShader(GL_TESS_CONTROL_SHADER, ReadFileToString("TessalationShader.glsl")));
	//shaderList.push_back(CreateShader(GL_TESS_EVALUATION_SHADER, ReadFileToString("TessalationEvaluator.glsl")));
	//shaderList.push_back(CreateShader(GL_GEOMETRY_SHADER, ReadFileToString("GeometryShader.glsl")));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, ReadFileToString("FragmentShader.glsl")));
	theProgram = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void InitializeVertexBuffer()
{
	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeVertexBuffer();
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//glPatchParameteri(GL_PATCH_VERTICES, 3);
	glUseProgram(theProgram);
	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);

	GLfloat attrib[] = { (float)sin(0.4) * 0.5f,(float)cos(1) * 0.6f, 0.0f, 0.0f };
	glVertexAttrib4fv(0, attrib);
	GLfloat attrib2[] = { 0.5f, 0.6f, 0.0f, 1.0f };
	glVertexAttrib4fv(1, attrib2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	//glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//glDisableVertexAttribArray(0);
	//glUseProgram(0);

	glutSwapBuffers();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		return;
	}
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("simple OpenGL example");
	glewInit();
	glutDisplayFunc(display);
	init();
	glutMainLoop();
}
unsigned int defaults(unsigned int displayMode, int &width, int &height) { return displayMode; }
