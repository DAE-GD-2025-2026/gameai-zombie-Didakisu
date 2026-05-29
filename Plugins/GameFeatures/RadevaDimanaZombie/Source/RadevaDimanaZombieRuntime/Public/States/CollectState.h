#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"
#include "Items/BaseItem.h"

class ASurvivorPawn;
//class Seek;

class CollectState : public State
{
public:
	CollectState(ASurvivorPawn* InPawn, AgentMemory* InMemory);
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
	AgentMemory* Memory = nullptr;
	ABaseItem* TargetItem = nullptr;

	Seek SeekBehavior;
	bool bInventoryFull = false;
};