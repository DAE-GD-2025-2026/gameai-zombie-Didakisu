#include "States/FleeState.h"
#include "Survivor/SurvivorPawn.h"
#include "SteeringBehaviors.h"
#include "Village/House/House.h"
#include "Common/InventoryComponent.h"
#include "States/WanderState.h"

FleeState::FleeState(ASurvivorPawn* InPawn, AgentMemory* InMemory, WanderState* InWander, CollectState* InCollect)
{
    Pawn = InPawn;
    Memory = InMemory;
    Wander = InWander;
    Collect = InCollect;
}

void FleeState::OnEnter()
{
    bHasTarget = false;
    CurrentPath.Empty();
    CurrentPathIndex = 0;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Entering Flee State"));
}

void FleeState::OnExit()
{
    bHasTarget = false;
    CurrentPath.Empty();
    CurrentPathIndex = 0;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting Flee State"));
}

void FleeState::BuildPathTo(FVector Target)
{
    NavTarget = Target;
    bHasTarget = true;
    CurrentPath.Empty();
    CurrentPathIndex = 0;

    UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(Pawn->GetWorld(), Pawn->GetActorLocation(), NavTarget);

    if (Path && Path->IsValid())
    {
        CurrentPath = Path->PathPoints;
    }
}

void FleeState::PickFleeTarget()
{
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
    if (!NavSystem)
    {
        return;
    }

    const TArray<FPerceivedTarget>& PurgeZones = Memory->GetPurgeZones();
    //purge zone
    if (PurgeZones.Num() > 0)
    {
        GEngine->AddOnScreenDebugMessage(51, 2.f, FColor::Purple, TEXT("Purge zone block entered"));

        int ClosestIndex = 0;
        float ClosestDist = FVector::Dist2D(Pawn->GetActorLocation(), PurgeZones[0].Location);

        for (int i = 1; i < PurgeZones.Num(); i++)
        {
            float Dist = FVector::Dist2D(Pawn->GetActorLocation(), PurgeZones[i].Location);
            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                ClosestIndex = i;
            }
        }

        FVector ZoneCenter = PurgeZones[ClosestIndex].Location;
        float Radius = 50.f;

        FVector ThroughDirection = (ZoneCenter - Pawn->GetActorLocation()).GetSafeNormal();

        FVector ExitPoint = ZoneCenter + ThroughDirection * (Radius + 100.f);

        FNavLocation Result;
        if (NavSystem->GetRandomReachablePointInRadius(ExitPoint, 1000.f, Result))
        {
            BuildPathTo(Result.Location);
            return;
        }
    }

    AActor* UnvisitedHouse = Memory->GetClosestHouse(Pawn->GetActorLocation(), true);
    if (UnvisitedHouse && IsInsideHouse(UnvisitedHouse))
    {
        if (!Collect->IsInventoryFull())
        {
            Memory->MarkHouseVisited(UnvisitedHouse);
        }
    }
    else if (UnvisitedHouse && !IsInsideHouse(UnvisitedHouse))
    {
        BuildPathTo(UnvisitedHouse->GetActorLocation());
        return;
    }

    FVector SpiralPoint = Wander->GetCurrentSpiralPoint();
    FNavLocation Result;
    if (NavSystem->GetRandomReachablePointInRadius(SpiralPoint, 500.f, Result))
    {
        BuildPathTo(Result.Location);
    }
}

bool FleeState::IsInsideHouse(AActor* House) const
{
    if (!House)
    {
        return false;
    }

    AHouse* HouseActor = Cast<AHouse>(House);
    if (!HouseActor)
    {
        return false;
    }

    FHouseBounds Bounds = HouseActor->GetBounds();
    FVector PawnLoc = Pawn->GetActorLocation();

    return PawnLoc.X >= Bounds.Origin.X - Bounds.Extent.X && PawnLoc.X <= Bounds.Origin.X + Bounds.Extent.X &&
           PawnLoc.Y >= Bounds.Origin.Y - Bounds.Extent.Y && PawnLoc.Y <= Bounds.Origin.Y + Bounds.Extent.Y;
}

void FleeState::FollowPath()
{
    if (CurrentPathIndex < CurrentPath.Num() - 1)
    {
        if (FVector::Dist2D(Pawn->GetActorLocation(), CurrentPath[CurrentPathIndex]) < WaypointAcceptanceRadius)
        {
            CurrentPathIndex++;
        }
    }

    if (CurrentPath.Num() > 0 && CurrentPathIndex < CurrentPath.Num())
    {
        FVector Direction = CurrentPath[CurrentPathIndex] - Pawn->GetActorLocation();
        Direction.Z = 0.f;
        Direction.Normalize();
        Pawn->AddMovementInput(Direction, 1.f);
    }
    else
    {
        FVector Direction = NavTarget - Pawn->GetActorLocation();
        Direction.Z = 0.f;
        Direction.Normalize();
        Pawn->AddMovementInput(Direction, 1.f);
    }
}

void FleeState::Update(float DeltaTime)
{
    if (!Pawn || !Memory)
    {
        return;
    }

    int CurrentPurgeCount = Memory->GetPurgeZones().Num();

    if (!bHasTarget || FVector::Dist2D(Pawn->GetActorLocation(), NavTarget) < AcceptanceRadius || CurrentPurgeCount > LastPurgeZoneCount)
    {
        LastPurgeZoneCount = CurrentPurgeCount;
        PickFleeTarget();
    }

    if (!bHasTarget)
    {
        return;
    }

    FollowPath();
}