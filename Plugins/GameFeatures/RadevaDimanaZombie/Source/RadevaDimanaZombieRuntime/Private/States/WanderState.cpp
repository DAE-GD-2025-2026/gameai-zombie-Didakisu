#include "States/WanderState.h"
#include "Survivor/SurvivorPawn.h"

WanderState::WanderState(ASurvivorPawn* InPawn/*, AgentMemory* InMemory*/)
{
	Pawn = InPawn;
	//Memory = InMemory;
}

void WanderState::OnEnter()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Entering Wander State"));
	}
}

void WanderState::OnExit()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting Wander State"));
	}
}

void WanderState::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}

	//FVector Direction;

	//if (Memory->GetHouses().Num() > 0)
	//{
	//	FVector HouseLocation = Memory->GetClosestHouseLocation(Pawn->GetActorLocation());
	//	Direction = SeekBehavior.CalculateSteering(Pawn, HouseLocation);
	//}
	//else
	//{
	//	Direction = WanderBehavior.CalculateSteering(Pawn, DeltaTime);
	//}

	FVector Direction = WanderBehavior.CalculateSteering(Pawn, DeltaTime);
	Pawn->AddMovementInput(Direction, 0.3f);
}