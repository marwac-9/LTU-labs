//
// Created by marwac-9 on 9/16/15.
//

#include "napp.h"
#include <iostream>
extern "C" {
	//#include "include/lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "luajit.h"
//#include "include/luaconf.h"
}

#include "LuaTools.h"

static int on_recv(lua_State *L, char *buf, size_t len)
{
	lua_getglobal(L, "callback");
	lua_pushlstring(L, buf, len); /* Binary strings are okay */
	int ret = lua_pcall(L, 1, 1, 0); /* 1 argument, 1 result */
	printf("ret: %d, buflen: %ld\n", ret, lua_tointeger(L, -1));
	lua_pop(L, 1);
	return ret;
}

static int on_recv_using_fast_registry(lua_State *L, int cb_ref, char *buf, size_t len)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, cb_ref);
	lua_pushlstring(L, buf, len);
	int ret = lua_pcall(L, 1, 1, 0);
	printf("ret: %d, buflen: %ld\n", ret, lua_tointeger(L, -1));
	lua_pop(L, 1);
	return ret;
}

/* Get/set opcode */
static int msg_op(lua_State *L)
{
	char *msg = (char*)lua_touserdata(L, 1);
	if (lua_isnumber(L, 2)) {
		msg[0] = lua_tointeger(L, 2) & 0xFF;
	}
	lua_pushnumber(L, msg[0]);
	return 1;
}
/* Register 'msg' meta table */
static int msg_register(lua_State *L)
{
	static const luaL_Reg wrap[] = {
		{ "op", &msg_op },
	{ NULL, NULL }
	};
	luaL_newmetatable(L, "msg");
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_openlib(L, NULL, wrap, 0);
	lua_pop(L, 1);
	return 0;
}

static int on_recv_fast_meta(lua_State *L, int cb_ref, char *buf, size_t len)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, cb_ref);
	lua_pushlightuserdata(L, buf);
	luaL_getmetatable(L, "msg");
	lua_setmetatable(L, -2);
	return lua_pcall(L, 1, 1, 0);
}
//----------------------------------------------
//defining functions to be used by LUA
int LUA_Square(lua_State*S)
{
	float x = lua_tonumber(S, 1); //get variable passed in lua to Square function
	lua_pushnumber(S, x*x); //we send the result to stack
	return 1; //we tell lua we return one value
}
//we must now register the function 
//lua_register(S, "Square", LUA_Square) //arguments: lua_State, name used in lua to call the function, name of the static C function to be called by lua
//and then we can call it from lua by the defined name "Square"
//----------------------------------------------

//----------------------------------------------
//light user data, allow lua to access c++ objects via pointer
//define a class
class LuaAccessibleClass
{
public:
	int r, i;
	LuaAccessibleClass(int r, int i) :r(r), i(i) {};
	~LuaAccessibleClass() {};
	void SetRectangular(int r, int i);
	void GetRectangular(int &outA, int &outB);//to return multiple values we have to pass them in the function as parameters, or maybe return a struct(another user data?)
private:

};

void LuaAccessibleClass::SetRectangular(int r, int i)
{
	this->r = r;
	this->i = i;
}
void LuaAccessibleClass::GetRectangular(int &outR, int &outI)
{
	outR = this->r;
	outI = this->i;
}

//define a function that will create the class object
int LUA_CreateClassObject(lua_State* S)
{
	LuaAccessibleClass* pointer = new LuaAccessibleClass(lua_tonumber(S, 1), lua_tonumber(S, 2)); //when the function is called by lua it will send parameters to the C function and we get them here with tonumber
	lua_pushlightuserdata(S, pointer); //we send the pointer  to lua
	return 1;
}
//since we create a new object dynamically we have to consider how to delete, free up the memory later on when we don't need the object anymore
int LUA_DestroyClassObject(lua_State* S)
{
	LuaAccessibleClass* pointer = (LuaAccessibleClass*)lua_touserdata(S, 1);
	delete pointer;
	return 0;
}
//define function that we will bind to lua that will call class object function, so we are not calling the function directly in lua but we call a function in C that will get the object and call the correct function
int LUA_SetRectangular(lua_State* S)
{
	LuaAccessibleClass* pointer = (LuaAccessibleClass*)lua_touserdata(S, 1); //we get the object from the stack
	pointer->SetRectangular(lua_tonumber(S,2), lua_tonumber(S,3)); //we get the parameters from the stack
	return 0;
}
//define a fucntin that we will bind to lua that will call class object function, it will return multiple variables
int LUA_GetRectangular(lua_State* S)
{
	LuaAccessibleClass* pointer = (LuaAccessibleClass*)lua_touserdata(S, 1); //we get the object from the stack
	int r, i;
	pointer->GetRectangular(r, i); //we get the parameters from the stack
	lua_pushnumber(S, r);
	lua_pushnumber(S, i);
	return 2;
}
//now we must register everything
//lua_register(S, "CreateObject", LUA_CreateClassObject);
//lua_register(S, "DestroyObject", LUA_DestroyClassObject);
//lua_register(S, "ObjectSetRectangular", LUA_SetRectangular);
//lua_register(S, "ObjectGetRectangular", LUA_GetRectangular);

