#include "States/WanderState.h"
#include "Survivor/SurvivorPawn.h"

WanderState::WanderState(ASurvivorPawn* InPawn)
{
	Pawn = InPawn;
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

	FVector Direction = WanderBehavior.CalculateSteering(Pawn, DeltaTime);
	Pawn->AddMovementInput(Direction, 0.3f);
}