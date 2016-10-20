#include <GL/glew.h>
#include <vector>
class Mesh;
class OBJ;

class GraphicsStorage
{
public:
	GraphicsStorage();
	~GraphicsStorage();
	static std::vector<Mesh*> meshes;
	static std::vector<OBJ*> objects;
private:

};

