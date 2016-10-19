#include "networklabb4.h"
#include "Protocol.h"
#include "ClientSocket.h"
#include "Connection.h"
#include "PlayerInfo.h"
#include "Player.h"
#include <stdio.h>
#include <windows.h>
#include <process.h>         // needed for _beginthread()
void  connectingThread(void *);       // function prototype

NetworkLabb4::NetworkLabb4(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//setup gui
	printf("setting up giu\n");
	scene = new QGraphicsScene();
	ui.graphicsView->setScene(scene);
	ui.lineEdit->setMaxLength(MAXNAMELEN);
	connect(this, SIGNAL(addItemToTheScene(QGraphicsItem*)), this, SLOT(addItemToScene(QGraphicsItem*)));
	connect(this, SIGNAL(setPlayerPos(QGraphicsItem*, int, int)), this, SLOT(setItemPos(QGraphicsItem*, int, int)));
	connect(this, SIGNAL(removePlayerFromScene(QGraphicsItem*)), this, SLOT(removeItemFromScene(QGraphicsItem*)));
	connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(startConnectionThread()));	
	ui.graphicsView->hide();
	//connect(ui.lineEdit, SIGNAL(editingFinished()), this, SLOT(startConnectionThread()));	
	//editingFinished() signal is bugged in qt since before v4.7 one can block the signal
	//example
	/*
	if (ui.lineEdit->text().toDouble() < 1000) {
		ui.lineEdit->blockSignals(true);
		QMessageBox::information(this, "Information", "Incorrect value");
		ui.lineEdit->blockSignals(false);
	}
	*/
	QBrush brush(Qt::transparent);
	QPen transPen(Qt::transparent);
	QGraphicsRectItem* box1 = scene->addRect(-150, -150, 10, 10, transPen, brush);
	QGraphicsRectItem* box4 = scene->addRect(250, 250, 10, 10, transPen, brush);
	connect(this, SIGNAL(hideInput()), this, SLOT(hideNameInput()));
	connect(this, SIGNAL(showInput()), this, SLOT(showNameInput()));

	//show gui
}

NetworkLabb4::~NetworkLabb4()
{

}

void NetworkLabb4::addPlayer(PlayerInfo* playerInfo)
{
	if (playerInfo->form == Cube)
	{
		QGraphicsItem* ellipse = createSphere(playerInfo);
		buildAndAddPlayer(ellipse, playerInfo);
	}
	else if (playerInfo->form == Sphere)
	{
		QGraphicsItem* ellipse = createSphere(playerInfo);
		buildAndAddPlayer(ellipse, playerInfo);
	}
	else if (playerInfo->form == Pyramid)
	{
		QGraphicsItem* ellipse = createSphere(playerInfo);
		buildAndAddPlayer(ellipse, playerInfo);
	}
	else if (playerInfo->form == Cone)
	{
		QGraphicsItem* ellipse = createSphere(playerInfo);
		buildAndAddPlayer(ellipse, playerInfo);
	}
	else
	{
		cout << "\nWrong player form" << endl;
		delete playerInfo;
		//return -1 to delete player info
	}
}

void NetworkLabb4::removePlayer(int id)
{
	int index = findPlayerIndex(id);
	if (index != -1)
	{
		emit removeItemFromScene(playersGraphics.at(index));
		playersGraphics.erase(playersGraphics.begin() + index);
		playersInfo.erase(playersInfo.begin() + index);
		players.erase(players.begin() + index);
	}
}

void NetworkLabb4::removeItemFromScene(QGraphicsItem* item)
{
	scene->removeItem(item);
}

void NetworkLabb4::startConnectionThread()
{
	//to avoid blocking the gui we try to connect to server in another thread
	clientName = ui.lineEdit->text();
	if (clientName.size() > 0)
	{
		//ui.lineEdit->blockSignals(true); // this is a workaround for editingFinished() bug in qt calling twice the slot function
		ui.lineEdit->setDisabled(true);
		
		//create socket
		socket = new ClientSocket("130.240.40.26", 49152);
		//when successful connect
		if (socket->Connect() == 0)
		{
			emit hideInput();
			ui.graphicsView->show();
			con = new Connection(this);
			_beginthread(Connection::run, 0, (void*)this);

			QBrush brush(Qt::transparent);
			QPen pen(Qt::black);
			
			// Add world border to the scene
			QGraphicsRectItem* worldBorder = scene->addRect(-100, -100, 300, 300, pen, brush);
		}
		else
		{
			emit showInput();
		}
		//ui.lineEdit->blockSignals(false);
	}
}

