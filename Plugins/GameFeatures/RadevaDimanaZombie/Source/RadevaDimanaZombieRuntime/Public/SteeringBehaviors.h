#pragma once
#include "CoreMinimal.h"

class ASurvivorPawn;

class Wander
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, float DeltaTime);
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