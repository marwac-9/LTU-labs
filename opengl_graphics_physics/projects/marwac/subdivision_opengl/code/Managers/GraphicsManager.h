#include <GL/glew.h>
#include <vector>
#include <map>
#include "MyMathLib.h"
#include "Scene.h"
class Mesh;
class Texture2D;
class OBJ;
class Material;

class GraphicsManager
{
public:
	GraphicsManager();
	~GraphicsManager();
	static bool LoadOBJs(const char * path);
	static bool SaveToOBJ(OBJ *objMesh);
	static Mesh* LoadOBJToVBO(OBJ* object, Mesh* mesh);
	static void LoadAllOBJsToVBO();
	static bool LoadTextures(const char * path, GLuint programID);
	static bool LoadMaterials(const char * path);
	static bool LoadLevel(const char * path);
	static GLuint LoadBMP(const char *imagepath);
	static GLuint LoadDDS(const char *imagepath);
	static unsigned char * LoadPng(const char* path, int* x, int* y, int* numOfElements, int forcedNumOfEle);
	static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	static unsigned int CreateTexture(int width, int height, bool isDepth, unsigned char* data);
	static void LoadAllAssets();
	static Texture2D* LoadTexture(char* path, const GLuint programID);
private:
};