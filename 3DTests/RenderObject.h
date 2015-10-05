#pragma once

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class RenderObject
{
public:
	RenderObject(std::vector<GLfloat> points, std::vector<GLfloat> color, GLuint program);

	void draw(glm::mat4& VPmatrix);

private:
	std::vector<float> m_points;
	std::vector<float> m_color;
	GLuint m_program;
	GLuint m_positionBufferObject;
	GLuint m_colorBuffferObject;
};

