#include "Sim.h"
#include "State.h"
#include <assert.h>   
#include "Meeting.h"   
#include "StateIdle.h"
#include "StateGlobal.h"
#include "StateMachine.h"
#include "Location.h"
#include <QtGui>
#include <QGraphicsEffect>
#include "ai_lab1_qt.h"
#include "Item.h"
Sim::Sim(int id) : BaseGameEntity(id),
				   money(0),
				   thirst(0),
				   fatigue(0),
				   hunger(0)
{
	//set up the state machine
	stateMachine = new StateMachine<Sim>(this);
	stateMachine->SetCurrentState(StateIdle::Instance());
	stateMachine->SetGlobalState(StateGlobal::Instance());
	Pressed = false;
}

Sim::Sim(int id, const char* displayName, int money, int loneliness, int hunger, int thirst, int fatigue, int hatred) : BaseGameEntity(id),
	money(money),
	loneliness(loneliness),
	hunger(hunger),
	thirst(thirst),
	fatigue(fatigue),
	hatred(hatred)
{
	Health();
	this->glowEffect = new QGraphicsDropShadowEffect();
	this->setGraphicsEffect(glowEffect);
	glowEffect->setOffset(0, 0);
	glowEffect->setBlurRadius(8);

	this->displayName = new QGraphicsTextItem;
	this->displayName->setPlainText(displayName);

	setFlag(ItemIsSelectable);
	setAcceptHoverEvents(true);

	this->name = displayName;
	//set up the state machine
	stateMachine = new StateMachine<Sim>(this);
	stateMachine->SetCurrentState(StateIdle::Instance());
	stateMachine->SetGlobalState(StateGlobal::Instance());
}

Sim::~Sim()
{
	delete stateMachine;
}

bool Sim::HandleMessage(const Telegram& msg)
{
	return stateMachine->HandleMessage(msg);
}

void Sim::Update()
{
	attributeUpdateCount++;
	stateMachine->Update();
	Health();
}

StateMachine<Sim>* Sim::GetFSM()const
{
	return stateMachine;
}

Location* Sim::GetLocation()
{ 
	return &this->location; 
}

Location* Sim::GetDestination() const
{
	return this->destination;
}

void Sim::AddToWealth(const int val)
{
	money += val;

	if (money < 0) money = 0;
}

bool Sim::Thirsty()const
{
	if (thirst >= thirstLevel){ return true; }

	return false;
}

bool Sim::Hungry()const
{
	if (hunger >= hungerLevel){ return true; }

	return false;
}


bool Sim::Lonely() const
{
	if (loneliness >= lonelinessLevel){ return true; }

	return false;
}

bool Sim::Fatigued() const
{
	if (fatigue > tirednessLevel)
	{
		return true;
	}

	return false;
}

QRectF Sim::boundingRect() const
{
	QRectF rectangle(0, 0, GetSize().w, GetSize().h);
	return rectangle;
}

void Sim::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	QRectF rec = boundingRect();
	QBrush brush(Qt::blue);
	QPen blackPen(Qt::black);
	blackPen.setWidth(2);
	blackPen.setStyle(Qt::DashLine);
	blackPen.setCapStyle(Qt::RoundCap);
	blackPen.setJoinStyle(Qt::RoundJoin);
	
	if (Hover)
	{
		blackPen.setColor(QColor(0, 0, 0, 255));
	}
	else
	{
		//if selected then set color else do the below no color
		if (isSelected())
		{
			blackPen.setColor(QColor(255, 0, 0, 255));
		}
		else
		{
			blackPen.setColor(QColor(0, 0, 0, 0));
		}
	}
	if (Pressed)
	{
		brush.setColor(Qt::red);
		glowEffect->setColor(QColor(255, 0, 0, 255));
	}
	else
	{
		Color simColor = graphicProperties.GetColor();
		brush.setColor(QColor(simColor.r, simColor.g, simColor.b));
		glowEffect->setColor(QColor(0, 0, 0, 0));
	}

	setPos(location.GetPosition().x, location.GetPosition().y);
	displayName->setPos(location.GetPosition().x - 5, location.GetPosition().y + GetSize().h);
	painter->fillRect(rec, brush);
	painter->setPen(blackPen);
	painter->drawRect(rec);
}

void Sim::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	std::cout << "button pressed";
	Pressed = true;
	update();
	QGraphicsItem::mousePressEvent(event);
}

void Sim::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	std::cout << "button released";
	Pressed = false;
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}

void Sim::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	std::cout << "mouse in";
	Hover = true;
	update();
	QGraphicsItem::hoverEnterEvent(event);
}

void Sim::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	std::cout << "mouse out";
	Hover = false;
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}

int Sim::Health()
{
	
	int balancing = (loneliness*0.1f + hunger*0.3f + thirst*1 + fatigue*0.5f);
	if (balancing > 100) // i might do the test somewhere for health lower than 0 instead
	{
		balancing = 100;
	}
	/*std::cout << "\n" << loneliness;
	std::cout << "\n" << hunger;
	std::cout << "\n" << thirst;
	std::cout << "\n" << fatigue;*/
	return health = 100 - balancing;
}

