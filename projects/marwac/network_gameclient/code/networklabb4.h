#ifndef NETWORKLABB4_H
#define NETWORKLABB4_H

#include <QtWidgets/QMainWindow>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include "ui_networklabb4.h"
#include <vector>

class ClientSocket;
class Connection;
struct PlayerInfo;
struct Player;
struct NewPlayerMsg;

class NetworkLabb4 : public QMainWindow
{
	Q_OBJECT

public:
	NetworkLabb4(QWidget *parent = 0);
	~NetworkLabb4();
	ClientSocket* socket = NULL;
	Connection* con = NULL;
	QGraphicsScene* scene;
	QString clientName;
	std::vector<QGraphicsItem*> playersGraphics;
	std::vector<PlayerInfo*> playersInfo;
	std::vector<Player*> players;

	void addPlayer(PlayerInfo* newPlayerMsg);
	void removePlayer(int id);
	void newPlayerPos(int id, int posx, int posy, int dirx, int diry);
	
	bool playerAlreadyExists(int id);
	int findPlayerIndex(int id);
	int id = NULL;	
	Player* localPlayer = NULL;
	bool crypt = false;
signals:
	void addItemToTheScene(QGraphicsItem* playerGraphics);
	void setPlayerPos(QGraphicsItem* item, int posx, int posy);
	void removePlayerFromScene(QGraphicsItem* item);
	void showInput();
	void hideInput();
private slots:
	void addItemToScene(QGraphicsItem* item);
	void setItemPos(QGraphicsItem* item, int posx, int posy);
	void removeItemFromScene(QGraphicsItem* item);
	void startConnectionThread();
	void hideNameInput();
	void showNameInput();

private:
	Ui::NetworkLabb4Class ui;
	void keyPressEvent(QKeyEvent *event);
	void closeEvent(QCloseEvent *event);
	QGraphicsItem* createSphere(PlayerInfo* info);
	Player* createPlayer(QGraphicsItem* graphics, PlayerInfo* info);
	void buildAndAddPlayer(QGraphicsItem* graphics, PlayerInfo* playerInfo);
	
	QTimer timer;
};

#endif // NETWORKLABB4_H
