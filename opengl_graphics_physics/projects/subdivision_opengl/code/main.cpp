#include "SubdivApp.h"

int
main(int argc, const char** argv)
{
	Subdivision::SubdivisionApp app;
	if (app.Open())
	{
		app.Run();
		app.Close();
	}
	app.Exit();

}