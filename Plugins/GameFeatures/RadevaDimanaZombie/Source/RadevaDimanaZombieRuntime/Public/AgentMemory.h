#pragma once
#include "CoreMinimal.h"
#include "Perception/AIPerceptionTypes.h"

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
	void RegisterZombie(AActor* Actor, FVector Location, float CurrentTime);
	void RegisterItem(AActor* Actor, FVector Location);
	void RegisterHouse(AActor* Actor, FVector Location);

	const TArray<FPerceivedTarget>& GetZombies() const;
	const TArray<FPerceivedTarget>& GetItems() const;
	const TArray<FPerceivedTarget>& GetHouses() const;

	FVector GetClosestZombieLocation(const FVector& FromLocation) const;
private:
	TArray<FPerceivedTarget> Zombies;
	TArray<FPerceivedTarget> Items;
	TArray<FPerceivedTarget> Houses;
};