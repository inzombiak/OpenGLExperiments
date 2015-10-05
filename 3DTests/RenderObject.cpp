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

	glGenBuffers(1, &m_colorBuffferObject);
	glBindBuffer(GL_ARRAY_BUFFER,m_colorBuffferObject);
	glBufferData(GL_ARRAY_BUFFER, m_color.size()*sizeof(GLfloat), m_color.data(), GL_STATIC_DRAW);
}

void RenderObject::draw(glm::mat4& VPmatrix)
{
	//Model-view-projection matrixid
	GLuint MVPMatID = glGetUniformLocation(m_program, "MVP");
	glm::mat4 model;
	model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -m_yRotate, glm::vec3(1.0f, 0.0f, 0.0f));
	//model = glm::rotate(model, -m_xRotate, glm::vec3(0.0f, 1.0f, 0.0f));

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


	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffferObject);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}