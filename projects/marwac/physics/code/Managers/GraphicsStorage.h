#include <GL/glew.h>
#include <vector>
#include <map>
#include <string>

class Mesh;
class Texture2D;
class Material;
class OBJ;

class GraphicsStorage
{
public:
	GraphicsStorage();
	~GraphicsStorage();
	static std::map<std::string,Mesh*> meshes; //get mesh by name
	static std::vector<Texture2D*> textures;
	static std::vector<Material*> materials;
	static std::vector<OBJ*> objects;
	static void ClearMaterials();
private:

};

