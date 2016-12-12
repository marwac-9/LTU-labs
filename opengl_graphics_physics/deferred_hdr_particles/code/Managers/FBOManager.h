#pragma once
#include <GL/glew.h>

enum FrameBufferMode
{
	read,
	draw,
	readDraw
};
class FBOManager
{
public:
	static FBOManager* Instance();
	void SetUpFrameBuffer(int windowWidth, int windowHeight);
	void SetUpDeferredFrameBuffer(int windowWidth, int windowHeight);
	void SetUpLightAndPostFrameBuffer(int windowWidth, int windowHeight);
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite);
	void BindGeometryBuffer(FrameBufferMode readWrite);
	void BindLightPostBuffer(FrameBufferMode readWrite);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned int* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	void DrawGeometryMaps(int width, int height);
	
	GLuint frameBufferHandle; 
	GLuint pickingTextureHandle; 
	GLuint depthTextureHandle; 

	GLuint geometryBufferHandle;
	GLuint positionBufferHandle;
	GLuint diffuseBufferHandle;
	GLuint normalBufferHandle;
	GLuint geoDepthTextureHandle;

	GLuint lightAndPostBufferHandle;
	GLuint finalColorTextureHandle;
	GLuint brightLightBufferHandle;
	GLuint lightDepthBufferHandle;

	GLuint diffIntAmbIntShinSpecIntBufferHandle;

private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};