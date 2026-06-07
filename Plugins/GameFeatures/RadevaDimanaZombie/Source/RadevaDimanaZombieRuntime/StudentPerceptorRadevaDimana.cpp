// Fill out your copyright notice in the Description page of Project Settings.
#include "StudentPerceptorRadevaDimana.h"
#include "Zombies/BaseZombie.h"
#include "SurvivorAIController.h"
#include "Survivor/SurvivorPawn.h"
#include "Village/House/House.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "PurgeZones/PurgeZone.h"

UStudentPerceptorRadevaDimana::UStudentPerceptorRadevaDimana()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptorRadevaDimana::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, TEXT("BeginPlay called!"));
	
	ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
	AgentController.Initialize(Pawn);

	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptorRadevaDimana::OnPerceptionUpdated);
	}
}

void UStudentPerceptorRadevaDimana::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
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
			ASurvivorPawn* Pawn = Cast<ASurvivorPawn>(GetOwner());
			AgentController.GetMemory().RegisterItem(Actor, Stimulus.StimulusLocation, Pawn->GetActorLocation());
		}
	}

	if (Cast<AHouse>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AgentController.GetMemory().RegisterHouse(Actor, Stimulus.StimulusLocation);
		}
	}

	if (Cast<APurgeZone>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			AgentController.GetMemory().RegisterPurgeZone(Actor, Stimulus.StimulusLocation);
		}
	}
}

void UStudentPerceptorRadevaDimana::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

void UStudentPerceptorRadevaDimana::DrawVisionCone(ASurvivorPawn* Pawn)
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