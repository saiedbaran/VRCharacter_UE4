#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryItem.generated.h"

UCLASS()
class VRCHARACTERPLUGIN_API AInventoryItem : public AActor
{
	GENERATED_BODY()

public:
	AInventoryItem();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
