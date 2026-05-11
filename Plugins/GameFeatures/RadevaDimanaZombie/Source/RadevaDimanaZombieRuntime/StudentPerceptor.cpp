// Fill out your copyright notice in the Description page of Project Settings.
#include "StudentPerceptor.h"
#include "Zombies/BaseZombie.h"
#include "SurvivorAIController.h"
#include "Survivor/SurvivorPawn.h"
#include "Village/House/House.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpringArmComponent.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	AgentController.Initialize(Pawn);

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (Cast<ABaseZombie>(Actor))
	{
		FString Msg = FString::Printf(TEXT("Zombie sensed at: %s"), *Stimulus.StimulusLocation.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Msg);

		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, FString::Printf(TEXT("Saw a zombie!")));
		if (Stimulus.WasSuccessfullySensed())
		{
			AgentController.GetMemory().RegisterZombie(Actor, Stimulus.StimulusLocation/*, CurrentTime*/);
		}
		//zombies get cleaned when the ForgetTime completes

	}

	if (Cast<ABaseItem>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AgentController.GetMemory().RegisterItem(Actor, Stimulus.StimulusLocation);
		}

		//items shouldnt be forgotten that fast
		/*else
		{
			AgentController.GetMemory().UnregisterItem(Cast<ABaseItem>(Actor));
		}*/
	}

	/*if (Cast<AHouse>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AgentController.GetMemory().RegisterHouse(Actor, Stimulus.StimulusLocation);
		}
	}*/
}

void UStudentPerceptor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	if (!Pawn)
	{
		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, TEXT("No Pawn!"));
		return;
	}

	DrawVisionCone(Pawn);
	AgentController.Update(DeltaTime);
}

void UStudentPerceptor::DrawVisionCone(ASurvivorPawn* Pawn)
{
	UAIPerceptionComponent* PerceptionComp = Pawn->GetComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComp) return;

	UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(
		PerceptionComp->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
	if (!SightConfig) return;

	float SightRadius = SightConfig->SightRadius;
	float HalfAngle = SightConfig->PeripheralVisionAngleDegrees;

	FVector PawnLocation = Pawn->GetActorLocation();

	USkeletalMeshComponent* Mesh = Pawn->GetComponentByClass<USkeletalMeshComponent>();
	FVector ForwardVector = Mesh ? Mesh->GetRightVector() : Pawn->GetActorForwardVector();

	FVector LeftDirection = ForwardVector.RotateAngleAxis(-HalfAngle, FVector::UpVector);
	FVector RightDirection = ForwardVector.RotateAngleAxis(HalfAngle, FVector::UpVector);

	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + LeftDirection * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + RightDirection * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + ForwardVector * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
}