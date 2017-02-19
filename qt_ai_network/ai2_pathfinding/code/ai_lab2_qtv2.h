#ifndef AI_LAB2_QTV2_H
#define AI_LAB2_QTV2_H

#include <QtWidgets/QMainWindow>
#include "ui_ai_lab2_qtv2.h"
#include "QTime"
#include "QDebug"
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QListWidgetItem>
#include <QTimer>
#include "Face.h"
#include <vector>
#include <string>
#include <list>
#include "Vector.h"

class HalfEdgeMesh2D;

class Ai_Lab2_qtv2 : public QMainWindow
{
	Q_OBJECT

public:
	Ai_Lab2_qtv2(QWidget *parent = 0);
	~Ai_Lab2_qtv2();
	QGraphicsScene* scene;
	int cellSize = 20;
public slots:
	void loadSelectedMap();
	void optimize();
	void quad(int state);
	void calculateAndDrawPath();
	void drawmap();
private:
	Ui::Ai_Lab2_qtv2Class ui;
	void drawfaces(const cop4530::Vector<Face*> &faces, QColor &color);
	void drawgoals(const cop4530::Vector<Face*> &faces, QColor &color);
	void clearMap();
	void drawfaces(const std::list<Face*> &faces, QColor &color);
	void drawPathFaces(const std::vector<Face*>& faces, QColor &color);
	void drawPathFaces(const std::list<Face*>& faces, QColor &color);
	QGraphicsPolygonItem* drawface(Face* face, QColor &color);
	void drawpath(const std::vector<Face*> &path, const mwm::Vector2 &start, const mwm::Vector2 &end);
	HalfEdgeMesh2D* mesh = NULL;
	std::vector<QGraphicsPolygonItem*> polygons;
	std::vector<QGraphicsPolygonItem*> goals;
	QGraphicsPolygonItem* start;
	QPolygonF buildPolygon(Face* face);
	std::vector<QGraphicsLineItem*> linepath;
	std::vector<QGraphicsPolygonItem*> facepath;
	std::vector<QString> mapPaths;
	void loadMapsPaths(const char*);
	void clearScene();
	float calculatePathLength(const std::vector<Face*> &path);
	int indexOfShortestGoal(const std::vector<float> &lengths, float mini);
};

#endif // AI_LAB2_QTV2_H
