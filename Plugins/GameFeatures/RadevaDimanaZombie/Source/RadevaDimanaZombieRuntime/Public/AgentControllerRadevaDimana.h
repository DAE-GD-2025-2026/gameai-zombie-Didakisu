#pragma once

#include "CoreMinimal.h"
#include "AgentMemoryRadevaDimana.h"
#include "AgentFSMRadevaDimana.h"

#include "States/FleeStateRadevaDimana.h"
#include "States/WanderStateRadevaDimana.h"
#include "States/CollectStateRadevaDimana.h"
#include "States/FightBackStateRadevaDimana.h"

class ASurvivorPawn;

class AgentControllerRadevaDimana
{
public:
	AgentControllerRadevaDimana();

	void Initialize(ASurvivorPawn* InPawn);
	void Update(float DeltaTime);

	AgentMemoryRadevaDimana& GetMemory() { return Memory; }
	FSM& GetFSM() { return StateMachine; }

	int FindItemOfType(UInventoryComponent* Inventory, EItemType Type);
	void HandleItemUsage();
	void TryCollectNearbyItem();
private:
	ASurvivorPawn* Pawn = nullptr;

	AgentMemoryRadevaDimana Memory;
	FSM StateMachine;

	FleeStateRadevaDimana* Flee;
	WanderStateRadevaDimana* Wander;
	CollectStateRadevaDimana* Collect;
	FightBackStateRadevaDimana* FightBack;

	float AttackCooldown = 0.f;
	float AttackCooldownDuration = 3.f;

	bool bUnderAttack = false;
	int LastHealth = 10;
};