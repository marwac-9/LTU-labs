//
// Created by marwac-9 on 9/17/15.
//
#include <GL/glew.h>
#include <string>
#include <map>

class ShaderManager
{
public:
    static ShaderManager* Instance();
    GLuint GetCurrentShaderID();
    void SetCurrentShader(GLuint);
	void DeleteShaders();
	void LoadShaders();
	std::map<std::string, GLuint> shaderIDs;
private:
    ShaderManager();
    ~ShaderManager();
    //copy
    ShaderManager(const ShaderManager&);
    //assign
    ShaderManager& operator=(const ShaderManager&);

    GLuint current_shader;

};