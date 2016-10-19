#include "ai_lab1_qt.h"
#include "QObject"
#include "QTime"
#include "QDebug"
#include "EntityManager.h"
#include "Timer.h"
#include "Storage.h"
#include <QGraphicsItem>
#include "Sim.h"
#include "Location.h"
#include "Shop.h"

Ai_Lab1_qt::Ai_Lab1_qt(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(updateEntities()));
	QObject::connect(ui.gameSpeed, SIGNAL(valueChanged(int)), this, SLOT(setTimerInterval()));
	QObject::connect(ui.pauseButton, SIGNAL(pressed()), this, SLOT(toggleTimePause()));
	QObject::connect(ui.decreaseLoneliness, SIGNAL(pressed()), this, SLOT(decreaseCurrentSimLoneliness()));
	QObject::connect(ui.increaseLoneliness, SIGNAL(pressed()), this, SLOT(increaseCurrentSimLoneliness()));
	QObject::connect(ui.decreaseHunger, SIGNAL(pressed()), this, SLOT(decreaseCurrentSimHunger()));
	QObject::connect(ui.increaseHunger, SIGNAL(pressed()), this, SLOT(increaseCurrentSimHunger()));
	QObject::connect(ui.decreaseThirst, SIGNAL(pressed()), this, SLOT(decreaseCurrentSimThirst()));
	QObject::connect(ui.increaseThirst, SIGNAL(pressed()), this, SLOT(increaseCurrentSimThirst()));
	QObject::connect(ui.decreaseFatigue, SIGNAL(pressed()), this, SLOT(decreaseCurrentSimFatigue()));
	QObject::connect(ui.increaseFatigue, SIGNAL(pressed()), this, SLOT(increaseCurrentSimFatigue()));
	QObject::connect(ui.moneyDisplay, SIGNAL(valueChanged(int)), this, SLOT(setCurrentSimMoney(int)));
	ui.decreaseLoneliness->setDisabled(true);
	ui.increaseLoneliness->setDisabled(true);
	ui.decreaseHunger->setDisabled(true);
	ui.increaseHunger->setDisabled(true);
	ui.decreaseThirst->setDisabled(true);
	ui.increaseThirst->setDisabled(true);
	ui.decreaseFatigue->setDisabled(true);
	ui.increaseFatigue->setDisabled(true);
	ui.moneyDisplay->blockSignals(true);
	timer.start(1000);
	ui.gameSpeed->setValue(timer.interval());
	Storage::Instance()->LoadLocations("data/locations.txt");
	Storage::Instance()->LoadShops("data/shops");
	Storage::Instance()->LoadJobs("data/jobs.txt");
	Storage::Instance()->LoadSims("data/sims.txt"); // auto registered in EntityManager


	scene = new QGraphicsScene(this);
	previewScene = new QGraphicsScene(this);
	ui.gameView->setScene(scene);
	ui.simPreview->setScene(previewScene);
	previewSim = previewScene->addRect(0, 0, 10, 10, QPen(Qt::black), QBrush(Qt::red));
	previewSim->hide();
	addLocationsToTheScene();
	addSimsToTheScene();

	QObject::connect(scene, SIGNAL(selectionChanged()), this, SLOT(setSimInFocus()));
	selectedSim = NULL;
}

Ai_Lab1_qt::~Ai_Lab1_qt()
{

}

void Ai_Lab1_qt::updateEntities()
{
	qDebug() << "\nupdate" << timer.interval();
	if (EntityManager::Instance()->EntityCount() > 0 && !Paused)
	{
		EntityManager::Instance()->UpdateAll();
		Timer::Instance()->tick();
		updateWorld();
	}
}

void Ai_Lab1_qt::setTimerInterval()
{
	ui.gameSpeed->blockSignals(true);
	int changedTimer = ui.gameSpeed->value();
	timer.setInterval(changedTimer);
	ui.gameSpeed->setValue(timer.interval());
	ui.gameSpeed->blockSignals(false);
}

