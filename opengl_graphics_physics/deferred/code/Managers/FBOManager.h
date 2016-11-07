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
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite);
	void BindGeometryBuffer(FrameBufferMode readWrite);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned int* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	void DrawGeometryMaps(int width, int height);
	
	GLuint frameBufferHandle; //frame buffer handle
	GLuint pickingTextureHandle; //picking texture for frame buffer
	GLuint geoDepthTextureHandle; //depth for gbuffer 
	GLuint depthTextureHandle; //depth for frame buffer 
	GLuint shadowMapHandle;

	GLuint geometryBufferHandle;
	GLuint positionBufferHandle;
	GLuint diffuseBufferHandle;
	GLuint normalBufferHandle;
	GLuint finalColorTextureHandle;

	GLuint diffIntAmbIntShinSpecIntBufferHandle;

private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};