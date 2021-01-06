// Pawn Class for VR applications in Unreal Engine
// Game Lab Graz, Jan 2021


#include "VRCharacterPlugin/Public/VRCharacterBase.h"
#include "InteractableInterface.h"
#include "MotionControllerComponent.h"
#include "VRHandMotionController.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"

AVRCharacterBase::AVRCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	VROffset = CreateDefaultSubobject<USceneComponent>("VROffset");
	VROffset->SetupAttachment(GetRootComponent());
	VROffset->SetRelativeLocation(FVector(0, 0, -88));

	TeleportIndicator = CreateDefaultSubobject<UStaticMeshComponent>("TeleportIndicator");
	TeleportIndicator->SetupAttachment(VROffset);

	VRCamera = CreateDefaultSubobject<UCameraComponent>("VRCamera");
	VRCamera->SetupAttachment(VROffset);

	RightHand = CreateDefaultSubobject<UVRHandMotionController>("RightHand");
	RightHand->SetupAttachment(VROffset);
	RightHand->MotionController->MotionSource = FName("Right");

	LeftHand = CreateDefaultSubobject<UVRHandMotionController>("LeftHand");
	LeftHand->SetupAttachment(VROffset);
	LeftHand->MotionController->MotionSource = FName("Left");
}

void AVRCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (bDebugGrabSphere)
	{
		RightHand->ToggleGrabSphereHiddenInGame(false);
		LeftHand->ToggleGrabSphereHiddenInGame(false);
	}

	TeleportIndicator->SetVisibility(false);
}

void AVRCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVRCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("GrabRight", IE_Pressed, this, &AVRCharacterBase::OnGrabRight);
	PlayerInputComponent->BindAction("GrabLeft", IE_Pressed, this, &AVRCharacterBase::OnGrabLeft);
	PlayerInputComponent->BindAction("GrabRight", IE_Released, this, &AVRCharacterBase::OnReleaseRight);
	PlayerInputComponent->BindAction("GrabLeft", IE_Released, this, &AVRCharacterBase::OnReleaseLeft);

	PlayerInputComponent->BindAxis("GrabAxisRight", this, &AVRCharacterBase::UpdateGripStatRight);
	PlayerInputComponent->BindAxis("GrabAxisLeft", this, &AVRCharacterBase::UpdateGripStatLeft);
}

void AVRCharacterBase::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	// There is a bug when I activate these two
	//RightHand->Initialization();
	//LeftHand->Initialization();
}

void AVRCharacterBase::OnGrabRight()
{
	if (bDebugGrabAction)
	{
		UE_LOG(LogTemp, Log, TEXT("Grabing Right"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Red, TEXT("Grabing Right"));
		}
	}

	AActor* NearestPickup = GetNearestOverlappingPickup(RightHand->GrabSphere);

	RightHand->AttachedObject = Cast<IInteractableInterface>(NearestPickup);
	if (RightHand->AttachedObject != nullptr)
	{
		RightHand->SetTypeOfGrab(RightHand->AttachedObject->GetGrabType());
		RightHand->AttachedObject->GrabPressed(RightHand->GrabSphere);
		RightHand->GripState = GripClose;

		if (RightHand->AttachedObject == LeftHand->AttachedObject)
		{
			LeftHand->AttachedObject = nullptr;
			LeftHand->GripState = GripOpen;
		}
	}

	// [Gravity Grab] //
	/*
	else if (HitResult.GetActor() != nullptr)
	{
		if (bIsShowingGravityGrabTrace)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black,
                                             FString::Printf(
                                                 TEXT("Gravity Grabbing Actor: %s"), *HitResult.Actor->GetName()));
		}

		RightAttachedPickup = Cast<IInteractableInterface>(HitResult.GetActor());
		if (RightAttachedPickup != nullptr)
		{
			bIsTrackingRightAxis = false;
			bIsGravityGrabbingRight = true;
            
			RightAttachedPickup->GrabPressed(RightGrabSphere);
			RightGripStat = GripClose;
            
		}
	}
	*/
}

