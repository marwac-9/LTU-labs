#pragma once
#include "BaseGameEntity.h"
#include "EnumLocations.h"
#include "Timer.h"
#include "Job.h"
#include "Position.h"
#include "Graphics.h"
#include "Location.h"
#include <QGraphicsItem>
#include "Size.h"

class Meeting;
class Job;
struct Item;
struct Telegram;
class QGraphicsDropShadowEffect;
template <class entity_type> class State;
template <class entity_type> class StateMachine;

//the amount of gold a miner must have before he feels comfortable
const int wealthLevel = 500;
//the amount of nuggets a miner can carry
const int maxNuggets = 3;
//above this value a miner is thirsty
const int thirstLevel = 50;
const int hungerLevel = 70;
//above this value a miner is sleepy
const int tirednessLevel = 50;

const int lonelinessLevel = 10;

class Sim : public QObject, public BaseGameEntity, public QGraphicsItem
{
	Q_OBJECT
	Q_INTERFACES(QGraphicsItem)
signals :
	void simMesssageToGui(QString message, Sim* sim);
public:
	Sim(int id);
	Sim(int id, const char* displayName, int money, int loneliness, int hunger, int thirst, int fatigue, int hatred);
	~Sim();

	void Update();
	virtual bool HandleMessage(const Telegram& msg);
	StateMachine<Sim>* GetFSM() const;
	Location* GetLocation();
	Location* GetDestination() const;
	

	
	
	void          ChangeLocation(Location &loc) { this->location = loc; }
	void          ChangeDestination(Location* dest) { this->destination = dest; }

	void		  AssignHomeLocation(Location* homeLoc){ this->home = homeLoc; }
	Location*	  GetHomeLocation();

	Size		  GetSize() const { return graphicProperties.GetSize(); }
	Color		  GetColor() const { return graphicProperties.GetColor(); }
	Position	  GetPosition() const { return this->location.GetPosition(); };

	
	void          SetSize(int w, int h) { this->graphicProperties.SetSize(w, h); }
	void          SetColor(int r, int g, int b) { this->graphicProperties.SetColor(r, g, b); }
	void		  SetPosition(int x, int y) { this->location.SetPosition(x, y); }
	

	void          SetMoney(int money) { this->money = money; }

	bool          Fatigued()const;
	bool		  CanWakeUp()const;
	void          DecreaseFatigue() { this->fatigue -= 1; if (fatigue < 0) { fatigue = 0; } }
	void          IncreaseFatigue() { this->fatigue += 1; if (fatigue > 100) { fatigue = 100; } }

	bool          Lonely()const;
	void          DecreaseLoneliness(){ this->loneliness -= 1; if (loneliness < 0) { loneliness = 0; } }
	void          IncreaseLoneliness(){ this->loneliness += 1; if (loneliness > 100) { loneliness = 100; } }

	int           Wealth()const { return this->money; }
	bool          Wealthy()const;
	void          SetWealth(const int val) { this->money = val; }
	void          AddToWealth(const int val);

	bool          Thirsty()const;
	bool          BuyAndDrinkAWhiskey(){ if (this->money > 1) { this->thirst = 0; this->money -= 2; return true; }	else { return false; }	}
	void          DecreaseThirst(){ this->thirst -= 1; if (thirst < 0) { thirst = 0; } }
	void          IncreaseThirst(){ this->thirst += 1; if (thirst > 100) { thirst = 100; } }
	
	bool		  IsTimeToWork() { return Timer::Instance()->getHours() == job->GetWorkHours(); }
	bool		  IsNowWorkingHours() { return (Timer::Instance()->getHours() >= job->GetWorkingFrom() && !IsTimeToStopWork()); }
	bool		  IsTimeToStopWork() { return Timer::Instance()->getHours() >= job->GetWorkingFrom() + job->GetWorkHours(); }

	bool          Hungry() const;
	bool		  BuyAndEatABurger(){ if (this->money > 1) { this->hunger = 0; this->money -= 2; return true; } else { return false; } }
	void          DecreaseHunger(){ this->hunger -= 1; if (hunger < 0) { hunger = 0; } }
	void          IncreaseHunger(){ this->hunger += 1; if (hunger > 100) { hunger = 100; } }

	int			  Health();
	bool		  Dead() { if (this->health < 1) { return true; } return false; }

	void		  AssignJob(Job* newJob){ this->job = newJob;}
	Job*		  GetCurrentJob() { return this->job; }
	Location*	  GetJobLocation();

	void		  GetPaid();
	bool		  Buy(Item* item);

	bool		  HasAnyMeetingsPlanned() const;
	bool		  IsTimeForMeeting() const;
	void		  AddMeeting(Meeting* meetingTime);
	Meeting*	  GetMeeting();
	void		  RemoveMeeting(Meeting* meeting);
	void		  DeleteMeeting(Meeting* meeting);

	void		  RecordTime(int time);
	int			  GetRecordedTime();

	void		  IncreaseTurnedDown();
	void		  ResetTurnedDown();
	bool		  TurnedDownTooManyTimes();
	int			  LastTimeTurnedDown();

	QRectF boundingRect() const;
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
	bool Pressed = false;
	bool Hover = false;

	int health;
	int loneliness;
	int hunger;
	int thirst;
	int fatigue; //opposite to energy
	int hatred;
	int money;
	void AdvanceToDestination();
	void ChangeLocationName(std::string newLocName) { this->location.name = newLocName; }
	bool IsInAreaOf(Location* dest);
	QGraphicsTextItem* GetName();
	int GetUpdateCounter();
	void ResetUpdateCounter();
	void SendMessageToGUI(std::string message);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
private:
	int	FindMeetingIndex(Meeting* meeting);

	int storedTime;
	int turnedDownCounter = 0;
	int dayLastTurnedDown = -1;
	int lastDayAskedForMeeting = 0;
	Graphics graphicProperties;
	Job* job;
	QGraphicsTextItem * displayName;
	Location* home;
	int attributeUpdateCount = 0;
	//an instance of the state machine class
	StateMachine<Sim>* stateMachine;
	//a pointer to an instance of a State

	QGraphicsDropShadowEffect* glowEffect;
	int speed = 2;
	// the place where the miner is currently situated
	Location location;
	Location* destination = NULL;
	//how many nuggets the miner has in his pockets

	std::vector<Meeting*> meetingsCue;
	

	
	
	
};

