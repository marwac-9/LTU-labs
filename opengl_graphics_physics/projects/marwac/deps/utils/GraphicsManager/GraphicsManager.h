class Mesh;
class Texture2D;
class OBJ;
class Material;

class GraphicsManager
{
	typedef unsigned int GLuint;
public:
	GraphicsManager();
	~GraphicsManager();
	static bool LoadOBJs(const char * path);
	static bool SaveToOBJ(OBJ *objMesh);
	static Mesh* LoadOBJToVBO(OBJ* object, Mesh* mesh);
	static void LoadAllOBJsToVBO();
	static bool LoadTextures(const char * path);
	static bool LoadMaterials(const char * path);
	static GLuint LoadBMP(const char *imagepath);
	static GLuint LoadDDS(const char *imagepath);
	static unsigned char * LoadPng(const char* path, int* x, int* y, int* numOfElements, int forcedNumOfEle);
	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
    static GLuint CreateTexture(int width, int height, bool isDepth, unsigned char* data);
	static void LoadAllAssets();
	static Texture2D* LoadTexture(char* path);
private:
};