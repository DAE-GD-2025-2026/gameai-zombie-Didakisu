// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"

#include "Public/AgentFSM.h"
#include "Public/SteeringBehaviors.h"
//#include "Public/AgentMemory.h"
#include "Public/AgentController.h"

#include "StudentPerceptor.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RADEVADIMANAZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UStudentPerceptor();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	Seek SeekBehavior;
	Flee FleeBehavior;
	Wander WanderBehavior;

	AgentController AgentController;
	//AgentMemory Memory;

	void DrawVisionCone(ASurvivorPawn* Pawn);
};
