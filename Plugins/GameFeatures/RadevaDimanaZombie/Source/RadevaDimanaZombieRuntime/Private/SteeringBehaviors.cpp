#include "SteeringBehaviors.h"
#include "Survivor/SurvivorPawn.h"

FVector Wander::CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime)
{
	if (!Pawn)
	{
		return FVector::ZeroVector;
	}

	TimeSinceLastUpdate += DeltaTime;

	if (TimeSinceLastUpdate >= TimeBetweenDirectionChange)
	{
		TimeSinceLastUpdate = 0.f;
		WanderAngle += FMath::FRandRange(-1.5f, 1.5f);
	}

	const float CircleRadius = 300.f;
	const float ForwardDistance = 300.f;

	FVector Forward = Pawn->GetActorForwardVector();
	FVector Right = Pawn->GetActorRightVector();
	FVector CircleCenter = Pawn->GetActorLocation() + Forward * ForwardDistance;

	FVector Offset = (Forward * FMath::Cos(WanderAngle) + Right * FMath::Sin(WanderAngle)) * CircleRadius;
	Offset.Z = 0.f;

	FVector WanderTarget = CircleCenter + Offset;

	DrawDebugSphere(Pawn->GetWorld(), WanderTarget, 20.f, 8, FColor::Green, false, -1.f);

	FVector DesiredDirection = WanderTarget - Pawn->GetActorLocation();
	DesiredDirection.Z = 0.f;
	DesiredDirection.Normalize();

	SmoothedDirection = FMath::VInterpTo(SmoothedDirection, DesiredDirection, DeltaTime, 3.f);
	return SmoothedDirection;
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

