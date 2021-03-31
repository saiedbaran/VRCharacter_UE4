// Base Actor class for Slider items
// Game Lab Graz, Jan 2021


#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "VRHandMotionController.h"
#include "GameFramework/Actor.h"
#include "SliderActor.generated.h"

class USphereComponent;

UCLASS()
class VRCHARACTERPLUGIN_API ASliderActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
public:
	ASliderActor();
	virtual void Tick(float DeltaTime) override;

	virtual void GrabPressed(UVRHandMotionController* AttachTo) override;
	virtual void GrabReleased() override;
	virtual int GetGrabType() override;

	void ToggleHighlight(bool bIsActivatingHighlight) const;
	void GenerateHighlightMesh() const;

protected:
	virtual void BeginPlay() override;

	FVector GetCustomAttachLocation() const;
	FRotator GetCustomAttachRotation() const;

	void SlidingAction();

	UFUNCTION()
	void StaticMeshBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void StaticMeshEndOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex);

	//Members
public:

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* BaseStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HandleStaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USphereComponent* BeginPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USphereComponent* EndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* HighlightMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* HighlightMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	bool bHasHighlight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USphereComponent* CustomAttachPoint;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bIsAutoWeld = true;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bSnapToHand = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomRotation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bUseCustomLocation = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Attach Behaviour")
	bool bIsShowingDebug = false;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Animation")
	TEnumAsByte<EGrabType> TypeOfGrab;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	USceneComponent* ControllerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Properties]: Attach Behaviour")
	float MaxDistanceToDetach = 100;

	UPROPERTY(EditAnywhere, Category = "[Properties]: Debug", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0",
		UIMax = "1.0"))
	float SlidingRatio = 0;


private:

	FVector SliderDirection;
	float SlideDistance;
	bool bIsSliding = false;
};
