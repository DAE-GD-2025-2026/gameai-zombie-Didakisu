#pragma once
#include "../AgentFSMRadevaDimana.h"
#include "../AgentMemoryRadevaDimana.h"
#include "SteeringBehaviorsRadevaDimana.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "States/CollectStateRadevaDimana.h"

class ASurvivorPawn;
class WanderStateRadevaDimana;

class FleeStateRadevaDimana : public State
{
public:
    FleeStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory, WanderStateRadevaDimana* InWander, CollectStateRadevaDimana* InCollect);
    void OnEnter() override;
    void OnExit() override;
    void Update(float DeltaTime) override;
private:
    void BuildPathTo(FVector Target);
    void PickFleeTarget();
    bool IsInsideHouse(AActor* House) const;
    void FollowPath();

    ASurvivorPawn* Pawn = nullptr;
    AgentMemoryRadevaDimana* Memory = nullptr;
    WanderStateRadevaDimana* Wander = nullptr;
    CollectStateRadevaDimana* Collect = nullptr;

    FVector NavTarget;
    TArray<FVector> CurrentPath;
    int CurrentPathIndex = 0;
    bool bHasTarget = false;

    float AcceptanceRadius = 50.f;
    float WaypointAcceptanceRadius = 50.f;

    int LastPurgeZoneCount = 0;
};