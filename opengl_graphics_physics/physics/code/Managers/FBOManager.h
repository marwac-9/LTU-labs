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
	void UpdateTextureBuffers(int windowWidth, int windowHeight);
	void BindFrameBuffer(FrameBufferMode readWrite);
	void UnbindFrameBuffer(FrameBufferMode readWrite);
	void ReadPixelID(unsigned int x, unsigned int y, unsigned int* data);
	void ReadWorldPos(unsigned int x, unsigned int y, float* data);
	GLuint frameBufferHandle; //frame buffer handle
	GLuint pickingTextureHandle; //picking texture for frame buffer
	GLuint worldPosTextureHandle;
	GLuint depthTextureHandle; //depth for frame buffer

private:
	FBOManager();
	~FBOManager();
	//copy
	FBOManager(const FBOManager&);
	//assign
	FBOManager& operator=(const FBOManager&);
};