#include "States/CollectState.h"
#include "Survivor/SurvivorPawn.h"
#include "Common/InventoryComponent.h"

CollectState::CollectState(ASurvivorPawn* InPawn, AgentMemory* InMemory)
{
	Pawn = InPawn;
	Memory = InMemory;
}

void CollectState::OnEnter()
{
	bInventoryFull = false;
	TargetItem = Memory->GetClosestItem(Pawn->GetActorLocation());
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Entering Collect State"));
}

void CollectState::OnExit()
{
	TargetItem = nullptr;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, TEXT("Exiting Collect State"));
}

void CollectState::Update(float DeltaTime)
{
	UpdateToTarget();
	MoveToTarget();
	TryPickUp();
}

void CollectState::UpdateToTarget()
{
	if ((!TargetItem || !IsValid(TargetItem)) && Memory->GetItems().Num() > 0)
	{
		TargetItem = Memory->GetClosestItem(Pawn->GetActorLocation());
	}
}

void CollectState::MoveToTarget()
{
	if (!Pawn || !TargetItem || !IsValid(TargetItem))
	{
		return;
	}

	FVector Direction = SeekBehavior.CalculateSteering(Pawn, TargetItem->GetActorLocation());
	Pawn->AddMovementInput(Direction, 0.3f);
}

void CollectState::TryPickUp()
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
		Memory->UnregisterItem(TargetItem);
		TargetItem = nullptr;
		bInventoryFull = true;
	}
}