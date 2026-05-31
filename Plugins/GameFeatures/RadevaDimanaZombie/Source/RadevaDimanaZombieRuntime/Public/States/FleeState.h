#pragma once
#include "../AgentFSM.h"
#include "../AgentMemory.h"
#include "SteeringBehaviors.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "States/CollectState.h"

class ASurvivorPawn;
class WanderState;

class FleeState : public State
{
public:
    FleeState(ASurvivorPawn* InPawn, AgentMemory* InMemory, WanderState* InWander, CollectState* InCollect);
    void OnEnter() override;
    void OnExit() override;
    void Update(float DeltaTime) override;
private:
    void BuildPathTo(FVector Target);
    void PickFleeTarget();
    bool IsInsideHouse(AActor* House) const;
    void FollowPath();

    ASurvivorPawn* Pawn = nullptr;
    AgentMemory* Memory = nullptr;
    WanderState* Wander = nullptr;
    CollectState* Collect = nullptr;

    FVector NavTarget;
    TArray<FVector> CurrentPath;
    int CurrentPathIndex = 0;
    bool bHasTarget = false;

    float AcceptanceRadius = 50.f;
    float WaypointAcceptanceRadius = 50.f;

    int LastPurgeZoneCount = 0;
};