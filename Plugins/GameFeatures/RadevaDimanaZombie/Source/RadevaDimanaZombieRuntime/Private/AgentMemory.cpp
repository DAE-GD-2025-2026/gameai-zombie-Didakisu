#include "AgentMemory.h"

AgentMemory::AgentMemory()
{

}

void AgentMemory::RegisterZombie(AActor* Actor, FVector Location, float CurrentTime)
{
	if (!Actor)
	{
		return;
	}

	//check if the survivor has seen the zombie
	for (int i = 0; i < Zombies.Num(); i++)
	{
		if (Zombies[i].Actor == Actor)
		{
			Zombies[i].Location = Location;
			Zombies[i].LastSeenTime = CurrentTime;
			return;
		}
	}

	FPerceivedTarget NewEntry;
	NewEntry.Actor = Actor;
	NewEntry.Location = Location;
	NewEntry.LastSeenTime = CurrentTime;

	Zombies.Add(NewEntry);
}

void AgentMemory::RegisterItem(AActor * Actor, FVector Location)
{

}

void AgentMemory::RegisterHouse(AActor * Actor, FVector Location)
{

}

void AgentMemory::UnregisterZombie(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	for (int i = Zombies.Num() - 1; i >= 0; i--)
	{
		if (Zombies[i].Actor == Actor)
		{
			Zombies.RemoveAt(i);
			return;
		}
	}
}

const TArray<FPerceivedTarget>& AgentMemory::GetZombies() const
{
	return Zombies;
}

const TArray<FPerceivedTarget>& AgentMemory::GetItems() const
{
	return Items;
}

const TArray<FPerceivedTarget>& AgentMemory::GetHouses() const
{
	return Houses;
}

FVector AgentMemory::GetClosestZombieLocation(const FVector& FromLocation) const
{
	if (Zombies.Num() == 0)
	{
		return FVector::ZeroVector;
	}

	int ClosestZombieIndex = 0;
	float ClosestDistanceSquared = FVector::DistSquared(Zombies[0].Location, FromLocation);

	for (int i = 1; i < Zombies.Num(); i++)
	{
		float CurrentDistanceSquared = FVector::DistSquared(Zombies[i].Location, FromLocation);

		if (CurrentDistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = CurrentDistanceSquared;
			ClosestZombieIndex = i;
		}
	}

	return Zombies[ClosestZombieIndex].Location;
}
