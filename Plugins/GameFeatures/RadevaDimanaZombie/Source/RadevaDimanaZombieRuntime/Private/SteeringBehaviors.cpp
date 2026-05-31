#include "SteeringBehaviors.h"
#include "Survivor/SurvivorPawn.h"

FVector Seek::CalculateSteering(ASurvivorPawn* Pawn, FVector TargetLocation)
{
	FVector Direction = TargetLocation - Pawn->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();
	return Direction;
}