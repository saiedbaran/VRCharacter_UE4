// Base Actor class for Circular Drive items, e.g. Door
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"

#include "InteractableInterface.h"
#include "VRHandMotionController.h"
#include "GameFramework/Actor.h"
#include "CircularDriveActor.generated.h"

class UArrowComponent;
class USphereComponent;
UCLASS()
class VRCHARACTERPLUGIN_API ACircularDriveActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ACircularDriveActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GrabPressed(UVRHandMotionController* AttachTo) override;
	virtual void GrabReleased() override;
	virtual int GetGrabType() override;

	UFUNCTION(BlueprintCallable)
	void LocationInitialization();

protected:
	virtual void BeginPlay() override;

	void RotationAction();

	// Members

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BaseStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	float MaxRotation = 90;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	UArrowComponent* RotationAxis;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USphereComponent* RotationBeginPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USphereComponent* CustomAttachPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	bool bIsActiveForInteraction = true;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bIsAutoWeld = true;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bSnapToHand = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomRotation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomLocation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Animation")
	TEnumAsByte<EGrabType> TypeOfGrab;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USceneComponent* ControllerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	float MaxDistanceToDetach = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Debug")
	float RotationRatio = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Debug")
	float CurrentRotation = 0;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Debug")
	bool bIsRotating = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Debug")
	bool bIsShowingDebug = false;

protected:

	FVector InitialRotationBegin;
	FVector InitialRotationAxis;
	FVector InitialRotationPivot;
	FVector BaseLocationVector;
	FRotator InitialDriveRotation;
	FVector InitialForwardAxis;
	
};