void Ai_Lab1_qt::setSimInFocus()
{
	QList<QGraphicsItem*> selectedItems = scene->selectedItems();
	if (selectedItems.size() > 0)
	{
		selectedSim = (Sim*)selectedItems.at(0);
		Size simSize = selectedSim->GetSize();
		Color simColor = selectedSim->GetColor();
		//update static gui elements here
		QString simName = selectedSim->GetName()->toPlainText();
		QString simJobName = QString::fromStdString(selectedSim->GetCurrentJob()->GetName());
		ui.simName->setText(simName);
		ui.jobName->setText(simJobName);
		QGraphicsRectItem* simRect = (QGraphicsRectItem*)previewSim;
		simRect->setRect(0, 0, simSize.w, simSize.h);
		simRect->setBrush(QBrush(QColor(simColor.r, simColor.g, simColor.b)));
		previewSim->show();
		updateGuiSimStats();
		if (Paused)
		{
			enableEditing();
		}
	}
	else
	{
		disableEditing();
		selectedSim = NULL;
		ui.healthBar->setValue(0);
		ui.lonelinessBar->setValue(0);
		ui.hungerBar->setValue(0);
		ui.thirstBar->setValue(0);
		ui.fatigueBar->setValue(0);
		ui.hatredBar->setValue(0);
		ui.moneyDisplay->setValue(0);
		ui.currentLocation->setText("Current Location");
		ui.workLocation->setText("Work Location");
		ui.simName->setText("Sim Name");
		ui.jobName->setText("Job Name");
		previewSim->hide();
	}
}

void Ai_Lab1_qt::updateWorld()
{
	int days = Timer::Instance()->getDays();
	int hours = Timer::Instance()->getHours();
	int minutes = Timer::Instance()->getMinutes();
	ui.gameTime->setTime(QTime(hours, minutes));
	ui.daysDisplay->setValue(days);
	if (selectedSim != NULL)
	{
		updateGuiSimStats();
	}
}

void Ai_Lab1_qt::updateGuiSimStats()
{
	//update dynamic gui elements here
	ui.healthBar->setValue(selectedSim->health);
	ui.lonelinessBar->setValue(selectedSim->loneliness);
	ui.hungerBar->setValue(selectedSim->hunger);
	ui.thirstBar->setValue(selectedSim->thirst);
	ui.fatigueBar->setValue(selectedSim->fatigue);
	ui.hatredBar->setValue(selectedSim->hatred);
	ui.moneyDisplay->setValue(selectedSim->money);
	QString simLocation = QString::fromStdString(selectedSim->GetLocation()->name);
	QString jobLocation = QString::fromStdString(selectedSim->GetJobLocation()->name);
	ui.currentLocation->setText(simLocation);
	ui.workLocation->setText(jobLocation);
}

void Ai_Lab1_qt::addSimsToTheScene()
{
	int simsCount = Storage::Instance()->simsList.size();
	for (int i = 0; i < simsCount; i++)
	{
		Sim* sim = Storage::Instance()->simsList.at(i);
		addSim(sim);
		QObject::connect(sim, SIGNAL(simMesssageToGui(QString, Sim*)), this, SLOT(addSimMessage(QString, Sim*)));
	}
}

void Ai_Lab1_qt::addSim(Sim* sim)
{
	scene->addItem(sim);
	scene->addItem(sim->GetName());
	EntityManager::Instance()->RegisterEntity(sim);
}

void Ai_Lab1_qt::addLocationsToTheScene()
{
	int locationsCount = Storage::Instance()->locationsList.size();
	for (int i = 0; i < locationsCount; i++)
	{
		addLocation(Storage::Instance()->locationsList.at(i));
	}
}

