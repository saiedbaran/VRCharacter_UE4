// Pawn Class for VR applications in Unreal Engine
// Game Lab Graz, Jan 2021


#include "VRCharacterPlugin/Public/VRCharacterBase.h"
#include "InteractableInterface.h"
#include "MotionControllerComponent.h"
#include "NavigationSystem.h"
#include "VRHandMotionController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"

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

	if (bIsUpdatingTeleportDestination)
	{
		UpdateDestinationMarker();
	}
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

	PlayerInputComponent->BindAction("TeleportRight", IE_Pressed, this, &AVRCharacterBase::OnPressTeleportRight);
	PlayerInputComponent->BindAction("TeleportRight", IE_Released, this, &AVRCharacterBase::OnReleaseTeleportRight);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Pressed, this, &AVRCharacterBase::OnPressTeleportLeft);
	PlayerInputComponent->BindAction("TeleportLeft", IE_Released, this, &AVRCharacterBase::OnReleaseTeleportLeft);
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
		RightHand->AttachedObject->GrabPressed(RightHand);
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
		LeftHand->AttachedObject->GrabPressed(LeftHand);
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

void AVRCharacterBase::OnPressTeleportRight()
{
	bIsUpdatingTeleportDestination = true;
	bIsRightHandDoTeleportation = true;
	RightHand->bIsTrackingHandPose = false;
	RightHand->SetTypeOfGrab(Pointing);
}

void AVRCharacterBase::OnReleaseTeleportRight()
{
	if (!bIsDestinationFound) { return; }

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(
        0, 1, TeleportFadeDelay, CameraFadeColor, false, true);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AVRCharacterBase::BeginTeleport, TeleportFadeDelay);

	bIsUpdatingTeleportDestination = false;
	TeleportIndicator->SetVisibility(false);
	bIsDestinationFound = false;
	ClearProjectilePool();
	bIsRightHandDoTeleportation = false;
	RightHand->bIsTrackingHandPose = true;
	RightHand->SetTypeOfGrab(DefaultTypeOfGrab);
}

void AVRCharacterBase::OnPressTeleportLeft()
{
	if (bIsRightHandDoTeleportation) { return; }
	bIsUpdatingTeleportDestination = true;
	LeftHand->bIsTrackingHandPose = false;
	LeftHand->SetTypeOfGrab(Pointing);
}

void AVRCharacterBase::OnReleaseTeleportLeft()
{
	if (!bIsDestinationFound) { return; }

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(
        0, 1, TeleportFadeDelay, CameraFadeColor, false, true);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AVRCharacterBase::BeginTeleport, TeleportFadeDelay);

	bIsUpdatingTeleportDestination = false;
	TeleportIndicator->SetVisibility(false);
	bIsDestinationFound = false;
	LeftHand->bIsTrackingHandPose = true;
	LeftHand->SetTypeOfGrab(DefaultTypeOfGrab);
	ClearProjectilePool();
}

void AVRCharacterBase::UpdateDestinationMarker()
{
	auto ActiveHand = bIsRightHandDoTeleportation ? RightHand->HandSkeletalMesh : LeftHand->HandSkeletalMesh;

	const FVector StartPos = ActiveHand->GetComponentLocation() + ActiveHand->GetForwardVector() * TeleportBeginOffset;
	const FVector LaunchVelocityVector = ActiveHand->GetForwardVector() * TeleportLaunchVelocity;

	FPredictProjectilePathParams PathParams(ProjectilePathRadius, StartPos, LaunchVelocityVector, ProjectileDuration,
                                            ECC_Visibility, this);
	FPredictProjectilePathResult PathResult;
	bIsHitTeleportTarget = UpdateProjectilePath(PathParams, PathResult, StartPos, LaunchVelocityVector);

	FNavLocation PointOnNavMeshLocation;
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	const auto bIsProjectedToNavMesh = NavSystem->ProjectPointToNavigation(
        PathResult.HitResult.Location, PointOnNavMeshLocation);

	if (bIsHitTeleportTarget && bIsProjectedToNavMesh)
	{
		TeleportIndicator->SetWorldLocation(PointOnNavMeshLocation.Location);

		auto ForwardVector = ActiveHand->GetForwardVector();
		ForwardVector = FVector(ForwardVector.X, ForwardVector.Y, 0);

		TeleportIndicator->SetWorldRotation(ForwardVector.Rotation());
		TeleportIndicator->SetVisibility(true);
		bIsDestinationFound = true;

		TArray<FVector> PathPoints;
		for (auto Point : PathResult.PathData)
		{
			PathPoints.Add(Point.Location);
		}

		UpdateProjectileSpline(PathPoints);
		UpdateProjectileMesh(PathPoints);
	}
	else
	{
		TeleportIndicator->SetVisibility(false);
		ClearProjectilePool();
		bIsDestinationFound = false;
	}
}

