#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"

class ASurvivorPawn;
//class Wander;

class WanderState : public State
{
public:
	WanderState(ASurvivorPawn* InPawn/*, AgentMemory* InMemory*/);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;
private:
	ASurvivorPawn* Pawn = nullptr;
	AgentMemory* Memory = nullptr;

	Wander WanderBehavior;
	Seek SeekBehavior;
};