void NetworkLabb4::newPlayerPos(int id, int posx, int posy, int dirx, int diry)
{
	cout << "\nNew posx: " << posx << endl;
	cout << "\nNew posy: " << posy << endl;
	int index = findPlayerIndex(id);
	if (index != -1)
	{
		emit setPlayerPos(playersGraphics.at(index), posx, posy);
	}
}

void NetworkLabb4::hideNameInput()
{
	ui.lineEdit->setHidden(true);
	ui.label->setHidden(true);
}

void NetworkLabb4::showNameInput()
{
	ui.lineEdit->setDisabled(false);
	ui.lineEdit->setHidden(false);
	ui.label->setHidden(false);
}

void NetworkLabb4::setItemPos(QGraphicsItem* item, int posx, int posy)
{
	item->setPos(posx, posy);
	//temp
	if (localPlayer != NULL)
	{
		cout << "Local Player posx: " << localPlayer->graphic->pos().x() << endl;
		cout << "Local Player posy: " << localPlayer->graphic->pos().y() << endl;
	}
}

int NetworkLabb4::findPlayerIndex(int pid)
{
	for (size_t i = 0; i < playersInfo.size(); i++)
	{
		if (playersInfo.at(i)->id == pid)
			return i;
	}
	return -1;
}

void NetworkLabb4::buildAndAddPlayer(QGraphicsItem* graphics, PlayerInfo* playerInfo)
{
	emit addItemToTheScene(graphics);
	Player* player = createPlayer(graphics, playerInfo);
	this->players.push_back(player);
	if (playerInfo->id == id)
	{
		localPlayer = player;
	}
}

QGraphicsItem* NetworkLabb4::createSphere(PlayerInfo* info)
{
	QBrush redBrush(Qt::red);
	QBrush blueBrush(Qt::blue);
	QPen blackPen(Qt::black);

	QGraphicsEllipseItem* ellipse = new QGraphicsEllipseItem(0, 0, 100, 100);
	if (id == info->id)
	{
		ellipse->setBrush(blueBrush);
	}
	else
	{
		ellipse->setBrush(redBrush);
	}
	
	ellipse->setPen(blackPen);
	return ellipse;
}

Player* NetworkLabb4::createPlayer(QGraphicsItem* graphics, PlayerInfo* info)
{
	this->playersGraphics.push_back(graphics);
	this->playersInfo.push_back(info);

	Player* player = new Player();
	player->graphic = graphics;
	player->info = info;
	return player;
}

void NetworkLabb4::addItemToScene(QGraphicsItem* item)
{
	scene->addItem(item);
}

bool NetworkLabb4::playerAlreadyExists(int pid)
{
	for (size_t i = 0; i < players.size(); i++)
	{
		if (players.at(i)->info->id == pid)
		{
			return true;
		}
	}
	return false;
}

//Overload function to listen for key presses
void NetworkLabb4::keyPressEvent(QKeyEvent *event)
{
	if (!ui.lineEdit->isEnabled())
	{
		//Move left
		if (event->key() == Qt::Key_A){
			con->reqMove(localPlayer->graphic->pos().x() - 1, localPlayer->graphic->pos().y(), -1, 0);
		}
		//Move right
		if (event->key() == Qt::Key_D){
			con->reqMove(localPlayer->graphic->pos().x() + 1, localPlayer->graphic->pos().y(), 1, 0);
		}
		//Move up
		if (event->key() == Qt::Key_W){
			con->reqMove(localPlayer->graphic->pos().x(), localPlayer->graphic->pos().y() - 1, 0, -1);
		}
		//Move down
		if (event->key() == Qt::Key_S){
			con->reqMove(localPlayer->graphic->pos().x(), localPlayer->graphic->pos().y() + 1, 0, 1);
		}
		//Calls overloaded close function(Invokes closeEvent())
		if (event->key() == Qt::Key_Escape){
			close();
		}
	}
}

//Overload function when game client closes
void NetworkLabb4::closeEvent(QCloseEvent *event)
{
	//Request to server to leave the game
	if (con != NULL)
	{
		con->reqLeave();
		//Close socket
		socket->Close();
	}
	//Exit program
	exit(1);
}



