#include "States/WanderState.h"
#include "Survivor/SurvivorPawn.h"
#include "NavigationSystem.h"

WanderState::WanderState(ASurvivorPawn* InPawn/*, AgentMemory* InMemory*/)
{
	Pawn = InPawn;
	//Memory = InMemory;
}

void WanderState::OnEnter()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Entering Wander State"));
		PickNewNavMeshTarget();
	}
}

void WanderState::OnExit()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting Wander State"));
		bHasTarget = false;
	}
}

void WanderState::PickNewNavMeshTarget()
{
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());

	if (!NavigationSystem)
	{
		return;
	}

	FNavLocation Result;
	if (NavigationSystem->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), SearchRadius, Result))
	{
		NavTarget = Result.Location;
		bHasTarget = true;
	}
}

void WanderState::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}

	//FVector Direction;

	//if (Memory->GetHouses().Num() > 0)
	//{
	//	FVector HouseLocation = Memory->GetClosestHouseLocation(Pawn->GetActorLocation());
	//	Direction = SeekBehavior.CalculateSteering(Pawn, HouseLocation);
	//}
	//else
	//{
	//	Direction = WanderBehavior.CalculateSteering(Pawn, DeltaTime);
	//}

	if (!bHasTarget || FVector::Dist2D(Pawn->GetActorLocation(), NavTarget) < AcceptanceRadius)
	{
		PickNewNavMeshTarget();
	}

	if (!bHasTarget)
	{
		return;
	}

	FVector Direction = NavTarget - Pawn->GetActorLocation();
	Direction.Z = 0.f;
	Direction.Normalize();

	Pawn->AddMovementInput(Direction, 0.3f);
}
