#include "MessageDispatcher.h"
#include "BaseGameEntity.h"
#include "EntityManager.h"
#include "EnumLocations.h"
#include "MessageTypes.h"
#include "Timer.h"
#include "Telegram.h"
#include <iostream>



MessageDispatcher::MessageDispatcher()
{}

MessageDispatcher* MessageDispatcher::Instance()
{
	static MessageDispatcher instance;

	return &instance;
}

void MessageDispatcher::Discharge(BaseGameEntity* receiver,
	const Telegram& telegram)
{
	if (!receiver->HandleMessage(telegram))
	{
		//telegram could not be handled
		std::cout << "\nMessage not handled by " << EntityManager::Instance()->GetEntityName(receiver->GetId());
	}
}

void MessageDispatcher::DischargeToAll(const Telegram& telegram)
{
	std::map<int, BaseGameEntity*>::iterator it = EntityManager::Instance()->entityMap.begin();
	std::map<int, BaseGameEntity*>::iterator itEnd = EntityManager::Instance()->entityMap.end();
	for (it; it != itEnd; it++)
	{
		if (!it->second->HandleMessage(telegram))
		{
			//telegram could not be handled
			std::cout << "\nMessage not handled by " << it->second->name;
		}
	}
	
}

//---------------------------- DispatchMessageToAll ---------------------------
//
//  given a message, a sender and any time delay , this function
//  routes the message to all agents (if no delay) or stores
//  in the message queue to be dispatched at the correct time
//------------------------------------------------------------------------
void MessageDispatcher::DispatchAMessageToAll(double  delay,
										int     sender,
										int     msg,
										void*   ExtraInfo)
{

	//get pointers to the sender and receiver
	BaseGameEntity* Sender = EntityManager::Instance()->GetEntityFromID(sender);

	//create the telegram
	Telegram telegram(0, sender, msg, ExtraInfo);

	//if there is no delay, route telegram immediately                       
	if (delay <= 0.0f)
	{
		std::cout << "\nInstant telegram dispatched at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes()
			<< " by " << EntityManager::Instance()->GetEntityName(Sender->GetId()) << " to all " << ". Msg is " << MsgToStr(msg);

		//send the telegram to the recipient
		DischargeToAll(telegram);
	}

	//else calculate the time when the telegram should be dispatched
	else
	{
		double currentTime = Timer::Instance()->getTotalMinutes();

		telegram.dispatchTime = currentTime + delay;

		//and put it in the queue
		priorityQ.insert(telegram);

		std::cout << "\nDelayed telegram from " << EntityManager::Instance()->GetEntityName(Sender->GetId()) << " recorded at time "
			<< Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes() << " for all " << ". Msg is " << MsgToStr(msg);
	}
}

void MessageDispatcher::DispatchAMessage(double  delay,
	int     sender,
	int     receiver,
	int     msg,
	void*   ExtraInfo)
{

	//get pointers to the sender and receiver
	BaseGameEntity* Sender = EntityManager::Instance()->GetEntityFromID(sender);
	BaseGameEntity* Receiver = EntityManager::Instance()->GetEntityFromID(receiver);

	//make sure the receiver is valid
	if (Receiver == NULL)
	{
		std::cout << "\nWarning! No Receiver with ID of " << receiver << " found";

		return;
	}

	//create the telegram
	Telegram telegram(0, sender, receiver, msg, ExtraInfo);

	//if there is no delay, route telegram immediately                       
	if (delay <= 0.0f)
	{
		std::cout << "\nInstant telegram dispatched at time: " << Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes()
			<< " by " << EntityManager::Instance()->GetEntityName(Sender->GetId()) << " for " << EntityManager::Instance()->GetEntityName(Receiver->GetId())
			<< ". Msg is " << MsgToStr(msg);

		//send the telegram to the recipient
		Discharge(Receiver, telegram);
	}

	//else calculate the time when the telegram should be dispatched
	else
	{
		double currentTime = Timer::Instance()->getTotalMinutes();

		telegram.dispatchTime = currentTime + delay;

		//and put it in the queue
		priorityQ.insert(telegram);

		std::cout << "\nDelayed telegram from " << EntityManager::Instance()->GetEntityName(Sender->GetId()) << " recorded at time "
			<< Timer::Instance()->getHours() << " " << Timer::Instance()->getMinutes() << " for " << EntityManager::Instance()->GetEntityName(Receiver->GetId())
			<< ". Msg is " << MsgToStr(msg);

	}
}


//---------------------- DispatchDelayedMessages -------------------------
//
//  This function dispatches any telegrams with a timestamp that has
//  expired. Any dispatched telegrams are removed from the queue
//------------------------------------------------------------------------
void MessageDispatcher::DispatchDelayedMessages()
{
	//get current time
	double CurrentTime = Timer::Instance()->getTotalMinutes();

	//now peek at the queue to see if any telegrams need dispatching.
	//remove all telegrams from the front of the queue that have gone
	//past their sell by date
	while (!priorityQ.empty() &&
		(priorityQ.begin()->dispatchTime < CurrentTime) &&
		(priorityQ.begin()->dispatchTime > 0))
	{
		//read the telegram from the front of the queue
		const Telegram& telegram = *priorityQ.begin();

		//find the recipient
		BaseGameEntity* Receiver = EntityManager::Instance()->GetEntityFromID(telegram.receiver);

		std::cout << "\nQueued telegram ready for dispatch: Sent to "
			<< EntityManager::Instance()->GetEntityName(Receiver->GetId()) << ". Msg is " << MsgToStr(telegram.msg);

		//send the telegram to the recipient
		Discharge(Receiver, telegram);

		//remove it from the queue
		priorityQ.erase(priorityQ.begin());
	}
}

void MessageDispatcher::DispatchDelayedMessagesToAll()
{
	//get current time
	double CurrentTime = Timer::Instance()->getTotalMinutes();

	//now peek at the queue to see if any telegrams need dispatching.
	//remove all telegrams from the front of the queue that have gone
	//past their sell by date
	while (!priorityQ.empty() &&
		(priorityQ.begin()->dispatchTime < CurrentTime) &&
		(priorityQ.begin()->dispatchTime > 0))
	{
		//read the telegram from the front of the queue
		const Telegram& telegram = *priorityQ.begin();

		std::cout << "\nQueued telegram ready for dispatch: Sent to all" << ". Msg is " << MsgToStr(telegram.msg);

		//send the telegram to the recipient
		DischargeToAll(telegram);

		//remove it from the queue
		priorityQ.erase(priorityQ.begin());
	}
}


