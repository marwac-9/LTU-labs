#include "FBOManager.h"
#include <string>
#include "ShaderManager.h"
#include "DebugDraw.h"

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, windowWidth, windowHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, worldPosTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, shadowMapHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, shadowMapBlurdHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void FBOManager::SetUpFrameBuffer(int windowWidth, int windowHeight)
{
	//set up frame buffer

	// Create the FBO
	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	glGenTextures(1, &pickingTextureHandle);
	glBindTexture(GL_TEXTURE_2D, pickingTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, windowWidth, windowHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTextureHandle, 0);

	glGenTextures(1, &worldPosTextureHandle);
	glBindTexture(GL_TEXTURE_2D, worldPosTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, worldPosTextureHandle, 0);

	glGenTextures(1, &shadowMapHandle);
	glBindTexture(GL_TEXTURE_2D, shadowMapHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, NULL);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, shadowMapHandle, 0);

	// Create the texture object for the depth buffer
	glGenTextures(1, &depthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,	depthTextureHandle, 0);


	//depth
	//glGenRenderbuffers(1, &depthTextureHandle);
	// Bind the depth buffer
	//glBindRenderbuffer(GL_RENDERBUFFER, depthTextureHandle);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, windowWidth, windowHeight);
	//glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTextureHandle);
	

	// Disable reading to avoid problems with older GPUs
	glReadBuffer(GL_NONE);
	//no color buffer
	glDrawBuffer(GL_NONE);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_NONE);
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

void FBOManager::SetUpBlurFrameBuffer(int windowWidth, int windowHeight)
{
	//set up blur frame buffer

	// Create the FBO
	glGenFramebuffers(1, &blurFrameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBufferHandle);

	glGenTextures(1, &shadowMapBlurdHandle);
	glBindTexture(GL_TEXTURE_2D, shadowMapBlurdHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, 0);
	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowMapBlurdHandle, 0);

	// Disable reading to avoid problems with older GPUs
	glReadBuffer(GL_NONE);
	//no color buffer
	glDrawBuffer(GL_NONE);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_NONE);
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

void FBOManager::BindBlurFrameBuffer(FrameBufferMode readWrite)
{
	switch (readWrite)
	{
	case read:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, blurFrameBufferHandle);
		break;
	case draw:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blurFrameBufferHandle);
		break;
	case readDraw:
		glBindFramebuffer(GL_FRAMEBUFFER, blurFrameBufferHandle);
		break;
	default:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	}
}

void FBOManager::ReadPixelID(unsigned int x, unsigned int y, unsigned int* data)
{
	BindFrameBuffer(read);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, data);
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

void FBOManager::DrawShadowMap(int width, int height)
{
	//Quad render

	ShaderManager::Instance()->SetCurrentShader(ShaderManager::Instance()->shaderIDs["depthPanel"]);
	//Enable Scissor box to only the clear the color buffer and depth buffer for it
	float fHeight = (float)height;
	float fWidth = (float)width;
	int glWidth = (int)(fWidth *0.15f);
	int glHeight = (int)(fHeight*0.20f);
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, glWidth, glHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, glWidth, glHeight);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMapHandle);

	DebugDraw::Instance()->DrawQuad();
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, width, height);
}