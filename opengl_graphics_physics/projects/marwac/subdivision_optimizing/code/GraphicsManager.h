#include "MyMathLib.h"

class Mesh;
class OBJ;

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();
	static bool LoadOBJs(const char * path);
	static bool SaveToOBJ(OBJ *objMesh);
	static Mesh* LoadOBJToVBO(OBJ* object, Mesh* mesh);
	static void LoadAllOBJsToVBO();
	static bool LoadLevel(const char * path);
private:
};