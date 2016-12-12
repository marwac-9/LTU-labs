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
	void SetUpPickingFrameBuffer(int windowWidth, int windowHeight);
	void SetUpDeferredFrameBuffer(int windowWidth, int windowHeight);
	void SetUpLightAndPostFrameBuffer(int windowWidth, int windowHeight);
	void SetUpBlurFrameBuffer(int windowWidth, int windowHeight);
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite, GLuint frameBuffer);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned int* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	void DrawGeometryMaps(int width, int height);

	GLuint pickingFrameBufferHandle; 
	GLuint pickingTextureHandle; 
	GLuint depthTextureHandle; 

	GLuint geometryFrameBufferHandle;
	GLuint positionBufferHandle;
	GLuint diffuseBufferHandle;
	GLuint normalBufferHandle;
	GLuint diffIntAmbIntShinSpecIntBufferHandle;
	GLuint geoDepthTextureHandle;

	GLuint lightAndPostFrameBufferHandle;
	GLuint finalColorTextureHandle;
	GLuint brightLightBufferHandle;
	GLuint lightDepthBufferHandle;

	GLuint blurFrameBufferHandle[2];
	GLuint blurBufferHandle[2];

private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};