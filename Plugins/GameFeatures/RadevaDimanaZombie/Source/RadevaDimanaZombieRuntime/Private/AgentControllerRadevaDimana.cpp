#include "AgentControllerRadevaDimana.h"
#include "Survivor/SurvivorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpectatorPawn.h"
#include "Items/BaseItem.h"
#include "Common/InventoryComponent.h"

AgentControllerRadevaDimana::AgentControllerRadevaDimana()
{

}

void AgentControllerRadevaDimana::Initialize(ASurvivorPawn* InPawn)
{
	Pawn = InPawn;

	UFloatingPawnMovement* Movement = Pawn->GetComponentByClass<UFloatingPawnMovement>();
	if (Movement)
	{
		Movement->MaxSpeed = 750.f;
	}

	auto CollectStatePtr = std::make_unique<CollectStateRadevaDimana>(Pawn, &Memory);
	auto WanderStatePtr = std::make_unique<WanderStateRadevaDimana>(Pawn, &Memory , CollectStatePtr.get());
	auto FleeStatePtr = std::make_unique<FleeStateRadevaDimana>(Pawn, &Memory, WanderStatePtr.get(), CollectStatePtr.get());
	auto FightBackStatePtr = std::make_unique<FightBackStateRadevaDimana>(Pawn, &Memory);

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
		UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
		if (Inventory)
		{
			int OccupiedSlots = 0;

			for (int i = 0; i < Inventory->GetInventory().Num(); i++)
			{
				if (Inventory->GetInventory()[i] != nullptr)
				{
					OccupiedSlots++;
				}
			}
			if (OccupiedSlots < Inventory->GetInventoryCapacity())
			{
				Collect->ResetInventoryFull();
			}
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

void AgentControllerRadevaDimana::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}

	UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
	if (Inventory)
	{
		int OccupiedSlots = 0;

		for (int i = 0; i < Inventory->GetInventory().Num(); i++)
		{
			if (Inventory->GetInventory()[i] != nullptr)
			{
				OccupiedSlots++;
			}
		}
		if (OccupiedSlots < Inventory->GetInventoryCapacity())
		{
			Collect->ResetInventoryFull();
		}
	}

	Memory.Update(DeltaTime);
	HandleItemUsage();
	StateMachine.Update(DeltaTime);

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

int AgentControllerRadevaDimana::FindItemOfType(UInventoryComponent* Inventory, EItemType Type)
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

void AgentControllerRadevaDimana::HandleItemUsage()
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

	if (Health->GetHealth() < 5)
	{
		int Slot = FindItemOfType(Inventory, EItemType::Medkit);
		
		if (Slot != -1)
		{
			bool bUsed = Inventory->UseItem(Slot);
			if (bUsed)
			{
				Inventory->RemoveItem(Slot);
				Collect->ResetInventoryFull();
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
				Collect->ResetInventoryFull();
				GEngine->AddOnScreenDebugMessage(20, 2.f, FColor::Orange, TEXT("Ate FOOD"));
			}
		}
	}
}