void Ai_Lab1_qt::addLocation(Location* loc)
{
	Color locColor = loc->GetColor();
	Position locPos = loc->GetPosition();
	Size locSize = loc->GetSize();
	QBrush brush(QColor(locColor.r, locColor.g, locColor.b));
	QPen pen(Qt::black);
	scene->addRect(locPos.x, locPos.y, locSize.w, locSize.h, pen, brush);

	QGraphicsTextItem* displayName = new QGraphicsTextItem;
	displayName->setPlainText(QString::fromStdString(loc->name));
	displayName->setPos(locPos.x, locPos.y + locSize.h);
	scene->addItem(displayName);
}

QGraphicsPixmapItem* Ai_Lab1_qt::loadImage(const char * path)
{
	QImage image(":/images/myFile.png");
	QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
	return item;
}

void Ai_Lab1_qt::toggleTimePause()
{
	if (Paused)
	{
		Paused = false;
		if (selectedSim != NULL)
		{
			disableEditing();
		}
	}
	else
	{
		Paused = true;
		if (selectedSim != NULL)
		{
			enableEditing();
		}
	}
}

void Ai_Lab1_qt::decreaseCurrentSimLoneliness()
{
	selectedSim->DecreaseLoneliness();	
	updateGuiSimStats();
}

void Ai_Lab1_qt::increaseCurrentSimLoneliness()
{
	selectedSim->IncreaseLoneliness();	
	updateGuiSimStats();
}

void Ai_Lab1_qt::decreaseCurrentSimHunger()
{
	selectedSim->DecreaseHunger();
	updateGuiSimStats();
}

void Ai_Lab1_qt::increaseCurrentSimHunger()
{
	selectedSim->IncreaseHunger();
	updateGuiSimStats();
}

void Ai_Lab1_qt::decreaseCurrentSimThirst()
{
	selectedSim->DecreaseThirst();
	updateGuiSimStats();
}

void Ai_Lab1_qt::increaseCurrentSimThirst()
{
	selectedSim->IncreaseThirst();
	updateGuiSimStats();
}

void Ai_Lab1_qt::decreaseCurrentSimFatigue()
{
	selectedSim->DecreaseFatigue();
	updateGuiSimStats();
}

void Ai_Lab1_qt::increaseCurrentSimFatigue()
{
	selectedSim->IncreaseFatigue();
	updateGuiSimStats();
}

void Ai_Lab1_qt::disableEditing()
{
	ui.moneyDisplay->setReadOnly(true);
	ui.decreaseLoneliness->setDisabled(true);
	ui.increaseLoneliness->setDisabled(true);
	ui.decreaseHunger->setDisabled(true);
	ui.increaseHunger->setDisabled(true);
	ui.decreaseThirst->setDisabled(true);
	ui.increaseThirst->setDisabled(true);
	ui.decreaseFatigue->setDisabled(true);
	ui.increaseFatigue->setDisabled(true);
	ui.moneyDisplay->blockSignals(true);
}

void Ai_Lab1_qt::enableEditing()
{
	ui.moneyDisplay->setReadOnly(false);
	ui.decreaseLoneliness->setDisabled(false);
	ui.increaseLoneliness->setDisabled(false);
	ui.decreaseHunger->setDisabled(false);
	ui.increaseHunger->setDisabled(false);
	ui.decreaseThirst->setDisabled(false);
	ui.increaseThirst->setDisabled(false);
	ui.decreaseFatigue->setDisabled(false);
	ui.increaseFatigue->setDisabled(false);
	ui.moneyDisplay->blockSignals(false);
}

void Ai_Lab1_qt::setCurrentSimMoney(int newMoneyValue)
{
	selectedSim->SetMoney(newMoneyValue);
}

void Ai_Lab1_qt::addSimMessage(QString message, Sim* sim)
{
	QTextCharFormat textformat;
	textformat = ui.simMessages->currentCharFormat();
	Color simColor = sim->GetColor();

	textformat.setForeground(QColor(simColor.r, simColor.g, simColor.b));
	ui.simMessages->setCurrentCharFormat(textformat);
	ui.simMessages->appendPlainText(message);
	//ui.simMessages->append(message);
}
