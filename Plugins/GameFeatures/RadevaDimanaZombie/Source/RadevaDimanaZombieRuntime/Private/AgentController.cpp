#include "AgentController.h"
#include "Survivor/SurvivorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpectatorPawn.h"

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
	auto WanderStatePtr = std::make_unique<WanderState>(Pawn/*, &Memory*/);
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
		return Memory.GetItems().Num() > 0;
	});

	StateMachine.AddTransition(Collect, Wander, [this]()
	{
		return Memory.GetItems().Num() == 0;
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