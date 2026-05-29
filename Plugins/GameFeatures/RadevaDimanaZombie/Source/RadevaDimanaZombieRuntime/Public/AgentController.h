#pragma once

#include "CoreMinimal.h"
#include "AgentMemory.h"
#include "AgentFSM.h"

#include "States/FleeState.h"
#include "States/WanderState.h"
#include "States/CollectState.h"
#include "States/FightBackState.h"

class ASurvivorPawn;

class AgentController
{
public:
	AgentController();

	void Initialize(ASurvivorPawn* InPawn);
	void Update(float DeltaTime);

	AgentMemory& GetMemory() { return Memory; }
	FSM& GetFSM() { return StateMachine; }

	int FindItemOfType(UInventoryComponent* Inventory, EItemType Type);
	void HandleItemUsage();
	void TryCollectNearbyItem();
private:
	ASurvivorPawn* Pawn = nullptr;

	AgentMemory Memory;
	FSM StateMachine;

	FleeState* Flee;
	WanderState* Wander;
	CollectState* Collect;
	FightBackState* FightBack;

	float AttackCooldown = 0.f;
	float AttackCooldownDuration = 3.f;

	bool bUnderAttack = false;
	int LastHealth = 10;
};