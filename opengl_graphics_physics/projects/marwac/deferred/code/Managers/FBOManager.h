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
	void SetUpBlurFrameBuffer(int windowWidth, int windowHeight);
	void SetUpDeferredFrameBuffer(int windowWidth, int windowHeight);
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite);
	void BindBlurFrameBuffer(FrameBufferMode readWrite);
	void BindGeometryBuffer(FrameBufferMode readWrite);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned char* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	GLuint blurFrameBufferHandle;
	GLuint frameBufferHandle; //frame buffer handle
	GLuint pickingTextureHandle; //picking texture for frame buffer
	GLuint depthTextureHandle; //depth for frame buffer 
	GLuint shadowMapHandle;
	GLuint shadowMapBlurdHandle;

	GLuint geometryBufferHandle;
	GLuint positionBufferHandle;
	GLuint diffuseBufferHandle;
	GLuint normalBufferHandle;
	GLuint finalColorTextureHandle;

	GLuint specularColorBufferHandle;
	GLuint materialColorBufferHandle;
	GLuint diffIntAmbIntShinBufferHandle;

	void ReadPixelFromTexture(GLenum attachment, unsigned int x, unsigned int y, float* data);
private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};