//in lua we call 
//myObject1 = CreateObject(10,10)
//myObject2 = CreateObject(20,20)
//myObject3 = CreateObject(30,30)
//DestroyObject(myObject1) //we don't call functions on objects like in C, we call the function and pass the object instead that's how we always know the stack position of the object and it's parameters
//a,b = GetRectangular(myObject2)
//SetRectangular(myObject3, 40,40)
//----------------------------------------------

//----------------------------------------------
//reverse binding accessing lua functions from C++
struct color
{
	char r, g, b;
};
//this function 
void LoadColorPallet(lua_State* S, std::string str, color*& pallet)
{
	//lua_pushstring(S, str.c_str());//we push the name on the stack
	//lua_gettable(S, LUA_GLOBALSINDEX);//we get the global variable with the specified name we pushed to lua
	lua_getglobal(S, str.c_str());//shortcut, get lua variable with name specified by the parameter  //table
	lua_pushstring(S, "colors"); //colors will be a function that returns the four colors of the specified pallete //talbe, "colors"
	lua_gettable(S, -2); //function colors
	lua_call(S, 0, 4); //we call function colors, stack: table, color 1, color 2, color 3, color 4
	pallet[0] = *(color*)lua_touserdata(S, -4); //we get colors from lua
	pallet[1] = *(color*)lua_touserdata(S, -3); //we get colors from lua
	pallet[2] = *(color*)lua_touserdata(S, -2); //we get colors from lua
	pallet[3] = *(color*)lua_touserdata(S, -1); //we get colors from lua
	lua_pop(S, 5);
}

int LUA_MakeColor(lua_State* S)
{
	color* c = (color*)lua_newuserdata(S, sizeof(color)); //instead of creating the color in C++ we tell lua to allocate the memory for the object so we can use garbage collector for deleting the object
	new (c) color; //we got the memory allocated but we have to construct the object, we do that in C++
	c->r = lua_tointeger(S, 1); //we get parameters from lua
	c->g = lua_tointeger(S, 2); //we get parameters from lua
	c->b = lua_tointeger(S, 3); //we get parameters from lua
	return 1; //we return one object
}

//no need to delete the color manually because lua garbage collector will take care of it since the object was created by lua
int LUA_DestroyColor(lua_State* S)
{
	delete (color*)lua_touserdata(S, 1);
	return 0;
}

//we need to register functions
//lua_register(S, "MakeColor", LUA_MakeColor);
//in C++ we can call LoadColorPallet and pass the name of the table we want and the array of colors we want to fill
//in lua we define the different color palletes by calling MakeColor for each color of the pallete and return them
//ex
//basic = { //basic is the pallete name
//	colors = function()  //colors is the function name
//		return MakeColor(255,255,255), MakeColor(255,0,255), MakeColor(0,0,255), MakeColor(0,255,0)
//end } etc

//we can use this to call lua functions
//functions that will allocate objects on lua stack
//construct and fill with the data defined in lua
//----------------------------------------------

LuaJitPlayground::LuaJitPlayground()
{
}

LuaJitPlayground::~LuaJitPlayground()
{
}

bool
LuaJitPlayground::Open()
{
    App::Open();
	return true;
}

void
LuaJitPlayground::Run()
{
	std::string input;
	std::string previnput;
	while (true)
	{
		std::getline(std::cin, input);
		if (input.size() == 0)
			if (previnput.size() == 0)
				return;
			else
				UpdateLuaScript(previnput.c_str());
		else
			previnput = input;
		UpdateLuaScript(previnput.c_str());
	}
}

void
LuaJitPlayground::UpdateLuaScript(const char* filename)
{
	lua_State *L;

	/*
	* All Lua contexts are held in this structure. We work with it almost
	* all the time.
	*/
	L = luaL_newstate();
	
	luaL_openlibs(L); /* Load Lua libraries */

	LoadLuaFile(filename, L);
	
	//lua_call(L, 0, 0);
	//CallFunctionMultiReturn(L);
	int status = LuaTools::docall(L, 0, 1);
	LuaTools::report(L, status);
	/* Get the returned value at the top of the stack (index -1) */
	double sum = lua_tonumber(L, -1);

	printf("Script returned: %.0f\n", sum);

	lua_pop(L, 1);  /* Take the returned value out of the stack */
	lua_close(L);   /* Cya, Lua */
}

