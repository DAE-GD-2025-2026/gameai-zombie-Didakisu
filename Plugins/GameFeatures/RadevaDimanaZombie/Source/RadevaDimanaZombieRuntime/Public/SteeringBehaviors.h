#pragma once
#include "CoreMinimal.h"

class ASurvivorPawn;

class Wander
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime);
private:
	float WanderAngle{ 0.f };
	float TimeSinceLastUpdate{ 0.f };
	float TimeBetweenDirectionChange{ 2.f };
	FVector SmoothedDirection = FVector::ForwardVector;
};

class Seek
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, FVector TargetLocation);
};

class Flee
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, FVector ThreatLocation);
};