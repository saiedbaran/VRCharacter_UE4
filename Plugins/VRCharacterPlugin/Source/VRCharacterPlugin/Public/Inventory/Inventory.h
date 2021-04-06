#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

UCLASS()
class VRCHARACTERPLUGIN_API AInventory : public AActor
{
	GENERATED_BODY()

public:
	AInventory();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
};
