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
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (Cast<ABaseZombie>(Actor))
	{
		FString Msg = FString::Printf(TEXT("Zombie sensed at: %s"), *Stimulus.StimulusLocation.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, Msg);

		GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, FString::Printf(TEXT("Saw a zombie!")));
		if (Stimulus.WasSuccessfullySensed())
		{
			Memory.RegisterZombie(Actor, Stimulus.StimulusLocation, CurrentTime);
		}
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

	DrawVisionCone(Pawn);

	//GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Green, TEXT("Moving!"));
	//FVector RandomPos = FVector(1000.f, 0.f, 0.f);

	/*FVector Direction = SeekBehavior.CalculateSteering(Pawn, RandomPos);
	Pawn->AddMovementInput(Direction, 1.f);*/

	//if (bZombieInSight)
	//{
	//	FVector Direction = FleeBehavior.CalculateSteering(Pawn, LastSeenZombieLocation);
	//	Pawn->AddMovementInput(Direction, 1.f);
	//}
}

void UStudentPerceptor::DrawVisionCone(ASurvivorPawn* Pawn)
{
	UAIPerceptionComponent* PerceptionComp = Pawn->GetComponentByClass<UAIPerceptionComponent>();
	if (!PerceptionComp)
	{
		return;
	}

	UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(PerceptionComp->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));
	if (!SightConfig)
	{
		return;
	}

	float SightRadius = SightConfig->SightRadius;
	float HalfAngle = SightConfig->PeripheralVisionAngleDegrees;

	FVector PawnLocation = Pawn->GetActorLocation();
	FVector ForwardVector = Pawn->GetActorForwardVector();

	FVector LeftDirection = ForwardVector.RotateAngleAxis(-HalfAngle, FVector::UpVector);
	FVector RightDirection = ForwardVector.RotateAngleAxis(HalfAngle, FVector::UpVector);

	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + LeftDirection * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + RightDirection * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
	DrawDebugLine(Pawn->GetWorld(), PawnLocation, PawnLocation + ForwardVector * SightRadius, FColor::Yellow, false, -1.f, 0, 2.f);
}