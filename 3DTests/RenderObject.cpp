#include "RenderObject.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

RenderObject::RenderObject(std::vector<GLfloat> points, std::vector<GLfloat> color, GLuint program)
{
	m_points = points;
	m_color = color;
	m_program = program;

	glGenBuffers(1, &m_positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_points.size()*sizeof(GLfloat), m_points.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER,m_colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_color.size()*sizeof(GLfloat), m_color.data(), GL_STATIC_DRAW);
}

RenderObject::RenderObject(std::vector<GLfloat> points, std::string texturePath, std::vector<GLfloat> UVVec, GLuint program)
{
	m_points = points;
	m_color = UVVec;
	m_program = program;

	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	FILE* file;
	fopen_s(&file, texturePath.c_str(), "rb");
	if (!file)                              
	{ 
		printf("Image could not be opened\n"); 
		return;
	}

	if (fread(header, 1, 54, file) != 54)
	{ 
		printf("Not a correct BMP file\n");
		return;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		printf("Not a correct BMP file\n");
		return;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0)    
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      
		dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	glGenTextures(1, &m_textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenBuffers(1, &m_positionBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_points.size()*sizeof(GLfloat), m_points.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_color.size()*sizeof(GLfloat), m_color.data(), GL_STATIC_DRAW);

	m_isTextured = true;
}

void RenderObject::draw(glm::mat4& VPmatrix)
{
	//Model-view-projection matrixid
	GLuint MVPMatID = glGetUniformLocation(m_program, "MVP");
	glm::mat4 model;
	model = glm::mat4(1.0f);

	glm::mat4 MVP = VPmatrix * model;

	glUseProgram(m_program);

	glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &MVP[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferObject);

	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);
	glEnableVertexAttribArray(0);

	if (m_isTextured)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferObject);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferObject);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
			);
	}
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}