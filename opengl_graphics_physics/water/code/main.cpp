#include "SimpleWaterApp.h"

int
main(int argc, const char** argv)
{
	SimpleWater::SimpleWaterApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();
}