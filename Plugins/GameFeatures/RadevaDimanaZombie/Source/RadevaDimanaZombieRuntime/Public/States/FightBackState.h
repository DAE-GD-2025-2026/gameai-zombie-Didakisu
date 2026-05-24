#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"
#include "Items/BaseItem.h"

class ASurvivorPawn;

class FightBackState : public State
{
public:
	FightBackState(ASurvivorPawn* InPawn, AgentMemory* InMemory);
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
	AgentMemory* Memory = nullptr;

	float SpinSpeed = 180.f;

	float FireCooldown = 0.f;
	float FireRate = 1.f;
};