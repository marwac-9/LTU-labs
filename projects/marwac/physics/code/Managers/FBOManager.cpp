#include "FBOManager.h"
#include <string>

FBOManager::FBOManager()
{
}

FBOManager::~FBOManager()
{
}

FBOManager* FBOManager::Instance()
{
	static FBOManager instance;

	return &instance;
}

void FBOManager::UpdateTextureBuffers(int windowWidth, int windowHeight)
{
	glBindTexture(GL_TEXTURE_2D, pickingTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight,
		0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, worldPosTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight,
		0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void FBOManager::SetUpFrameBuffer(int windowWidth, int windowHeight)
{
	//set up frame buffer

	// Create the FBO
	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	// Create the texture object for the primitive information buffer
	glGenTextures(1, &pickingTextureHandle);
	glBindTexture(GL_TEXTURE_2D, pickingTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight,
		0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		pickingTextureHandle, 0);

	// Create the texture object for the primitive information buffer
	glGenTextures(1, &worldPosTextureHandle);
	glBindTexture(GL_TEXTURE_2D, worldPosTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight,
		0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
		worldPosTextureHandle, 0);

	// Create the texture object for the depth buffer
	glGenTextures(1, &depthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depthTextureHandle, 0);

	// Disable reading to avoid problems with older GPUs
	glReadBuffer(GL_NONE);

	//        glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Verify that the FBO is correct
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return;
	}

	// Restore the default framebuffer
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBOManager::BindFrameBuffer(FrameBufferMode readWrite)
{
	switch (readWrite)
	{
	case read:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBufferHandle);
		break;
	case draw:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferHandle);
		break;
	case readDraw:
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);
		break;
	default:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	}
}

void FBOManager::UnbindFrameBuffer(FrameBufferMode readWrite)
{
	switch (readWrite)
	{
	case read:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		break;
	case draw:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		break;
	case readDraw:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	default:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	}
	
}

void FBOManager::ReadPixelID(unsigned int x, unsigned int y, unsigned char* data)
{
	BindFrameBuffer(read);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer(read);
}

void FBOManager::ReadWorldPos(unsigned int x, unsigned int y, float* data)
{
	BindFrameBuffer(read);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, data);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer(read);
}

void FBOManager::ReadPixelFromTexture(GLenum attachment, unsigned int x, unsigned int y, float* data){
	BindFrameBuffer(read);
	glReadBuffer(attachment);

	//inverted y coordinate because glfw 0,0 starts at topleft while opengl texture 0,0 starts at bottomleft
	glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &data);
	glReadBuffer(GL_NONE);
	UnbindFrameBuffer(read);
}

