#include "napp.h"

int
main(int argc, const char** argv)
{
	LuaJitPlayground app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();

}

