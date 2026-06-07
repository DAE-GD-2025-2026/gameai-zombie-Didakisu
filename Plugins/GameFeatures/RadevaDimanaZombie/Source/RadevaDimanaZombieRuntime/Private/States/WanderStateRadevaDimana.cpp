#include "States/WanderStateRadevaDimana.h"
#include "Survivor/SurvivorPawn.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Village/House/House.h"
#include "Common/InventoryComponent.h"

WanderStateRadevaDimana::WanderStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory, CollectStateRadevaDimana* InCollect)
{
	Pawn = InPawn;
	Memory = InMemory;
    Collect = InCollect;
}

void WanderStateRadevaDimana::OnEnter()
{
    if (Pawn)
    {
        if (!bSpawnLocationSet)
        {
            SpawnLocation = Pawn->GetActorLocation();
            bSpawnLocationSet = true;
        }
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Entering Wander State"));
        PickNewNavMeshTarget();
    }
}

void WanderStateRadevaDimana::OnExit()
{
	if (Pawn)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting Wander State"));
		bHasTarget = false;
		bSeekingHouse = false;
		CurrentPath.Empty();
		CurrentPathIndex = 0;
	}
}

bool WanderStateRadevaDimana::IsInsideHouse(AActor* House) const
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

FVector WanderStateRadevaDimana::GetCurrentSpiralPoint() const
{
    return SpawnLocation + FVector(FMath::Cos(FMath::DegreesToRadians(ExplorationAngle)) * ExplorationRadius, FMath::Sin(FMath::DegreesToRadians(ExplorationAngle)) * ExplorationRadius, 0.f);
}

void WanderStateRadevaDimana::AdvanceSpiral()
{
    ExplorationAngle += 45.f;
    ExplorationRadius = FMath::Min(ExplorationRadius + ExplorationRadiusStep, MaxExplorationRadius);
}

void WanderStateRadevaDimana::BuildPathTo(FVector Target)
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

void WanderStateRadevaDimana::PickNewNavMeshTarget()
{
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
    if (!NavSystem)
    {
        return;
    }

    FNavLocation Result;

    if (Memory && Memory->GetHouses().Num() > 0)
    {
        AActor* ClosestHouse = Memory->GetClosestHouse(Pawn->GetActorLocation());
        if (ClosestHouse && IsInsideHouse(ClosestHouse) && !Memory->IsHouseVisited(ClosestHouse))
        {
            FBox HouseBounds = ClosestHouse->GetComponentsBoundingBox();
            for (int i = 0; i < 20; i++)
            {
                if (NavSystem->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), SearchRadius, Result))
                {
                    FVector Location = Result.Location;
                    bool bIsOutside = Location.X < HouseBounds.Min.X || Location.X > HouseBounds.Max.X || Location.Y < HouseBounds.Min.Y || Location.Y > HouseBounds.Max.Y;

                    if (bIsOutside)
                    {
                        BuildPathTo(Result.Location);
                        GEngine->AddOnScreenDebugMessage(33, 2.f, FColor::Green, TEXT("Exit point found"));
                        return;
                    }
                }
            }
            GEngine->AddOnScreenDebugMessage(33, 0.f, FColor::Red, TEXT("Failed to find exit"));
            return;
        }
    }

    if (Memory)
    {
        AActor* UnvisitedHouse = Memory->GetClosestHouse(Pawn->GetActorLocation(), true);

        if (UnvisitedHouse && !IsInsideHouse(UnvisitedHouse) && !Collect->IsInventoryFull())
        {
            if (NavSystem->GetRandomReachablePointInRadius(UnvisitedHouse->GetActorLocation(), 300.f, Result))
            {
                BuildPathTo(Result.Location);
                return;
            }
        }
    }

    FVector ExplorePoint = SpawnLocation + FVector(FMath::Cos(FMath::DegreesToRadians(ExplorationAngle)) * ExplorationRadius, FMath::Sin(FMath::DegreesToRadians(ExplorationAngle)) * ExplorationRadius, 0.f);
    AdvanceSpiral();

    if (NavSystem->GetRandomReachablePointInRadius(ExplorePoint, 500.f, Result))
    {
        BuildPathTo(Result.Location);
    }
}

void WanderStateRadevaDimana::Update(float DeltaTime)
{
    if (!Pawn)
    {
        return;
    }

    if (Memory && Memory->GetHouses().Num() > 0)
    {
        AActor* ClosestHouse = Memory->GetClosestHouse(Pawn->GetActorLocation(), true);

        if (ClosestHouse)
        {
            bool bInside = IsInsideHouse(ClosestHouse);
            bool bVisited = Memory->IsHouseVisited(ClosestHouse);

            FBox Bounds = ClosestHouse->GetComponentsBoundingBox();
            
            DrawDebugBox(Pawn->GetWorld(), Bounds.GetCenter(), Bounds.GetExtent(), FColor::Yellow, false, -1.f, 0, 3.f);

            if (bInside && !bVisited)
            {
                if (!Collect->IsInventoryFull())
                {
                    Memory->MarkHouseVisited(ClosestHouse);
                    bSeekingHouse = false;
                    CurrentPath.Empty();
                    CurrentPathIndex = 0;
                    bHasTarget = false;
                }
            }
            else if (!bInside && !bVisited && !bSeekingHouse && !Collect->IsInventoryFull())
            {
                UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
                if (NavSystem)
                {
                    FNavLocation Result;
                    if (NavSystem->GetRandomReachablePointInRadius(ClosestHouse->GetActorLocation(), 300.f, Result))
                    {
                        bSeekingHouse = true;
                        BuildPathTo(Result.Location);
                    }
                }
            }
        }
    }

    if (!bHasTarget || FVector::Dist2D(Pawn->GetActorLocation(), NavTarget) < AcceptanceRadius)
    {
        bSeekingHouse = false;
        PickNewNavMeshTarget();
    }

    if (!bHasTarget)
    {
        return;
    }

    if (CurrentPathIndex < CurrentPath.Num() - 1)
    {
        if (FVector::Dist2D(Pawn->GetActorLocation(), CurrentPath[CurrentPathIndex]) < WaypointAcceptanceRadius)
        {
            //the pawn is close enough, 50.f, to the waypoint, so continue to the next one
            CurrentPathIndex++;
        }
    }

    if (CurrentPath.Num() > 0 && CurrentPathIndex < CurrentPath.Num())
    {
        FVector NextWaypoint = CurrentPath[CurrentPathIndex];

        FVector Direction = NextWaypoint - Pawn->GetActorLocation();
        Direction.Z = 0.f;
        Direction.Normalize();
        Pawn->AddMovementInput(Direction, 0.3f);
    }
    else
    {
        FVector Direction = NavTarget - Pawn->GetActorLocation();
        Direction.Z = 0.f;
        Direction.Normalize();
        Pawn->AddMovementInput(Direction, 0.3f);
    }
}