#include "AgentController.h"
#include "Survivor/SurvivorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpectatorPawn.h"
#include "Items/BaseItem.h"
#include "Common/InventoryComponent.h"

AgentController::AgentController()
{

}

void AgentController::Initialize(ASurvivorPawn* InPawn)
{
	Pawn = InPawn;

	UFloatingPawnMovement* Movement = Pawn->GetComponentByClass<UFloatingPawnMovement>();
	if (Movement)
	{
		Movement->MaxSpeed = 750.f;
	}

	auto WanderStatePtr = std::make_unique<WanderState>(Pawn, &Memory);
	auto FleeStatePtr = std::make_unique<FleeState>(Pawn, &Memory, WanderStatePtr.get());
	auto CollectStatePtr = std::make_unique<CollectState>(Pawn, &Memory);
	auto FightBackStatePtr = std::make_unique<FightBackState>(Pawn, &Memory);

	Flee = FleeStatePtr.get();
	Wander = WanderStatePtr.get();
	Collect = CollectStatePtr.get();
	FightBack = FightBackStatePtr.get();

	StateMachine.AddState(std::move(FleeStatePtr));
	StateMachine.AddState(std::move(WanderStatePtr));
	StateMachine.AddState(std::move(CollectStatePtr));
	StateMachine.AddState(std::move(FightBackStatePtr));

	//transitions

	StateMachine.AddTransition(Wander, Flee, [this]()
	{
		return (Memory.GetZombies().Num() > 0 || bUnderAttack) && !FightBack->HasWeapon();
	});

	StateMachine.AddTransition(Wander, FightBack, [this]()
	{
		return (Memory.GetZombies().Num() > 0 || bUnderAttack) && FightBack->HasWeapon();
	});

	StateMachine.AddTransition(Wander, Collect, [this]()
	{
		/*return Memory.GetItems().Num() > 0 && !Collect->IsInventoryFull();*/
		UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
		if (Inventory && Inventory->GetInventory().Num() < Inventory->GetInventoryCapacity())
		{
			Collect->ResetInventoryFull();
		}
		return Memory.GetItems().Num() > 0 && !Collect->IsInventoryFull();
	});

	StateMachine.AddTransition(Collect, Flee, [this]()
	{
		return (Memory.GetZombies().Num() > 0 || bUnderAttack) && !FightBack->HasWeapon();
	});

	StateMachine.AddTransition(Collect, FightBack, [this]()
	{
		return (Memory.GetZombies().Num() > 0 || bUnderAttack) && FightBack->HasWeapon();
	});

	StateMachine.AddTransition(FightBack, Wander, [this]()
	{
		return Memory.GetZombies().Num() == 0 && !bUnderAttack;
	});

	StateMachine.AddTransition(FightBack, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0 && !FightBack->HasWeapon() && bUnderAttack;
	});

	StateMachine.AddTransition(Flee, Wander, [this]()
	{
		return Memory.GetZombies().Num() == 0 && !bUnderAttack;
	});

	StateMachine.AddTransition(Collect, Wander, [this]()
	{
		return Memory.GetItems().Num() == 0 || Collect->IsInventoryFull();
	});

	StateMachine.AddTransition(Flee, Collect, [this]()
	{
		return Memory.GetZombies().Num() == 0 && Memory.GetItems().Num() > 0 && !bUnderAttack;
	});

	StateMachine.AddTransition(Flee, FightBack, [this]()
	{
		return FightBack->HasWeapon();
	});

	StateMachine.SetInitialState(Wander);
}

void AgentController::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}

	Memory.Update(DeltaTime);
	HandleItemUsage();
	StateMachine.Update(DeltaTime);

	TryCollectNearbyItem();

	bUnderAttack = false;

	FVector Dir = Pawn->GetVelocity();
	Dir.Z = 0.f;

	if (AttackCooldown > 0.f)
	{
		AttackCooldown -= DeltaTime;
		bUnderAttack = true;
	}
	else
	{
		bUnderAttack = false;
	}

	if (!Dir.IsNearlyZero())
	{
		Dir.Normalize();

		FRotator TargetRotation = Dir.Rotation();
		FRotator NewRotation = FMath::RInterpTo(Pawn->GetActorRotation(), TargetRotation, DeltaTime, 10.f);
		Pawn->SetActorRotation(NewRotation);
	}

	GEngine->AddOnScreenDebugMessage(10, 0.f, FColor::White,
		FString::Printf(TEXT("Items: %d | Zombies: %d"),
			Memory.GetItems().Num(),
			Memory.GetZombies().Num()));

	GEngine->AddOnScreenDebugMessage(11, 0.f, FColor::Yellow,
		FString::Printf(TEXT("Houses: %d | Visited: %d"),
			Memory.GetHouses().Num(),
			Memory.GetVisitedHousesCount()));
}

int AgentController::FindItemOfType(UInventoryComponent* Inventory, EItemType Type)
{
	const TArray<ABaseItem*>& Items = Inventory->GetInventory();

	for (int i = 0; i < Items.Num(); i++)
	{
		if (Items[i] && Items[i]->GetItemType() == Type)
		{
			return i;
		}
	}

	return -1;
}

void AgentController::HandleItemUsage()
{
	UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
	UHealthComponent* Health = Pawn->GetComponentByClass<UHealthComponent>();
	UStaminaComponent* Stamina = Pawn->GetComponentByClass<UStaminaComponent>();

	if (!Inventory || !Health || !Stamina)
	{
		return;
	}

	//detect damage by comparing health each frame
	int CurrentHealth = Health->GetHealth();

	if (CurrentHealth < LastHealth)
	{
		AttackCooldown = AttackCooldownDuration;
		GEngine->AddOnScreenDebugMessage(23, 2.f, FColor::Red, TEXT("Taking damage!"));
	}
	LastHealth = CurrentHealth;

	if (Health->GetHealth() < 8)
	{
		int Slot = FindItemOfType(Inventory, EItemType::Medkit);
		
		if (Slot != -1)
		{
			bool bUsed = Inventory->UseItem(Slot);
			if (bUsed)
			{
				Inventory->RemoveItem(Slot);
				GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Magenta, TEXT("Used Medkit!"));
			}
		}
	}

	if (Stamina->GetCurrentStamina() < 5.f)
	{
		int Slot = FindItemOfType(Inventory, EItemType::Food);

		if (Slot != -1)
		{
			bool bUsed = Inventory->UseItem(Slot);
			if (bUsed)
			{
				Inventory->RemoveItem(Slot);
				GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Orange, TEXT("Ate FOOD"));
			}
		}
	}
}

void AgentController::TryCollectNearbyItem()
{
	const TArray<FPerceivedTarget>& KnownItems = Memory.GetItems();

	for (const FPerceivedTarget& Entry : KnownItems)
	{
		ABaseItem* Item = Cast<ABaseItem>(Entry.Actor);
		if (!Item || !IsValid(Item))
		{
			continue;
		}

		float Dist = FVector::Dist2D(Pawn->GetActorLocation(), Item->GetActorLocation());
		if (Dist > 100.f)
		{
			continue;
		}

		UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
		if (!Inventory)
		{
			continue;
		}

		for (int i = 0; i < Inventory->GetInventoryCapacity(); i++)
		{
			if (Inventory->GrabItem(i, Item))
			{
				Memory.UnregisterItem(Item);
				break;
			}
		}
		break;
	}
}