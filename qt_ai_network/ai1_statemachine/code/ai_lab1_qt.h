#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_ai_lab1_qt.h"
class Sim;
class Location;
struct Shop;
class Ai_Lab1_qt : public QMainWindow
{
	Q_OBJECT

public:
	Ai_Lab1_qt(QWidget *parent = 0);
	~Ai_Lab1_qt();
	QTimer timer;
	QGraphicsScene* scene;
	QGraphicsScene* previewScene;
	QGraphicsEllipseItem* ellipse;
	QGraphicsRectItem* rectangle;
	void addSim(Sim* sim);
	void addLocation(Location* loc);
public slots:
	void updateEntities();
	void setTimerInterval();
	void setSimInFocus();
	void toggleTimePause();
	void decreaseCurrentSimLoneliness();
	void increaseCurrentSimLoneliness();
	void decreaseCurrentSimHunger();
	void increaseCurrentSimHunger();
	void decreaseCurrentSimThirst();
	void increaseCurrentSimThirst();
	void decreaseCurrentSimFatigue();
	void increaseCurrentSimFatigue();
	void setCurrentSimMoney(int newMoneyValue);
	void addSimMessage(QString message, Sim* sim);
private:
	void updateWorld();
	void updateGuiSimStats();
	void addSimsToTheScene();
	void addLocationsToTheScene();
	void disableEditing();
	void enableEditing();
	QGraphicsPixmapItem* loadImage(const char * path);
	QGraphicsItem* previewSim;
	Sim* selectedSim;
	bool Paused = false;
	Ui::Ai_Lab1_qtClass ui;
};
