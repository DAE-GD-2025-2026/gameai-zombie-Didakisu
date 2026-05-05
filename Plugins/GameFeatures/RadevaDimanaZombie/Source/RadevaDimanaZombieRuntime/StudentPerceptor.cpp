// Fill out your copyright notice in the Description page of Project Settings.
#include "StudentPerceptor.h"
#include "Zombies/BaseZombie.h"
#include "SurvivorAIController.h"
#include "Survivor/SurvivorPawn.h"

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	/*GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));*/

	if (Cast<ABaseZombie>(Actor))
	{
		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, FString::Printf(TEXT("Saw a zombie!")));
	}
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

	GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Green, TEXT("Moving!"));
	FVector Direction = Pawn->GetActorForwardVector();
	Pawn->AddMovementInput(Direction, 1.f);
}
