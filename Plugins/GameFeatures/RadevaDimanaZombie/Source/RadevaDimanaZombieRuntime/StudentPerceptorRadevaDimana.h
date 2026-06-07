// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"

#include "Public/AgentFSMRadevaDimana.h"
#include "Public/SteeringBehaviorsRadevaDimana.h"
#include "Public/AgentControllerRadevaDimana.h"

#include "StudentPerceptorRadevaDimana.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RADEVADIMANAZOMBIERUNTIME_API UStudentPerceptorRadevaDimana : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UStudentPerceptorRadevaDimana();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	Seek SeekBehavior;
	AgentControllerRadevaDimana AgentController;
	void DrawVisionCone(ASurvivorPawn* Pawn);
};
