#include "SteeringBehaviors.h"
#include "Survivor/SurvivorPawn.h"

FVector Wander::CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime)
{
	return FVector();
}

FVector Seek::CalculateSteering(ASurvivorPawn* Pawn, FVector TargetLocation)
{
	FVector Direction = TargetLocation - Pawn->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();
	return Direction;
}

FVector Flee::CalculateSteering(ASurvivorPawn* Pawn, FVector ThreatLocation)
{
	FVector Direction = Pawn->GetActorLocation() - ThreatLocation;
	Direction.Z = 0.f;
	Direction.Normalize();
	return Direction;
}

