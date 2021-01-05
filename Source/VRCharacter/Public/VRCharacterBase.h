// Pawn Class for VR applications in Unreal Engine
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "VRCharacterBase.generated.h"


class USphereComponent;
class UVRHandMotionController;
class UCameraComponent;

constexpr float GripOpen = 0.0f;
constexpr float GripClose = 1.0f;
constexpr int DefaultTypeOfGrab = 0;


UCLASS()
class VRCHARACTER_API AVRCharacterBase : public ACharacter
{
	GENERATED_BODY()

	// Methods
public:
	AVRCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostRegisterAllComponents() override;

	void OnGrabRight();
	void OnGrabLeft();
	void OnReleaseRight();
	void OnReleaseLeft();
	static AActor* GetNearestOverlappingPickup(USphereComponent* SphereComponent);

	void UpdateGripStatRight(const float AxisValue);
	void UpdateGripStatLeft(const float AxisValue);

protected:
	virtual void BeginPlay() override;

private:

	// Members
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	USceneComponent* VROffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UStaticMeshComponent* TeleportIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UCameraComponent* VRCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UVRHandMotionController* RightHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] components")
	UVRHandMotionController* LeftHand;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Debug")
	bool bDebugGrabSphere = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Debug")
	bool bDebugGrabAction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature] Debug")
	bool bDebugHandPose = false;

	

private:


};
