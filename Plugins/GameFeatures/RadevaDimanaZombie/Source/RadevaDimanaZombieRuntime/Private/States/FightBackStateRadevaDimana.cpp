#include "States/FightBackStateRadevaDimana.h"
#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Items/ItemType.h" 
#include "Zombies/BaseZombie.h" 

FightBackStateRadevaDimana::FightBackStateRadevaDimana(ASurvivorPawn* InPawn, AgentMemoryRadevaDimana* InMemory)
{
    Pawn = InPawn;
    Memory = InMemory;
}

void FightBackStateRadevaDimana::OnEnter()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Entering FightBack State"));
}

void FightBackStateRadevaDimana::OnExit()
{
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Exiting FightBack State"));
}

void FightBackStateRadevaDimana::Update(float DeltaTime)
{
    FireCooldown -= DeltaTime;

    if (Memory->GetZombies().Num() == 0)
    {
        Spin(DeltaTime);
    }
    else
    {
        FaceTarget(DeltaTime);
        if (FireCooldown <= 0.f)
        {
            Shoot();
            FireCooldown = FireRate;
        }
    }
}

bool FightBackStateRadevaDimana::HasWeapon() const
{
    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    const TArray<ABaseItem*>& Items = Inventory->GetInventory();

    for (int i = 0; i < Items.Num(); i++)
    {
        if (Items[i] && Items[i]->GetValue() > 0 && (Items[i]->GetItemType() == EItemType::Pistol || Items[i]->GetItemType() == EItemType::Shotgun))
        {
            return true;
        }
    }
    return false;
}

void FightBackStateRadevaDimana::FaceTarget(float DeltaTime)
{
    FVector ZombieLocation = Memory->GetClosestZombieLocation(Pawn->GetActorLocation());
    FVector Direction = ZombieLocation - Pawn->GetActorLocation();
    Direction.Z = 0.f;
    Direction.Normalize();

    FRotator TargetRotation = Direction.Rotation();
    Pawn->SetActorRotation(TargetRotation);
}

int FightBackStateRadevaDimana::FindItemOfType(UInventoryComponent* Inventory, EItemType Type)
{
    const TArray<ABaseItem*>& Items = Inventory->GetInventory();

    for (int i = 0; i < Items.Num(); i++)
    {
        if (Items[i] && Items[i]->GetItemType() == Type)
        {
            return i;
        }
    }
    return -1;
}

void FightBackStateRadevaDimana::Shoot()
{
    DrawDebugLine(Pawn->GetWorld(),  Pawn->GetActorLocation(), Pawn->GetActorLocation() + Pawn->GetActorForwardVector() * 1000.f, FColor::Red, false, 2.f, 0, 3.f);

    UInventoryComponent* Inventory = Pawn->GetComponentByClass<UInventoryComponent>();
    int Slot = -1;

    if (Memory->GetZombies().Num() > 1)
    {
        Slot = FindItemOfType(Inventory, EItemType::Shotgun);
    }

    if (Slot == -1)
    {
        Slot = FindItemOfType(Inventory, EItemType::Pistol);
    }

    if (Slot == -1)
    {
        Slot = FindItemOfType(Inventory, EItemType::Shotgun);
    }

    if (Slot != -1)
    {
        bool bUsed = Inventory->UseItem(Slot);
        if (bUsed)
        {
            Inventory->RemoveItem(Slot);
            GEngine->AddOnScreenDebugMessage(30, 2.f, FColor::Yellow, TEXT("Shooting zombie!"));
        }
    }
}

void FightBackStateRadevaDimana::Spin(float DeltaTime)
{
    FRotator CurrentRotation = Pawn->GetActorRotation();
    CurrentRotation.Yaw += SpinSpeed * DeltaTime;
    Pawn->SetActorRotation(CurrentRotation);
}