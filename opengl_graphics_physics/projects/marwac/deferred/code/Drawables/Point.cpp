#include "Point.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Material.h"
#include <algorithm>
using namespace mwm;
Point::Point(){

	mat = new Material();
	SetUpBuffers();
}

const Vector3 Point::vertices[] = {
	Vector3(0.f, 0.f, 0.f)
};

const GLushort Point::elements[] = {
	0
};



void Point::SetUpBuffers()
{
	mesh = new Mesh();
	//Create VAO
	glGenVertexArrays(1, &mesh->vaoHandle);
	//Bind VAO
	glBindVertexArray(mesh->vaoHandle);

	// 1rst attribute buffer : vertices
	glGenBuffers(1, &mesh->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(Vector3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset
	glEnableVertexAttribArray(0);

	// 4th element buffer Generate a buffer for the indices as well
	glGenBuffers(1, &mesh->elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1 * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
	mesh->indicesSize = 1;

	//Unbind the VAO now that the VBOs have been set up
	glBindVertexArray(0);
}

void Point::Draw(const Matrix4& Model, const Matrix4& View, const Matrix4& Projection, float size)
{
	Matrix4F MVP = (Model*View*Projection).toFloat();
	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	GLuint wireframeShader = ShaderManager::Instance()->shaderIDs["wireframe"];
	glUseProgram(wireframeShader);
	MatrixHandle = glGetUniformLocation(wireframeShader, "MVP");
	MaterialColorValueHandle = glGetUniformLocation(wireframeShader, "MaterialColorValue");

	glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(MaterialColorValueHandle, 1, &this->mat->color.vect[0]);

	//bind vao before drawing
	glBindVertexArray(this->mesh->vaoHandle);
	glPointSize(size);
	// Draw the lines !
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_SHORT, 0);
	glUseProgram(prevShader);
}