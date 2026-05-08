#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"

class ASurvivorPawn;
class Flee;

class FleeState : public State
{
public:
	FleeState(ASurvivorPawn* InPawn, AgentMemory* InMemory);
	void Update(float DeltaTime) override;

private:
	ASurvivorPawn* Pawn = nullptr;
	AgentMemory* Memory = nullptr;

	Flee* FleeBehavior;
};