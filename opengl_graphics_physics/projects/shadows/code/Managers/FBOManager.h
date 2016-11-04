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
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite);
	void BindBlurFrameBuffer(FrameBufferMode readWrite);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned char* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	void DrawShadowMap(int width, int height);
	GLuint blurFrameBufferHandle;
	GLuint frameBufferHandle; //frame buffer handle
	GLuint pickingTextureHandle; //picking texture for frame buffer
	GLuint worldPosTextureHandle;
	GLuint depthTextureHandle; //depth for frame buffer 
	GLuint shadowMapHandle;
	GLuint shadowMapBlurdHandle;
	void ReadPixelFromTexture(GLenum attachment, unsigned int x, unsigned int y, float* data);
private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};