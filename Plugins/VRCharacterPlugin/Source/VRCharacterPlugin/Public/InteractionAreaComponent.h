#pragma once

#include "CoreMinimal.h"

#include "InteractableInterface.h"
#include "Components/SphereComponent.h"
#include "InteractionAreaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRCHARACTERPLUGIN_API UInteractionAreaComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UInteractionAreaComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, Category = "[Properties]: Animation")
	bool bDistanceBaseAnimation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Animation")
	TEnumAsByte<EGrabType> TypeOfGrab;
	
protected:


};
