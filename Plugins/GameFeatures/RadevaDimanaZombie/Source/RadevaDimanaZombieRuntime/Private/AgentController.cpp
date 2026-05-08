#include "AgentController.h"

AgentController::AgentController()
{

}

void AgentController::Initialize(ASurvivorPawn* InPawn)
{
	Pawn = InPawn;

	
}

void AgentController::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}
}