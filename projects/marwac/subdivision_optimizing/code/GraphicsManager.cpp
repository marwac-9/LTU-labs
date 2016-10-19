#define _CRT_SECURE_NO_DEPRECATE
#include "GraphicsManager.h"
#include "GraphicsStorage.h"
#include "OBJ.h"
#include "Mesh.h"
#include "HalfEdgeMesh.h"
#include "Object.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <GL/glew.h>
#include <iosfwd>
#include "Scene.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

using namespace mwm;

GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{
}

bool GraphicsManager::LoadOBJs(const char * path)
{
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	while (1){

		char lineHeader[128];
		//meshID not in use currently
		int objID = 0;
		// read the first word of the line
		int res = fscanf(file, "%s %d", lineHeader, &objID);
		if (res == EOF)
		{
			break; // EOF = End Of File. Quit the loop.
		}
		// else : parse lineHeader

		OBJ* tempOBJ = new OBJ();
		tempOBJ->LoadAndIndexOBJ(lineHeader);
		tempOBJ->ID = objID;
		GraphicsStorage::objects.push_back(tempOBJ);
	}
	fclose(file);
	return true;
}

bool GraphicsManager::SaveToOBJ(OBJ *obj)
{
	FILE * file;
	file = fopen("savedFile.obj", "w");
	for (size_t i = 0; i < obj->indexed_vertices.size(); i++)
	{
		std::ostringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << "v ";
		ss << obj->indexed_vertices.at(i).vect[0];
		ss << " "; 
		ss << obj->indexed_vertices.at(i).vect[1]; 
		ss << " ";
		ss << obj->indexed_vertices.at(i).vect[2];
		ss << "\n";
		std::string s(ss.str());
		fputs(s.c_str(), file);
	}

	for (size_t i = 0; i < obj->indexed_uvs.size(); i++)
	{
		std::ostringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << "vt ";
		ss << obj->indexed_uvs.at(i).vect[0];
		ss << " ";
		ss << obj->indexed_uvs.at(i).vect[1];
		ss << " ";
		ss << obj->indexed_uvs.at(i).vect[2];
		ss << "\n";
		std::string s(ss.str());
		fputs(s.c_str(), file);
	}

	for (size_t i = 0; i < obj->indexed_normals.size(); i++)
	{
		std::ostringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << "vn ";
		ss << obj->indexed_normals.at(i).vect[0];
		ss << " ";
		ss << obj->indexed_normals.at(i).vect[1];
		ss << " ";
		ss << obj->indexed_normals.at(i).vect[2];
		ss << "\n";
		std::string s(ss.str());
		fputs(s.c_str(), file);
	}

	for (size_t i = 0; i < obj->indices.size(); i += 3)
	{
		std::string faceRow = "f " + std::to_string(obj->indices.at(i) + 1) + "/" + std::to_string(obj->indices.at(i) + 1) + "/" + std::to_string(obj->indices.at(i) + 1) + " ";
		faceRow = faceRow + std::to_string(obj->indices.at(i + 1) + 1) + "/" + std::to_string(obj->indices.at(i + 1) + 1) + "/" + std::to_string(obj->indices.at(i + 1) + 1) + " ";
		faceRow = faceRow + std::to_string(obj->indices.at(i + 2) + 1) + "/" + std::to_string(obj->indices.at(i + 2) + 1) + "/" + std::to_string(obj->indices.at(i + 2) + 1) + "\n";
		fputs(faceRow.c_str(), file);
	}
	
	fclose(file);

	return true;
}

bool GraphicsManager::LoadLevel(const char * path) {
	FILE * file;
	file = fopen(path, "r");
	if (file == NULL){
		printf("Impossible to open the file ! Are you in the right path ?\n");
		getchar();
		return false;
	}
	std::map<int,Object*>::iterator it = Scene::Instance()->objectsToRender.begin();
	char line[128];
	while (fgets(line, sizeof(line), file)) {
		if (line[0] == '/' || line[0] == ';' || line[0] == '#') continue; /* ignore comment line */
		int meshID;
		int materialID;
		char meshName[128];
		int matches = sscanf(line, "%s %d %d", meshName, &meshID, &materialID);
		if (matches != 3)
		{
			printf("Wrong material information!\n");
		}
		else {
			it->second->AssignMesh(GraphicsStorage::meshes.at(meshID));
			it++;
		}
	}
	fclose(file);
	return true;
}

Mesh* GraphicsManager::LoadOBJToVBO(OBJ* object, Mesh* mesh)
{
	//Create VAO
	glGenVertexArrays(1, &mesh->vaoHandle);
	//Bind VAO
	glBindVertexArray(mesh->vaoHandle);

	// 1rst attribute buffer : vertices
	glGenBuffers(1, &mesh->vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, object->indexed_vertices.size() * sizeof(Vector3), &object->indexed_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : UVs
	glGenBuffers(1, &mesh->uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, object->indexed_uvs.size() * sizeof(Vector2), &object->indexed_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset
	glEnableVertexAttribArray(1);

	// 3rd attribute buffer : normals
	glGenBuffers(1, &mesh->normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, object->indexed_normals.size() * sizeof(Vector3), &object->indexed_normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // attribute, size, type, normalized?, stride, array buffer offset
	glEnableVertexAttribArray(2);

	// 4th element buffer Generate a buffer for the indices as well
	glGenBuffers(1, &mesh->elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, object->indices.size() * sizeof(unsigned int), &object->indices[0], GL_STATIC_DRAW);
	mesh->indicesSize = object->indices.size();

    //Unbind the VAO now that the VBOs have been set up
    glBindVertexArray(0);

	return mesh;
}

void GraphicsManager::LoadAllOBJsToVBO()
{
	for (size_t i = 0; i < GraphicsStorage::objects.size(); i++)
	{	
		Mesh* newMesh = new Mesh();
		LoadOBJToVBO(GraphicsStorage::objects.at(i), newMesh);
		GraphicsStorage::meshes.push_back(newMesh);
	}
}
