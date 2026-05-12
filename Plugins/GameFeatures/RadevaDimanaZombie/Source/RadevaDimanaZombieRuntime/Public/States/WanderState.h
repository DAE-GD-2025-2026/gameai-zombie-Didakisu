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
	void PickNewNavMeshTarget();

	ASurvivorPawn* Pawn = nullptr;
	FVector NavTarget = FVector::ZeroVector;
	bool bHasTarget = false;

	float AcceptanceRadius = 150.f;
	float SearchRadius = 2000.f;
};