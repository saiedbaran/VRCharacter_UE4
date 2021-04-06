#include "Inventory/InventoryItem.h"

AInventoryItem::AInventoryItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

