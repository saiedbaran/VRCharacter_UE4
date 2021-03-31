// Motion Controller Class for VR Hand
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "InteractionAreaComponent.h"
#include "VRHandMotionController.generated.h"


class USplineComponent;
class UArrowComponent;
class USphereComponent;
class UMotionControllerComponent;
class UPhysicsConstraintComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRCHARACTERPLUGIN_API UVRHandMotionController : public USceneComponent
{
	GENERATED_BODY()

	// Methods	
public:
	UVRHandMotionController();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void Initialization();
	void SetMesh();
	void SetupPhysicsConstraint();
	void ToggleGrabSphereHiddenInGame(bool bIsHiddenInGame) const;
	void SetGrabSphereOffset();

	void SetTypeOfGrab(int TOG);
	float GetGripStat() const;
	int GetTypeOfGrab() const;

	void ChangePhysicalBehaviour(bool isCollisionEnabled, bool isSimulatingPhysics);
	UPrimitiveComponent* GetNearestOverlappingComponent() const;

	UFUNCTION()
	void GrabSphereOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);

	UFUNCTION()
	void GrabSphereEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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
	bool bTrackDistanceBaseGripStat = false;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Overwrite")
	USkeletalMesh* HandMeshVisualisation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Overwrite")
	UStaticMesh* HandPivotVisualisation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Overwrite")
	TSubclassOf<UAnimInstance> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Overwrite")
	FVector GrabSphereOffset;

	UPROPERTY(Transient)
	UInteractionAreaComponent* InteractionAreaComponent = nullptr;

private:
};
