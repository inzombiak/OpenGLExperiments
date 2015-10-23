#include "RenderObject.h"
#include "HelperFunctions.h"

#include <ddraw.h>
#include <gl\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

RenderObject::RenderObject(GLuint program)
{
	m_program = program;	
}

//bool RenderObject::InitWithColor(std::vector<GLfloat> color)
//{
//	m_uvs = FloatVecToGLMVec3(color);
//
//	glGenBuffers(1, &m_colorBufferObject);
//	glBindBuffer(GL_ARRAY_BUFFER, m_colorBufferObject);
//	glBufferData(GL_ARRAY_BUFFER, m_uvs.size()*sizeof(glm::vec2), m_uvs.data(), GL_STATIC_DRAW);
//	
//	return true;
//}

bool RenderObject::InitWithOBJ(std::string filepath)
{
	printf("Loading OBJ file %s...\n", filepath);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file;
	fopen_s(&file, filepath.c_str(), "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0){
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0){
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0){
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;
	std::vector<unsigned int> indicies;

	// For each vertex of each triangle
	for (unsigned int i = 0; i<vertexIndices.size(); i++)
	{

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		vertices.push_back(vertex);
		uvs.push_back(uv);
		normals.push_back(normal);

	}

	computeTangentBasis(
		vertices, uvs, normals, 
		tangents, bitangents 
		);

	indexVBO_TBN(
		vertices, uvs, normals, tangents, bitangents,
		m_indices, m_vertices, m_uvs, m_normals, m_tangents, m_bitangents
		);

	glGenBuffers(1, &m_vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_uvs.size() * sizeof(glm::vec2), &m_uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_normalBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_tangentBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(glm::vec3), &m_tangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_bitangentBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(glm::vec3), &m_bitangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_indexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_indexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned short), &m_indices[0], GL_STATIC_DRAW);
	
	return true;
}

void RenderObject::SetSpecularTex(GLuint texID)
{
	m_specularTextureID = texID;
}
void RenderObject::SetNormalTex(GLuint texID)
{
	m_normalTextureID = texID;
}
void RenderObject::SetDiffuseTex(GLuint texID)
{
	m_diffuseTextureID = texID;
}

bool RenderObject::SetUV(std::vector<GLfloat> UVcoordinates)
{
	m_uvs = FloatVecToGLMVec2(UVcoordinates);

	glGenBuffers(1, &m_uvBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_uvs.size()*sizeof(glm::vec2), m_uvs.data(), GL_STATIC_DRAW);

	return true;
}

bool RenderObject::SetVerticies(std::vector<GLfloat> vertexCoords)
{
	m_vertices = FloatVecToGLMVec3(vertexCoords);

	glGenBuffers(1, &m_vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);

	return true;
}

void RenderObject::draw(glm::mat4& Vmatrix, glm::mat4& Pmatrix)
{
	glUseProgram(m_program);

	glm::mat4 Mmatrix;
	Mmatrix = glm::mat4(1.0f);

	glm::mat4 MVMatrix = Vmatrix * Mmatrix;
	glm::mat3 MV3x3Matrix = glm::mat3(MVMatrix);

	GLuint VMatID = glGetUniformLocation(m_program, "V");
	glUniformMatrix4fv(VMatID, 1, GL_FALSE, &Vmatrix[0][0]);

	GLuint MMatID = glGetUniformLocation(m_program, "M");
	glUniformMatrix4fv(MMatID, 1, GL_FALSE, &Mmatrix[0][0]);

	GLuint MVPMatID = glGetUniformLocation(m_program, "MVP");
	glm::mat4 MVP = Pmatrix * Vmatrix * Mmatrix;
	glUniformMatrix4fv(MVPMatID, 1, GL_FALSE, &MVP[0][0]);

	GLuint MV3x3MatrixID = glGetUniformLocation(m_program, "MV3x3");
	glUniformMatrix3fv(MV3x3MatrixID, 1, GL_FALSE, &MV3x3Matrix[0][0]);

	if (m_diffuseTextureID != 0)
	{
		GLuint DiffuseTextureID = glGetUniformLocation(m_program, "DiffuseTextureSampler");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_diffuseTextureID);
		glUniform1i(DiffuseTextureID, 0);
	}

	if (m_normalTextureID != 0)
	{
		GLuint NormalTextureID = glGetUniformLocation(m_program, "NormalTextureSampler");
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normalTextureID);
		glUniform1i(NormalTextureID, 1);
	}

	if (m_specularTextureID != 0)
	{
		GLuint SpecularTextureID = glGetUniformLocation(m_program, "SpecularTextureSampler");
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_specularTextureID);
		glUniform1i(SpecularTextureID, 2);
	}

	GLuint LightID = glGetUniformLocation(m_program, "LightPosition_worldspace");
	glm::vec3 lightPos = glm::vec3(0, 4, 4);
	glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_uvBufferObject);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glEnableVertexAttribArray(1);
	/*
	glBindBuffer(GL_ARRAY_BUFFER, m_normalBufferObject);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, m_tangentBufferObject);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, m_bitangentBufferObject);
	glVertexAttribPointer(
		4,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);
	glEnableVertexAttribArray(4);*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,      // mode
		m_indices.size(),    // count
		GL_UNSIGNED_SHORT, // type
		(void*)0           // element array buffer offset
		);
}