#pragma once
#include "../AgentFSMRadevaDimana.h"
#include "../AgentMemoryRadevaDimana.h"
#include "SteeringBehaviorsRadevaDimana.h"
#include "Items/BaseItem.h"

class ASurvivorPawn;

class FightBackStateRadevaDimana : public State
{
public:
	FightBackStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory);
	void OnEnter() override;
	void OnExit() override;
	void Update(float DeltaTime) override;

	bool HasWeapon() const;
private:
	void FaceTarget(float DeltaTime);
	void Shoot();
	void Spin(float DeltaTime);
	int FindItemOfType(UInventoryComponent* Inventory, EItemType Type);

	ASurvivorPawn* Pawn = nullptr;
	AgentMemoryRadevaDimana* Memory = nullptr;

	float SpinSpeed = 180.f;

	float FireCooldown = 0.f;
	float FireRate = 1.f;
};