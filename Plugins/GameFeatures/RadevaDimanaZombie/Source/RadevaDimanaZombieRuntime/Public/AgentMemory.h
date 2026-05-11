#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"
#include "Items/BaseItem.h"

class ABaseZombie;

struct FPerceivedTarget
{
	AActor* Actor;
	FVector Location;
	float LastSeenTime;
};

class AgentMemory
{
public: 
	AgentMemory();
	void Update(float DeltaTime);

	void RegisterZombie(AActor* Actor, FVector Location/*, float CurrentTime*/);
	void RegisterItem(AActor* Actor, FVector Location);
	//void RegisterHouse(AActor* Actor, FVector Location);

	void UnregisterZombie(AActor* Actor);
	void UnregisterItem(ABaseItem* Item);

	const TArray<FPerceivedTarget>& GetZombies() const;
	const TArray<FPerceivedTarget>& GetItems() const;
	const TArray<FPerceivedTarget>& GetHouses() const;

	FVector GetClosestZombieLocation(const FVector& FromLocation) const;
	//FVector GetClosestHouseLocation(const FVector& FromLocation) const;
	ABaseItem* GetClosestItem(const FVector& FromLocation) const;
	void SetForgetTime(float NewForgetTime) {}
private:
	TArray<FPerceivedTarget> Zombies;
	TArray<FPerceivedTarget> Items;
	TArray<FPerceivedTarget> Houses;

	float ElapsedTime = 0.f;
	float ZombieForgetTime = 3.f;
	float ItemForgetTime = 10.f;
};