#pragma once

#include "CoreMinimal.h"
#include "AgentMemory.h"
#include "AgentFSM.h"

class ASurvivorPawn;

class AgentController
{
public:
	AgentController();

	void Initialize(ASurvivorPawn* InPawn);
	void Update(float DeltaTime);

	AgentMemory& GetMemory() { return Memory; }
	FSM& GetFSM() { return StateMachine; }
private:
	ASurvivorPawn* Pawn = nullptr;

	AgentMemory Memory;
	FSM StateMachine;
};