#include "Inventory/Inventory.h"

AInventory::AInventory()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInventory::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

