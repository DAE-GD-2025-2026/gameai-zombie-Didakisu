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
