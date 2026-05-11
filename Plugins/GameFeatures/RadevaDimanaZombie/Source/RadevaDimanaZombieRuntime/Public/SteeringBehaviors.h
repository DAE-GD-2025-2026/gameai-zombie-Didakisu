#pragma once
#include "CoreMinimal.h"

class ASurvivorPawn;

class Wander
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime);
private:
	float WanderAngle = 0.f;

	float CircleDistance = 300.f; 
	float CircleRadius = 150.f;  
	float WanderJitter = 2.f;
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