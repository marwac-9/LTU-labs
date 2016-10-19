#pragma once
#include <cassert>
#include <string>

#include "State.h"
#include "Telegram.h"


template <class entity_type>
class StateMachine
{
public:
	StateMachine(entity_type* owner) :owner(owner),
		currentState(NULL),
		previousState(NULL),
		globalState(NULL)
	{}
	virtual ~StateMachine(){}
	//use these methods to initialize the FSM
	void SetCurrentState(State<entity_type>* s){ currentState = s; }
	void SetGlobalState(State<entity_type>* s) { globalState = s; }
	void SetPreviousState(State<entity_type>* s){ previousState = s; }
	//call this to update the FSM
	void  Update()const
	{
		//if a global state exists, call its execute method, else do nothing
		if (globalState)   globalState->Execute(owner);

		//same for the current state
		if (currentState) currentState->Execute(owner);
	}

	bool  HandleMessage(const Telegram& msg)const
	{
		//first see if the current state is valid and that it can handle
		//the message
		if (currentState && currentState->OnMessage(owner, msg))
		{
			return true;
		}

		//if not, and if a global state has been implemented, send 
		//the message to the global state
		if (globalState && globalState->OnMessage(owner, msg))
		{
			return true;
		}

		return false;
	}
	//change to a new state
	void  ChangeState(State<entity_type>* pNewState)
	{
		assert(pNewState && "<StateMachine::ChangeState>:trying to assign null state to current");
		//std::cout << GetNameOfCurrentState();
		//keep a record of the previous state
		previousState = currentState;

		//call the exit method of the existing state
		currentState->Exit(owner);

		//change state to the new state
		currentState = pNewState;

		//call the entry method of the new state
		currentState->Enter(owner);
	}
	//change state back to the previous state
	void RevertToPreviousState()
	{
		ChangeState(previousState);
	}

	//accessors
	State<entity_type>*  CurrentState()  const{ return currentState; }
	State<entity_type>*  GlobalState()   const{ return globalState; }
	State<entity_type>*  PreviousState() const{ return previousState; }
	//returns true if the current state’s type is equal to the type of the
	//class passed as a parameter.
	bool isInState(const State<entity_type>& st) const
	{
		if (typeid(*currentState) == typeid(st)) return true;
		return false;
	}

	//only ever used during debugging to grab the name of the current state
	std::string GetNameOfCurrentState() const
	{
		std::string s(typeid(*currentState).name());

		//remove the 'class ' part from the front of the string
		if (s.size() > 5)
		{
			s.erase(0, 6);
		}

		return s;
	}

private:
	//a pointer to the agent that owns this instance
	entity_type* owner;

	State<entity_type>* currentState;
	//a record of the last state the agent was in
	State<entity_type>* previousState;
	//this state logic is called every time the FSM is updated
	State<entity_type>* globalState;
};
