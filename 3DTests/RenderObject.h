#pragma once

#include <vector>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

class RenderObject
{
public:
	RenderObject(GLuint program);
	//bool InitWithColor(std::vector<GLfloat> color);
	bool InitWithOBJ(std::string filepath);

	void SetSpecularTex(GLuint texID);
	void SetNormalTex(GLuint texID);
	void SetDiffuseTex(GLuint texID);

	bool SetUV(std::vector<GLfloat> UVcoordinates);
	bool SetVerticies(std::vector<GLfloat> vertexCoords);

	void draw(glm::mat4& Vmatrix, glm::mat4& Pmatrix);

private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvs;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_tangents;
	std::vector<glm::vec3> m_bitangents;
	std::vector<unsigned short> m_indices;

	GLuint m_program;
	GLuint m_vertexBufferObject;
	GLuint m_uvBufferObject;
	GLuint m_normalBufferObject;
	GLuint m_tangentBufferObject;
	GLuint m_bitangentBufferObject;
	GLuint m_indexBufferObject;

	GLuint m_diffuseTextureID;
	GLuint m_specularTextureID;
	GLuint m_normalTextureID;
	
};

