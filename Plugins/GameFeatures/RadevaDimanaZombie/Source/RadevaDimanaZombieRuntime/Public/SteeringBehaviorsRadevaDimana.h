#pragma once
#include "CoreMinimal.h"

class ASurvivorPawn;

class Seek
{
public:
	FVector CalculateSteering(ASurvivorPawn* Pawn, FVector TargetLocation);
};
