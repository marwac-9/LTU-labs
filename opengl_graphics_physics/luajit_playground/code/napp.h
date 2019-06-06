#include "app.h"
struct lua_State;

class LuaJitPlayground : public Core::App
{
public:
    /// constructor
	LuaJitPlayground();
    /// destructor
    ~LuaJitPlayground();

    /// open app
    bool Open();
    /// run app
    void Run();

	void UpdateLuaScript(const char*);

	void LoadLuaFile(const char*, lua_State*);

	void StackTest(lua_State *);

	void StackTestClean(lua_State *);

	void CallFunctionMultiReturn(lua_State *);

private:
	
};