void Sim::AdvanceToDestination()
{
	int simCenterX = location.GetPosition().x + (GetSize().w / 2);
	int simCenterY = location.GetPosition().y + (GetSize().h / 2);

	int dx = destination->GetPosition().x + (destination->GetSize().w / 2) - (location.GetPosition().x + (GetSize().w / 2));
	int dy = destination->GetPosition().y + (destination->GetSize().h / 2) - (location.GetPosition().y + (GetSize().h / 2));

	int destNX = destination->GetPosition().x + GetSize().w; // new destination position x 
	int destNY = destination->GetPosition().y + GetSize().h; // new destination position y
	int destX2 = destination->GetPosition().x + destination->GetSize().w - GetSize().w; // new destination x2
	int destY2 = destination->GetPosition().y + destination->GetSize().h - GetSize().h; // new destination y2

	if (simCenterX > destX2 || simCenterX < destNX)
	{
		if (dx < 0)
		{
			location.SetX(location.GetPosition().x - 4);
		}
		else
		{
			location.SetX(location.GetPosition().x + 4);
		}
	}
	if (simCenterY > destY2 || simCenterY < destNY)
	{
		if (dy < 0)
		{
			location.SetY(location.GetPosition().y - 4);
		}
		else
		{
			location.SetY(location.GetPosition().y + 4);
		}
	}
	update();
}

bool Sim::IsInAreaOf(Location* dest)
{
	int dx = dest->GetPosition().x - location.GetPosition().x;
	int dy = dest->GetPosition().x - location.GetPosition().x;

	int locationCenterX = location.GetPosition().x + (GetSize().w / 2); // new location position x
	int locationCenterY = location.GetPosition().y + (GetSize().h / 2); // new location position y

	int destinationCenterX = dest->GetPosition().x + (dest->GetSize().w / 2);
	int destinationCenterY = dest->GetPosition().y + (dest->GetSize().h / 2);

	int destNX = dest->GetPosition().x + (GetSize().w / 2) - 2; // new destination position x // -2 and + 2 to increase search boundary a bit
	int destNY = dest->GetPosition().y + (GetSize().h / 2) - 2; // new destination position y
	int destX2 = dest->GetPosition().x + dest->GetSize().w - (GetSize().w / 2) + 2; // new destination x2
	int destY2 = dest->GetPosition().y + dest->GetSize().h - (GetSize().h / 2) + 2; // new destination y2

	//check if location enter is somewhere in the destination new rectangle
	if (locationCenterX >= destNX && locationCenterX <= destX2 && locationCenterY >= destNY && locationCenterY <= destY2)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Location* Sim::GetJobLocation()
{
	return job->GetLocation();
}

Location* Sim::GetHomeLocation()
{
	return this->home;
}

QGraphicsTextItem* Sim::GetName()
{
	return displayName;
}

void Sim::GetPaid()
{
	AddToWealth(job->GetPayment());
}

bool Sim::CanWakeUp() const
{
	return fatigue < 2;
}

bool Sim::Wealthy() const
{
	if (money > wealthLevel)
	{
		return true;
	}
	return false;
}

bool Sim::Buy(Item* item)
{
	if (money > item->value)
	{
		money = money - item->value;
		return true;
	}
	return false;
}

bool Sim::HasAnyMeetingsPlanned() const
{
	return meetingsCue.size() > 0;
}

void Sim::AddMeeting(Meeting* meeting)
{
	meetingsCue.push_back(meeting);
}

Meeting* Sim::GetMeeting()
{
	if (meetingsCue.size() < 1)
	{
		return NULL;
	}
	return meetingsCue.at(0);
}

void Sim::RecordTime(int time)
{
	storedTime = time;
}

int Sim::GetRecordedTime()
{
	return storedTime;
}

int Sim::GetUpdateCounter()
{
	return attributeUpdateCount;
}

void Sim::ResetUpdateCounter()
{
	attributeUpdateCount = 0;
}

bool Sim::IsTimeForMeeting() const
{
	if (HasAnyMeetingsPlanned())
	{
		return Timer::Instance()->getTotalMinutes() == meetingsCue.at(0)->GetTime().tm;
	}
	return false;
}

void Sim::RemoveMeeting(Meeting* meeting)
{
	int index = FindMeetingIndex(meeting);
	if (index != -1)
	{
		meetingsCue.erase(meetingsCue.begin() + index);
	}
}

void Sim::DeleteMeeting(Meeting* meeting)
{
	int index = FindMeetingIndex(meeting);
	if (index != -1)
	{
		delete meeting;
	}
}

int Sim::FindMeetingIndex(Meeting* meeting)
{
	for (size_t i = 0; i < meetingsCue.size(); i++)
	{
		if (meetingsCue.at(i) == meeting)
		{
			return i;
		}
	}
	return -1;
}

void Sim::IncreaseTurnedDown()
{
	turnedDownCounter++;
	dayLastTurnedDown = Timer::Instance()->getDays();
}

void Sim::ResetTurnedDown()
{
	turnedDownCounter = 0;
}

bool Sim::TurnedDownTooManyTimes()
{
	if (turnedDownCounter >= 3)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int Sim::LastTimeTurnedDown()
{
	return dayLastTurnedDown;
}

void Sim::SendMessageToGUI(std::string message)
{
	QString mes = QString::fromStdString(message);
	emit simMesssageToGui(mes, this);
}


