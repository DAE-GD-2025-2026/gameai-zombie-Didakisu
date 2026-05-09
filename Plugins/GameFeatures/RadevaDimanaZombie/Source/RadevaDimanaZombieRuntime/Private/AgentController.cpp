#include "AgentController.h"
#include "Survivor/SurvivorPawn.h"
#include "GameFramework/FloatingPawnMovement.h"

AgentController::AgentController()
{

}

void AgentController::Initialize(ASurvivorPawn* InPawn)
{
	Pawn = InPawn;

	UFloatingPawnMovement* Movement = Pawn->GetComponentByClass<UFloatingPawnMovement>();
	if (Movement)
	{
		Movement->MaxSpeed = 750.f;
	}

	auto FleeStatePtr = std::make_unique<FleeState>(Pawn, &Memory);
	auto WanderStatePtr = std::make_unique<WanderState>(Pawn);

	Flee = FleeStatePtr.get();
	Wander = WanderStatePtr.get();

	StateMachine.AddState(std::move(FleeStatePtr));
	StateMachine.AddState(std::move(WanderStatePtr));

	//transitions
	StateMachine.AddTransition(Wander, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0;
	});

	StateMachine.AddTransition(Flee, Wander, [this]()
	{
		return Memory.GetZombies().Num() == 0;
	});

	StateMachine.SetInitialState(Wander);
}

void AgentController::Update(float DeltaTime)
{
	if (!Pawn)
	{
		return;
	}

	StateMachine.Update(DeltaTime);
}