#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"

class ASurvivorPawn;
class Flee;

class FleeState : public State
{
public:
	FleeState(ASurvivorPawn* InPawn, AgentMemory* InMemory);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;
private:
	ASurvivorPawn* Pawn = nullptr;
	AgentMemory* Memory = nullptr;

	Flee FleeBehavior;
};