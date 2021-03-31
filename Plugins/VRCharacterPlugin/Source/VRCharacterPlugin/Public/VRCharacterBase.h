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

struct FPredictProjectilePathResult;
struct FPredictProjectilePathParams;


UCLASS()
class VRCHARACTERPLUGIN_API AVRCharacterBase : public ACharacter
{
	GENERATED_BODY()

	// Methods
public:
	AVRCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostRegisterAllComponents() override;

	// Grab interaction
	void OnGrabRight();
	void OnGrabLeft();
	void OnReleaseRight();
	void OnReleaseLeft();
	static AActor* GetNearestOverlappingPickup(USphereComponent* SphereComponent);

	// Hand Pose
	void UpdateGripStatRight(const float AxisValue);
	void UpdateGripStatLeft(const float AxisValue);

	// Teleportation
	void OnPressTeleportRight();
	void OnReleaseTeleportRight();
	void OnPressTeleportLeft();
	void OnReleaseTeleportLeft();

	void UpdateDestinationMarker();
	bool UpdateProjectilePath(FPredictProjectilePathParams& PathParams, FPredictProjectilePathResult& PathResult,
	                          FVector StartPos, FVector LaunchVelocityVector) const;
	void UpdateProjectileSpline(TArray<FVector> PathPoints) const;
	void ClearProjectilePool();
	void UpdateProjectileMesh(TArray<FVector> PathPoints);
	void BeginTeleport() const;

	USkeletalMeshComponent* GetHandSkeletalMesh(USceneComponent* AttachTo) const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void RightHandBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
	                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void LeftHandBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

	UPROPERTY(EditAnywhere, Category = "[Feature] Debug")
	bool bDebugGrabSphere = false;

	UPROPERTY(EditAnywhere, Category = "[Feature] Debug")
	bool bDebugGrabAction = false;

	UPROPERTY(EditAnywhere, Category = "[Feature] Debug")
	bool bDebugHandPose = false;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Debug")
	bool bIsShowProjectionDebug = true;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float MaxTeleportDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float TeleportBeginOffset = 20.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float TeleportFadeDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float ProjectileDuration = 10.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float TeleportLaunchVelocity = 900.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float ProjectilePathRadius = 5.0f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	bool bIsTraceComplex = true;

	UPROPERTY(EditDefaultsOnly, Category = "[Feature]: Teleportation")
	UStaticMesh* TeleportProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = "[Feature]: Teleportation")
	UMaterialInterface* TeleportProjectileMaterial;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	TArray<AActor*> ActorsToIgnore;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	FVector ProjectNavigationExtend = FVector(500.0f, 500.0f, 500.0f);

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	UHapticFeedbackEffect_Base* TeleportationHaptic;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	float TeleportHapticIntensity = 0.3f;

	UPROPERTY(EditAnywhere, Category = "[Feature]: Teleportation")
	FColor CameraFadeColor = FColor::Black;

	UPROPERTY(VisibleAnywhere, Category = "[Feature]: Teleportation")
	TArray<class USplineMeshComponent*> ProjectileMeshPool;


private:

	bool bIsHitTeleportTarget = false;
	bool bIsUpdatingTeleportDestination = false;
	bool bIsDestinationFound = false;
	bool bIsRightHandDoTeleportation = true;
};
