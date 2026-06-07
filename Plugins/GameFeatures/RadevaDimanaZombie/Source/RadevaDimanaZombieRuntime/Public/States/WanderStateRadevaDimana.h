#pragma once
#include "../AgentFSMRadevaDimana.h"
#include "../AgentMemoryRadevaDimana.h"
#include "SteeringBehaviorsRadevaDimana.h"
#include "States/CollectStateRadevaDimana.h"

class ASurvivorPawn;

class WanderStateRadevaDimana : public State
{
public:
	WanderStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory, CollectStateRadevaDimana* InCollect);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;

	bool IsInsideHouse(AActor* House) const;
	FVector GetCurrentSpiralPoint() const;

	void AdvanceSpiral();
private:
	void PickNewNavMeshTarget();
	void BuildPathTo(FVector Target);

	ASurvivorPawn* Pawn = nullptr;
	AgentMemoryRadevaDimana* Memory = nullptr;

	FVector NavTarget = FVector::ZeroVector;
	bool bHasTarget = false;

	float AcceptanceRadius = 100.f; //150.f
	float SearchRadius = 4000.f;

	bool bSeekingHouse = false;

	TArray<FVector> CurrentPath;
	int32 CurrentPathIndex = 0;
	float WaypointAcceptanceRadius = 50.f; //keep small so the pawn to be close enough before changing

	float ExplorationRadius = 500.f;
	float ExplorationRadiusStep = 500.f;
	float MaxExplorationRadius = 4000.f;
	float ExplorationAngle = 0.f;
	FVector SpawnLocation;
	bool bSpawnLocationSet = false;

	CollectStateRadevaDimana* Collect = nullptr;
	bool bJustEnteredHouse = false;
};