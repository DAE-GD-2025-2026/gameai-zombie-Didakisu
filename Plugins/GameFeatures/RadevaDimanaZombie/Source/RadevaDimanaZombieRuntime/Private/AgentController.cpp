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
	auto CollectStatePtr = std::make_unique<CollectState>(Pawn, &Memory);

	Flee = FleeStatePtr.get();
	Wander = WanderStatePtr.get();
	Collect = CollectStatePtr.get();

	StateMachine.AddState(std::move(FleeStatePtr));
	StateMachine.AddState(std::move(WanderStatePtr));
	StateMachine.AddState(std::move(CollectStatePtr));

	//transitions
	StateMachine.AddTransition(Wander, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0;
	});

	StateMachine.AddTransition(Flee, Wander, [this]()
	{
		return Memory.GetZombies().Num() == 0;
	});

	StateMachine.AddTransition(Wander, Collect, [this]()
	{
		return Memory.GetItems().Num() > 0;
	});

	StateMachine.AddTransition(Collect, Wander, [this]()
	{
		return Memory.GetItems().Num() == 0;
	});

	StateMachine.AddTransition(Collect, Flee, [this]()
	{
		return Memory.GetZombies().Num() > 0;
	});

	StateMachine.AddTransition(Flee, Collect, [this]()
	{
		return Memory.GetZombies().Num() == 0 && Memory.GetItems().Num() > 0;
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