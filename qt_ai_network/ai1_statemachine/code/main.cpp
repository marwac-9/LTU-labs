#include "ai_lab1_qt.h"
#include <QtWidgets/QApplication>
#include <time.h> 
int main(int argc, char *argv[])
{
	srand((unsigned)time(0));
	QApplication a(argc, argv);
	Ai_Lab1_qt w;
	w.show();
	return a.exec();
}
