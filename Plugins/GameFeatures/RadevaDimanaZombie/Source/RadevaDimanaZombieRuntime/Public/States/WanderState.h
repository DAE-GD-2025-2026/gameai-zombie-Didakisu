#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"

class ASurvivorPawn;

class WanderState : public State
{
public:
	WanderState(ASurvivorPawn* InPawn, AgentMemory* InMemory);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;

	bool IsInsideHouse(AActor* House) const;
private:
	void PickNewNavMeshTarget();
	void BuildPathTo(FVector Target);

	ASurvivorPawn* Pawn = nullptr;
	AgentMemory* Memory = nullptr;

	FVector NavTarget = FVector::ZeroVector;
	bool bHasTarget = false;

	float AcceptanceRadius = 150.f;
	float SearchRadius = 4000.f;

	bool bSeekingHouse = false;

	TArray<FVector> CurrentPath;
	int32 CurrentPathIndex = 0;
	float WaypointAcceptanceRadius = 50.f; //keep small so the pawn to be close enough before changing
};