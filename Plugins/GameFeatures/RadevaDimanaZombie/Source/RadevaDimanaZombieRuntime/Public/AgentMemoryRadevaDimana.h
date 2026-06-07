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

class AgentMemoryRadevaDimana
{
public: 
	AgentMemoryRadevaDimana();
	void Update(float DeltaTime);

	void RegisterZombie(AActor* Actor, FVector Location);
	void RegisterItem(AActor* Actor, FVector Location, FVector AgentLocation);
	void RegisterHouse(AActor* Actor, FVector Location);
	void RegisterPurgeZone(AActor* Actor, FVector Location);

	void UnregisterZombie(AActor* Actor);
	void UnregisterItem(ABaseItem* Item);

	const TArray<FPerceivedTarget>& GetZombies() const;
	const TArray<FPerceivedTarget>& GetItems() const;
	const TArray<FPerceivedTarget>& GetHouses() const;
	const TArray<FPerceivedTarget>& GetPurgeZones() const;

	FVector GetClosestZombieLocation(const FVector& FromLocation) const;
	AActor* GetClosestHouse(const FVector& FromLocation, bool bUnvisitedOnly = false) const;
	ABaseItem* GetClosestItem(const FVector& FromLocation) const;
	ABaseItem* GetClosestItemOfType(const FVector& FromLocation, EItemType Type) const;

	void MarkHouseVisited(AActor* House);
	bool IsHouseVisited(AActor* House) const;

	int GetVisitedHousesCount() const { return VisitedHouses.Num(); }
private:
	TArray<FPerceivedTarget> Zombies;
	TArray<FPerceivedTarget> Items;
	TArray<FPerceivedTarget> Houses;
	TArray<FPerceivedTarget> PurgeZones;

	float ElapsedTime = 0.f;
	float ZombieForgetTime = 3.f;
	float ItemForgetTime = 10.f;

	TArray<AActor*> VisitedHouses;
};