void AVRCharacterBase::OnGrabLeft()
{
	if (bDebugGrabAction)
	{
		UE_LOG(LogTemp, Log, TEXT("Grabing Left"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Red, TEXT("Grabing Left"));
		}
	}

	AActor* NearestPickup = GetNearestOverlappingPickup(LeftHand->GrabSphere);

	LeftHand->AttachedObject = Cast<IInteractableInterface>(NearestPickup);
	if (LeftHand->AttachedObject != nullptr)
	{
		LeftHand->SetTypeOfGrab(LeftHand->AttachedObject->GetGrabType());
		LeftHand->AttachedObject->GrabPressed(LeftHand->GrabSphere);
		LeftHand->GripState = GripClose;

		if (RightHand->AttachedObject == LeftHand->AttachedObject)
		{
			RightHand->AttachedObject = nullptr;
			RightHand->GripState = GripOpen;
		}
	}
}

void AVRCharacterBase::OnReleaseRight()
{
	if (RightHand->AttachedObject == nullptr) { return; }

	RightHand->AttachedObject->GrabReleased();
	RightHand->AttachedObject = nullptr;
	RightHand->bIsTrackingHandPose = true;

	// [Gravity Grab]
	//bIsGravityGrabbingRight = false;

	RightHand->GripState = GripOpen;
	RightHand->SetTypeOfGrab(DefaultTypeOfGrab);
}

void AVRCharacterBase::OnReleaseLeft()
{
	if (LeftHand->AttachedObject == nullptr) { return; }

	LeftHand->AttachedObject->GrabReleased();
	LeftHand->AttachedObject = nullptr;
	LeftHand->bIsTrackingHandPose = true;

	// [Gravity Grab]
	//bIsGravityGrabbingRight = false;

	LeftHand->GripState = GripOpen;
	LeftHand->SetTypeOfGrab(DefaultTypeOfGrab);
}

AActor* AVRCharacterBase::GetNearestOverlappingPickup(USphereComponent* SphereComponent)
{
	AActor* NearestPickup = nullptr;
	TArray<UPrimitiveComponent*> OverlappingComponents;

	SphereComponent->GetOverlappingComponents(OverlappingComponents);

	/*
	for (auto Component : OverlappingComponents)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Red,
			                                 FString::Printf(TEXT("OverlappedActor: %s"), *Component->GetName()));
		}
	}
	*/

	float ShortestDistance = TNumericLimits<float>::Max();
	for (auto Component : OverlappingComponents)
	{
		if (Component->GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
		{
			const auto Distance = FVector::Distance(SphereComponent->GetComponentLocation(),
			                                        Component->GetComponentLocation());
			if (Distance < ShortestDistance)
			{
				ShortestDistance = Distance;
				NearestPickup = Component->GetOwner();
				//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 20.0f, FColor::Black,
				//                                FString::Printf(TEXT("Nearest Pickup: %s"), *Component->GetName()));
			}
		}
	}

	return NearestPickup;
}

void AVRCharacterBase::UpdateGripStatRight(const float AxisValue)
{
	if (!RightHand->bIsTrackingHandPose) { return; }

	if (bDebugHandPose)
	{
		UE_LOG(LogTemp, Log, TEXT("Axis Value %f"), AxisValue);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Blue,
                                             FString::Printf(TEXT("Right Axis Value %f"), AxisValue));
		}
	}

	RightHand->GripState = AxisValue;
}

void AVRCharacterBase::UpdateGripStatLeft(const float AxisValue)
{
	if (!LeftHand->bIsTrackingHandPose) { return; }

	if (bDebugHandPose)
	{
		UE_LOG(LogTemp, Log, TEXT("Axis Value %f"), AxisValue);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 3.0f, FColor::Blue,
                                             FString::Printf(TEXT("Left Axis Value %f"), AxisValue));
		}
	}

	LeftHand->GripState = AxisValue;
}
