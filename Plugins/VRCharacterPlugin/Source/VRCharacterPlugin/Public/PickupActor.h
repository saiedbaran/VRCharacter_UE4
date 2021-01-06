// Base Actor class for Pickup items
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class USphereComponent;

UCLASS()
class VRCHARACTERPLUGIN_API APickupActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

	// Methods
public:
	APickupActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GrabPressed(USceneComponent* AttachTo) override;
	virtual void GrabReleased() override;
	virtual int GetGrabType() override;

protected:
	virtual void BeginPlay() override;

	FVector GetCustomAttachLocation() const;
	FRotator GetCustomAttachRotation() const;

	//Members
public:

protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "[Properties]: Attach Behaviour")
	USphereComponent* CustomAttachPoint;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bSnapToHand = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomRotation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomLocation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bIsShowingDebug = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool Testbool = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Animation")
	TEnumAsByte<EGrabType> TypeOfGrab;
};
