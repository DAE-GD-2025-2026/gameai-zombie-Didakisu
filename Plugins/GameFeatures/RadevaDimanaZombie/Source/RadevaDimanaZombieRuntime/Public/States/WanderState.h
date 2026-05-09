#pragma once
#include "../AgentFSM.h"
#include "SteeringBehaviors.h"

class ASurvivorPawn;
//class Wander;

class WanderState : public State
{
public:
	WanderState(ASurvivorPawn* InPawn);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;
private:
	ASurvivorPawn* Pawn = nullptr;
	Wander WanderBehavior;
};