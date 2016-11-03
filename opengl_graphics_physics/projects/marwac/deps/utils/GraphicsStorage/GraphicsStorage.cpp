#include "GraphicsStorage.h"
#include "Material.h"
#include "Mesh.h"
#include "Texture2D.h"
#include <GL/glew.h>

GraphicsStorage::GraphicsStorage()
{
}

GraphicsStorage::~GraphicsStorage()
{
}

void GraphicsStorage::ClearMaterials()
{
	for (auto& mat : materials)
	{
		delete mat;
	}
	materials.clear();
}

void GraphicsStorage::ClearMeshes()
{
	for (auto& mesh : meshes)
	{
		delete mesh.second;
	}
	meshes.clear();
}

void GraphicsStorage::ClearTextures()
{
	for (auto& texture : textures)
	{
		delete texture;
	}
	textures.clear();
}

std::map<std::string, Mesh*> GraphicsStorage::meshes;
std::vector<Texture2D*> GraphicsStorage::textures;
std::vector<Material*> GraphicsStorage::materials;
std::vector<OBJ*> GraphicsStorage::objects;