#include "AgentMemory.h"

AgentMemory::AgentMemory()
{

}

void AgentMemory::Update(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	Zombies.RemoveAll([&](const FPerceivedTarget& Entry)
	{
		return (ElapsedTime - Entry.LastSeenTime) > ZombieForgetTime;
	});

	Items.RemoveAll([&](const FPerceivedTarget& Entry)
	{
		return !IsValid(Entry.Actor) || (ElapsedTime - Entry.LastSeenTime) > ItemForgetTime;
	});
}

void AgentMemory::RegisterZombie(AActor* Actor, FVector Location/*, float CurrentTime*/)
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
			Zombies[i].LastSeenTime = ElapsedTime;
			return;
		}
	}

	FPerceivedTarget NewEntry;
	NewEntry.Actor = Actor;
	NewEntry.Location = Location;
	NewEntry.LastSeenTime = ElapsedTime;

	Zombies.Add(NewEntry);
}

void AgentMemory::RegisterItem(AActor * Actor, FVector Location)
{
	if (!Actor || Actor->IsHidden())
	{
		return;
	}

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Actor == Actor)
		{
			Items[i].Location = Location;
			Items[i].LastSeenTime = ElapsedTime;
			return;
		}
	}

	FPerceivedTarget NewEntry;
	NewEntry.Actor = Actor;
	NewEntry.Location = Location;
	NewEntry.LastSeenTime = ElapsedTime;

	Items.Add(NewEntry);
}

//void AgentMemory::RegisterHouse(AActor * Actor, FVector Location)
//{
//	if (!Actor)
//	{
//		return;
//	}
//
//	for (int i = 0; i < Houses.Num(); i++)
//	{
//		if (Houses[i].Actor == Actor)
//		{
//			Houses[i].Location = Location;
//			return;
//		}
//	}
//
//	FPerceivedTarget NewEntry;
//	NewEntry.Actor = Actor;
//	NewEntry.Location = Location;
//	NewEntry.LastSeenTime = 0.f;
//
//	Houses.Add(NewEntry);
//}

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

void AgentMemory::UnregisterItem(ABaseItem* Item)
{
	if (!Item)
	{
		return;
	}

	for (int i = Items.Num() - 1; i >= 0; i--)
	{
		if (Items[i].Actor == Item)
		{
			Items.RemoveAt(i);
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

//FVector AgentMemory::GetClosestHouseLocation(const FVector& FromLocation) const
//{
//	if (Houses.Num() == 0)
//	{
//		return FVector::ZeroVector;
//	}
//
//	int ClosestHouseIndex = 0;
//	float ClosestDistanceSquared = FVector::DistSquared(Houses[0].Location, FromLocation);
//
//	for (int i = 1; i < Houses.Num(); i++)
//	{
//		float CurrentDistanceSquared = FVector::DistSquared(Houses[i].Location, FromLocation);
//
//		if (CurrentDistanceSquared < ClosestDistanceSquared)
//		{
//			ClosestDistanceSquared = CurrentDistanceSquared;
//			ClosestHouseIndex = i;
//		}
//	}
//
//	return Houses[ClosestHouseIndex].Location;
//}

ABaseItem* AgentMemory::GetClosestItem(const FVector& FromLocation) const
{
	if (Items.Num() == 0)
	{
		return nullptr;
	}

	int ClosestItemIndex = 0;
	float ClosestDistanceSquared = FVector::DistSquared(Items[0].Location, FromLocation);

	for (int i = 1; i < Items.Num(); i++)
	{
		float CurrentDistanceSquared = FVector::DistSquared(Items[i].Location, FromLocation);

		if (CurrentDistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = CurrentDistanceSquared;
			ClosestItemIndex = i;
		}
	}

	ABaseItem* Item = Cast<ABaseItem>(Items[ClosestItemIndex].Actor);
	if (!IsValid(Item))
	{
		return nullptr;
	}

	return Item;
}