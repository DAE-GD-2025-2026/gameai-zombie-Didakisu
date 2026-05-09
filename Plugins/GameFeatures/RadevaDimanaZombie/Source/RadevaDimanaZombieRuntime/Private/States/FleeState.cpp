#include "States/FleeState.h"
#include "Survivor/SurvivorPawn.h"
#include "SteeringBehaviors.h"

FleeState::FleeState(ASurvivorPawn* InPawn, AgentMemory* InMemory)
{
	Pawn = InPawn;
	Memory = InMemory;
}

void FleeState::OnEnter()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Entering Flee State"));
	}
}

void FleeState::OnExit()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting Flee State"));
	}
}

void FleeState::Update(float DeltaTime)
{
	if (!Pawn || !Memory)
	{
		return;
	}

	FVector ThreatLocation = Memory->GetClosestZombieLocation(Pawn->GetActorLocation());

	FVector Direction = FleeBehavior.CalculateSteering(Pawn, ThreatLocation);
	Direction.Z = 0.f;
	Direction = Direction.GetSafeNormal();

	Pawn->AddMovementInput(Direction, 1.f);
}