void
LuaJitPlayground::LoadLuaFile(const char * filename, lua_State* L)
{
	
	std::string directorywithfilename = "../resources/lua/tests/";
	directorywithfilename.append(filename);
	directorywithfilename.append(".lua");

	/* Load the file containing the script we are going to run */
	int status = luaL_loadfile(L, directorywithfilename.c_str());
	if (status) {
		/* If something went wrong, error message is at the top of */
		/* the stack */
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		//exit(1);
	}
}

void
LuaJitPlayground::StackTest(lua_State * L)
{
	/*
	* Ok, now here we go: We pass data to the lua script on the stack.
	* That is, we first have to prepare Lua's virtual stack the way we
	* want the script to receive it, then ask Lua to run it.
	*/
	lua_newtable(L);    /* We will pass a table */

						/*
						* To put values into the table, we first push the index, then the
						* value, and then call lua_rawset() with the index of the table in the
						* stack. Let's see why it's -3: In Lua, the value -1 always refers to
						* the top of the stack. When you create the table with lua_newtable(),
						* the table gets pushed into the top of the stack. When you push the
						* index and then the cell value, the stack looks like:
						*
						* <- [stack bottom] -- table, index, value [top]
						*
						* So the -1 will refer to the cell value, thus -3 is used to refer to
						* the table itself. Note that lua_rawset() pops the two last elements
						* of the stack, so that after it has been called, the table is at the
						* top of the stack.
						*/
	for (int i = 1; i <= 5; i++) {
		lua_pushnumber(L, i);   /* Push the table index */
		lua_pushnumber(L, i * 2); /* Push the cell value */
		lua_rawset(L, -3);      /* Stores the pair in the table */
	}
	LuaTools::StackDump(L);
	/* By what name is the script going to reference our table? */
	lua_setglobal(L, "foo");

	/* Ask Lua to run our little script */
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	if (result) {
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}

	/* Get the returned value at the top of the stack (index -1) */
	double sum = lua_tonumber(L, -1);

	printf("Script returned: %.0f\n", sum);

	lua_pop(L, 1);  /* Take the returned value out of the stack */


					/* Call a callback for mock datagram. */
	char buf[512] = { 0x05, 'h', 'e', 'l', 'l', 'o' };
	on_recv(L, buf, sizeof(buf));


	lua_getglobal(L, "callback");
	int cb_ref = luaL_ref(L, LUA_REGISTRYINDEX);//store reference for next time call

	on_recv_using_fast_registry(L, cb_ref, buf, sizeof(buf));


	/* Read out config */
	lua_getglobal(L, "address"); /* (2) */
	lua_getglobal(L, "port");

	printf("address: %s, port: %ld\n", /* (3) */
		lua_tostring(L, -2), lua_tointeger(L, -1));
	lua_settop(L, 0); /* (4) */



					  //lua_getglobal(L, "callbackNew");
					  //int cb_ref2 = luaL_ref(L, LUA_REGISTRYINDEX);//store reference for next time call
					  //on_recv_fast_meta(L, cb_ref2, buf, sizeof(buf));
					  //msg_register(L); /* Register 'msg' meta */
}

void
LuaJitPlayground::StackTestClean(lua_State *L)
{
	/*
	* Ok, now here we go: We pass data to the lua script on the stack.
	* That is, we first have to prepare Lua's virtual stack the way we
	* want the script to receive it, then ask Lua to run it.
	*/
	lua_newtable(L);    /* We will pass a table */

						/*
						* To put values into the table, we first push the index, then the
						* value, and then call lua_rawset() with the index of the table in the
						* stack. Let's see why it's -3: In Lua, the value -1 always refers to
						* the top of the stack. When you create the table with lua_newtable(),
						* the table gets pushed into the top of the stack. When you push the
						* index and then the cell value, the stack looks like:
						*
						* <- [stack bottom] -- table, index, value [top]
						*
						* So the -1 will refer to the cell value, thus -3 is used to refer to
						* the table itself. Note that lua_rawset() pops the two last elements
						* of the stack, so that after it has been called, the table is at the
						* top of the stack.
						*/
	for (int i = 1; i <= 5; i++) {
		lua_pushnumber(L, i);   /* Push the table index */
		lua_pushnumber(L, i * 2); /* Push the cell value */
		lua_rawset(L, -3);      /* Stores the pair in the table */
	}

	/* By what name is the script going to reference our table? */
	lua_setglobal(L, "foo");
}

void
LuaJitPlayground::CallFunctionMultiReturn(lua_State *L)
{
	/* Ask Lua to run our little script */
	int result = lua_pcall(L, 0, LUA_MULTRET, 0);
	LuaTools::report(L, result);
	/*
	if (result) {
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		//exit(1);
	}
	*/
}
