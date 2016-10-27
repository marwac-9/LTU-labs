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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	//glBindTexture(GL_TEXTURE_2D, shadowMapHandle);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, windowWidth, windowHeight, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, positionBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, diffuseBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, normalBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, diffIntAmbIntShinBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, finalColorTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, materialColorBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, specularColorBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
}

void FBOManager::SetUpDeferredFrameBuffer(int windowWidth, int windowHeight)
{
	//set up frame buffer

	// Create the FBO
	glGenFramebuffers(1, &geometryBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, geometryBufferHandle);
	//int maxColorAttachments;
	//glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGenTextures(1, &positionBufferHandle);
	glBindTexture(GL_TEXTURE_2D, positionBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionBufferHandle, 0);

	glGenTextures(1, &diffuseBufferHandle);
	glBindTexture(GL_TEXTURE_2D, diffuseBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, diffuseBufferHandle, 0);

	glGenTextures(1, &normalBufferHandle);
	glBindTexture(GL_TEXTURE_2D, normalBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, normalBufferHandle, 0);

	glGenTextures(1, &diffIntAmbIntShinBufferHandle);
	glBindTexture(GL_TEXTURE_2D, diffIntAmbIntShinBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, diffIntAmbIntShinBufferHandle, 0);

	glGenTextures(1, &finalColorTextureHandle);
	glBindTexture(GL_TEXTURE_2D, finalColorTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, finalColorTextureHandle, 0);

	glGenTextures(1, &materialColorBufferHandle);
	glBindTexture(GL_TEXTURE_2D, materialColorBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, materialColorBufferHandle, 0);

	glGenTextures(1, &specularColorBufferHandle);
	glBindTexture(GL_TEXTURE_2D, specularColorBufferHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, specularColorBufferHandle, 0);

	glGenTextures(1, &depthTextureHandle);
	glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthTextureHandle, 0);
	
	// Disable reading to avoid problems with older GPUs
	//can't read unless you specify it later
	glReadBuffer(GL_NONE);
	//disable draw if we don't want to draw to all
	//glDrawBuffer(GL_NONE);
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

void FBOManager::SetUpFrameBuffer(int windowWidth, int windowHeight)
{
	//set up frame buffer

	// Create the FBO
	glGenFramebuffers(1, &frameBufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferHandle);

	glGenTextures(1, &pickingTextureHandle);
	glBindTexture(GL_TEXTURE_2D, pickingTextureHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTextureHandle, 0);

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
	//glGenTextures(1, &depthTextureHandle);
	//glBindTexture(GL_TEXTURE_2D, depthTextureHandle);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,	depthTextureHandle, 0);


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
	BindGeometryBuffer(read);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
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

void FBOManager::BindGeometryBuffer(FrameBufferMode readWrite)
{
	switch (readWrite)
	{
	case read:
		glBindFramebuffer(GL_READ_FRAMEBUFFER, geometryBufferHandle);
		break;
	case draw:
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, geometryBufferHandle);
		break;
	case readDraw:
		glBindFramebuffer(GL_FRAMEBUFFER, geometryBufferHandle);
		break;
	default:
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		break;
	}
}

