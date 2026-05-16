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

	auto FleeStatePtr = std::make_unique<FleeState>(Pawn, &Memory);
	auto WanderStatePtr = std::make_unique<WanderState>(Pawn, &Memory);
	auto CollectStatePtr = std::make_unique<CollectState>(Pawn, &Memory);

	Flee = FleeStatePtr.get();
	Wander = WanderStatePtr.get();
	Collect = CollectStatePtr.get();

	StateMachine.AddState(std::move(FleeStatePtr));
	StateMachine.AddState(std::move(WanderStatePtr));
	StateMachine.AddState(std::move(CollectStatePtr));

	//transitions
	StateMachine.AddTransition(Wander, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0;
	});

	StateMachine.AddTransition(Flee, Wander, [this]()
	{
		return Memory.GetZombies().Num() == 0;
	});

	StateMachine.AddTransition(Wander, Collect, [this]()
	{
		return Memory.GetItems().Num() > 0 && !Collect->IsInventoryFull();
	});

	StateMachine.AddTransition(Collect, Wander, [this]()
	{
		return Memory.GetItems().Num() == 0 || Collect->IsInventoryFull();
	});

	StateMachine.AddTransition(Collect, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0;
	});

	StateMachine.AddTransition(Flee, Collect, [this]()
	{
		return Memory.GetZombies().Num() == 0 && Memory.GetItems().Num() > 0;
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
	StateMachine.Update(DeltaTime);
	HandleItemUsage();

	FVector Dir = Pawn->GetVelocity();
	Dir.Z = 0.f;

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
		bUnderAttack = true;
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

	if (Stamina->GetCurrentStamina() < 3.f)
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

	if (bUnderAttack)
	{
		int Slot = FindItemOfType(Inventory, EItemType::Pistol);

		if (Slot != -1)
		{
			Inventory->UseItem(Slot);
			GEngine->AddOnScreenDebugMessage(22, 2.f, FColor::Yellow, TEXT("Shooting back attacker!"));
		}

		bUnderAttack = false;
	}
}