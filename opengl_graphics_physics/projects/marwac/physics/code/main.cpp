#include "config.h"
#include "PickingApp.h"

int
main(int argc, const char** argv)
{
	Picking::PickingApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();

}

