//
// Created by marwac-9 on 9/17/15.
//
#include <string>
#include <map>

class ShaderManager
{
	typedef unsigned int GLuint;
public:
    static ShaderManager* Instance();
    GLuint GetCurrentShaderID();
    void SetCurrentShader(GLuint);
	void DeleteShaders();
	void AddShader(const char* shaderName, GLuint shader);
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