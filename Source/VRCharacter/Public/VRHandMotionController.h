// Motion Controller Class for VR Hand
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "VRHandMotionController.generated.h"


class USplineComponent;
class UArrowComponent;
class USphereComponent;
class UMotionControllerComponent;
class UPhysicsConstraintComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRCHARACTER_API UVRHandMotionController : public USceneComponent
{
	GENERATED_BODY()

	// Methods	
public:
	UVRHandMotionController();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SetupPhysicsConstraint();
	void ToggleGrabSphereHiddenInGame(bool bIsHiddenInGame) const;
	void SetTypeOfGrab(int TOG);
	float GetGripStat() const;
	int GetTypeOfGrab() const;

protected:
	virtual void BeginPlay() override;

private:

	// Members
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UMotionControllerComponent* MotionController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UStaticMeshComponent* HandPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	USkeletalMeshComponent* HandSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	USphereComponent* GrabSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UArrowComponent* ArchDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	USplineComponent* TeleportSpline;

	IInteractableInterface* AttachedObject;

	float GripState = 0;
	int TypeOfGrab = 0;
	bool bIsTrackingHandPose = true;

protected:

private:
};
