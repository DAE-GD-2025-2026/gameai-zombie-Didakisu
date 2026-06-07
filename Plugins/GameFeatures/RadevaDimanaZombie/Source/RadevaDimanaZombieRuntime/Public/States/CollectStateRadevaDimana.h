#pragma once
#include "../AgentFSMRadevaDimana.h"
#include "../AgentMemoryRadevaDimana.h"
#include "SteeringBehaviorsRadevaDimana.h"
#include "Items/BaseItem.h"

class ASurvivorPawn;
//class Seek;

class CollectStateRadevaDimana : public State
{
public:
	CollectStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;

	bool IsInventoryFull() const { return bInventoryFull; }
	void ResetInventoryFull() { bInventoryFull = false; }
private:
	void UpdateToTarget();
	void MoveToTarget();
	void TryPickUp();

	ASurvivorPawn* Pawn = nullptr;
	AgentMemoryRadevaDimana* Memory = nullptr;
	ABaseItem* TargetItem = nullptr;

	Seek SeekBehavior;
	bool bInventoryFull = false;
};