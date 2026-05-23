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

private:
	ASurvivorPawn* Pawn = nullptr;

	AgentMemory Memory;
	FSM StateMachine;

	FleeState* Flee;
	WanderState* Wander;
	CollectState* Collect;
	FightBackState* FightBack;

	int LastHealth = 10;
};