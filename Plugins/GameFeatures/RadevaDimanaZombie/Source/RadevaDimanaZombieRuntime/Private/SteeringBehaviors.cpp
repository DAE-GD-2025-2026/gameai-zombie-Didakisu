#include "SteeringBehaviors.h"
#include "Survivor/SurvivorPawn.h"

FVector Wander::CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime)
{
    if (!Pawn)
    {
    	return FVector::ZeroVector;
    }

    WanderAngle += FMath::FRandRange(-WanderJitter, WanderJitter) * DeltaTime;

    FVector CircleCenter = Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * CircleDistance;
    FVector WanderPoint = CircleCenter + FVector(FMath::Cos(WanderAngle), FMath::Sin(WanderAngle), 0.f) * CircleRadius;

    FVector Direction = WanderPoint - Pawn->GetActorLocation();
    Direction.Z = 0.f;
    Direction.Normalize();

    return Direction;
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