bool AVRCharacterBase::UpdateProjectilePath(FPredictProjectilePathParams& PathParams,
	FPredictProjectilePathResult& PathResult, FVector StartPos, FVector LaunchVelocityVector) const
{
	if (bIsShowProjectionDebug)
	{
		PathParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	}
	PathParams.bTraceComplex = bIsTraceComplex;
	const bool bIsHit = UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

	return bIsHit;
}

void AVRCharacterBase::UpdateProjectileSpline(TArray<FVector> PathPoints) const
{
	const auto CurrentProjectileSpline = bIsRightHandDoTeleportation ? RightHand->TeleportSpline : LeftHand->TeleportSpline;

	CurrentProjectileSpline->ClearSplinePoints(false);


	for (int32 i = 0; i < PathPoints.Num(); i++)
	{
		FVector LocalPosition = CurrentProjectileSpline->GetComponentTransform().
                                                         InverseTransformPosition(PathPoints[i]);
		FSplinePoint SplinePoint(i, LocalPosition);
		CurrentProjectileSpline->AddPoint(SplinePoint, false);
	}

	CurrentProjectileSpline->UpdateSpline();
}

void AVRCharacterBase::ClearProjectilePool()
{
	if (ProjectileMeshPool.Num() != 0)
	{
		for (auto PMesh : ProjectileMeshPool)
		{
			PMesh->DestroyComponent();
		}
		ProjectileMeshPool.Empty();
	}
}

void AVRCharacterBase::UpdateProjectileMesh(TArray<FVector> PathPoints)
{
	ClearProjectilePool();

	const auto ActiveHand = bIsRightHandDoTeleportation ? RightHand->HandSkeletalMesh : LeftHand->HandSkeletalMesh;
	const auto CurrentProjectileSpline = bIsRightHandDoTeleportation ? RightHand->TeleportSpline : LeftHand->TeleportSpline;
	FVector StartPos, EndPos, StartTangent, EndTangent;
	const auto NumberOfSegments = PathPoints.Num() - 1;
	for (int32 i = 0; i < NumberOfSegments; ++i)
	{
		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>();
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->AttachToComponent(ActiveHand, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMesh->SetStaticMesh(TeleportProjectileMesh);
		SplineMesh->SetMaterial(0, TeleportProjectileMaterial);
		SplineMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		SplineMesh->RegisterComponent();
		SplineMesh->RegisterComponentWithWorld(GetWorld());
		ProjectileMeshPool.Add(SplineMesh);

		CurrentProjectileSpline->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTangent);
		CurrentProjectileSpline->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent);
		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	}
}

void AVRCharacterBase::BeginTeleport() const
{
	GetRootComponent()->SetWorldLocation(
    TeleportIndicator->GetComponentLocation() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * GetActorUpVector());

	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraFade(1, 0, TeleportFadeDelay, CameraFadeColor);
}

USkeletalMeshComponent* AVRCharacterBase::GetHandSkeletalMesh(USceneComponent* AttachTo) const
{
	if(AttachTo == RightHand->GrabSphere){return RightHand->HandSkeletalMesh;}
	else if(AttachTo == LeftHand->GrabSphere) {return LeftHand->HandSkeletalMesh;}
	return nullptr;
}
