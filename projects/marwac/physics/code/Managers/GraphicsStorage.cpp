#include "GraphicsStorage.h"
#include "Material.h"

GraphicsStorage::GraphicsStorage()
{
}

GraphicsStorage::~GraphicsStorage()
{
}

void GraphicsStorage::ClearMaterials()
{
	for (auto& mat : GraphicsStorage::materials)
	{
		delete mat;
	}
	GraphicsStorage::materials.clear();
}

std::map<std::string, Mesh*> GraphicsStorage::meshes;
std::vector<Texture2D*> GraphicsStorage::textures;
std::vector<Material*> GraphicsStorage::materials;
std::vector<OBJ*> GraphicsStorage::objects;