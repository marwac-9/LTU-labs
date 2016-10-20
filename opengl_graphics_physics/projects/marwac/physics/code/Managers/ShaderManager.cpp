//
// Created by marwac-9 on 9/17/15.
//
#include "ShaderManager.h"
#include "GraphicsManager.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

ShaderManager* ShaderManager::Instance()
{
    static ShaderManager instance;

    return &instance;
}

GLuint ShaderManager::GetCurrentShaderID()
{
	return this->current_shader;
}

void ShaderManager::SetCurrentShader(GLuint id)
{
    this->current_shader = id;
	glUseProgram(id);
}

void ShaderManager::LoadShaders()
{
	shaderIDs["color"] = GraphicsManager::LoadShaders("Resources/Shaders/VertexShader.glsl", "Resources/Shaders/FragmentShader.glsl");
	shaderIDs["picking"] = GraphicsManager::LoadShaders("Resources/Shaders/VSPicking.glsl", "Resources/Shaders/FSPicking.glsl");
	shaderIDs["wireframe"] = GraphicsManager::LoadShaders("Resources/Shaders/VSBB.glsl", "Resources/Shaders/FSBB.glsl");
	shaderIDs["dftext"] = GraphicsManager::LoadShaders("Resources/Shaders/VSDFText.glsl", "Resources/Shaders/FSDFText.glsl");
}

void ShaderManager::DeleteShaders()
{ 
	for (auto& shader : shaderIDs)
	{
		glDeleteProgram(shader.second);
	}
	shaderIDs.clear();
}