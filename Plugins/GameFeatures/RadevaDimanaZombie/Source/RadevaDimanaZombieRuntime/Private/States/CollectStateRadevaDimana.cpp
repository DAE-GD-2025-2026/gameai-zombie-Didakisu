#include "States/CollectStateRadevaDimana.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"

CollectStateRadevaDimana::CollectStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory)
{
	Pawn = InPawn;
	Memory = InMemory;
}

void CollectStateRadevaDimana::OnEnter()
{
	bInventoryFull = false;
	UpdateToTarget();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Entering Collect State"));
}

void CollectStateRadevaDimana::OnExit()
{
	TargetItem = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, TEXT("Exiting Collect State"));
}

void CollectStateRadevaDimana::Update(float DeltaTime)
{
	UpdateToTarget();
	MoveToTarget();
	TryPickUp();
}

void CollectStateRadevaDimana::UpdateToTarget()
{
	UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
	bool bHasWeapon = false;
	if (Inventory)
	{
		for (int i = 0; i < Inventory->GetInventory().Num(); i++)
		{
			ABaseItem* Item = Inventory->GetInventory()[i];
			if (Item && (Item->GetItemType() == EItemType::Shotgun || Item->GetItemType() == EItemType::Pistol))
			{
				bHasWeapon = true;
				break;
			}
		}
	}

	UHealthComponent* Health = Pawn->GetComponentByClass<UHealthComponent>();

	bool bLowHealth = false;
	if (Health)
	{
		float HealthPercent = Health->GetHealth() / (float)Health->GetMaxHealth();
		bLowHealth = HealthPercent < 0.5f;
	}

	if (TargetItem && IsValid(TargetItem))
	{
		return;
	}

	if (Memory->GetItems().Num() == 0)
	{
		return;
	}

	FVector CurrentPosition = Pawn->GetActorLocation();

	if (!bHasWeapon)
	{
		TargetItem = Memory->GetClosestItemOfType(CurrentPosition, EItemType::Shotgun);
		if (!TargetItem)
		{
			TargetItem = Memory->GetClosestItemOfType(CurrentPosition, EItemType::Pistol);
		}
	}

	if (!TargetItem && bLowHealth)
	{
		TargetItem = Memory->GetClosestItemOfType(CurrentPosition, EItemType::Medkit);
	}

	if (!TargetItem)
	{
		TargetItem = Memory->GetClosestItem(CurrentPosition);
	}
}

void CollectStateRadevaDimana::MoveToTarget()
{
	if (!Pawn || !TargetItem || !IsValid(TargetItem))
	{
		return;
	}

	FVector Direction = SeekBehavior.CalculateSteering(Pawn, TargetItem->GetActorLocation());
	Pawn->AddMovementInput(Direction, 0.5f);
}

void CollectStateRadevaDimana::TryPickUp()
{
	if (!Pawn || !TargetItem || !IsValid(TargetItem))
	{
		return;
	}

	float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetItem->GetActorLocation());
	if (Distance >= 100.f)
	{
		return;
	}

	UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	bool bPickedUp = false;

	for (int i = 0; i < Inventory->GetInventoryCapacity(); i++)
	{
		if (Inventory->GrabItem(i, TargetItem))
		{
			Memory->UnregisterItem(TargetItem);
			TargetItem = nullptr;
			bPickedUp = true;
			break;
		}
	}

	if (!bPickedUp)
	{
		bool bAllSlotsFull = true;
		for (int i = 0; i < Inventory->GetInventoryCapacity(); i++)
		{
			if (!Inventory->GetInventory()[i]) 
			{
				bAllSlotsFull = false;
				break;
			}
		}

		Memory->UnregisterItem(TargetItem);
		TargetItem = nullptr;

		if (bAllSlotsFull)
		{
			bInventoryFull = true;
		}
	}
}