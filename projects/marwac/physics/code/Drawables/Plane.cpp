#include "Plane.h"
#include "Mesh.h"
#include "ShaderManager.h"
#include "Material.h"
#include <algorithm>

using namespace mwm;

Plane::Plane(){

	mat = new Material();
	SetUpBuffers();
}

const Vector3 Plane::vertices[] = {
	Vector3(-1.f, -1.f, 0.f),
	Vector3(1.f, -1.f, 0.f),
	Vector3(-1.f, 1.f, 0.f),
	Vector3(1.f, 1.f, 0.f)
};

const GLushort Plane::elements[] = {
	0, 1, 2, 2, 1, 3
};



void Plane::SetUpBuffers()
{
	mesh = new Mesh();
	//Create VAO
	glGenVertexArrays(1, &mesh->vaoHandle);
	//Bind VAO
	glBindVertexArray(mesh->vaoHandle);

	// 1rst attribute buffer : vertices
	glGenBuffers(1, &mesh->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vector3), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset
	glEnableVertexAttribArray(0);

	// 4th element buffer Generate a buffer for the indices as well
	glGenBuffers(1, &mesh->elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
	mesh->indicesSize = 6;

	//Unbind the VAO now that the VBOs have been set up
	glBindVertexArray(0);
}

void Plane::Draw(const Matrix4& Model, const Matrix4& View, const Matrix4& Projection)
{
	Matrix4F MVP = (Model*View*Projection).toFloat();
	GLuint prevShader = ShaderManager::Instance()->GetCurrentShaderID();
	ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["wireframe"]);	
	MatrixHandle = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["wireframe"], "MVP");
	MaterialColorValueHandle = glGetUniformLocation(ShaderManager::Instance()->shaderIDs["wireframe"], "MaterialColorValue");

	glUniformMatrix4fv(MatrixHandle, 1, GL_FALSE, &MVP[0][0]);
	glUniform3fv(MaterialColorValueHandle, 1, &this->mat->color.vect[0]);

	//bind vao before drawing
	glBindVertexArray(this->mesh->vaoHandle);

	// Draw the triangles !
	glDrawElements(GL_TRIANGLES, this->mesh->indicesSize, GL_UNSIGNED_SHORT, (void*)0); // mode, count, type, element array buffer offset
	ShaderManager::Instance()->SetCurrentShader(prevShader);
}
