﻿// Base Actor class for Circular Drive items, e.g. Door
// Game Lab Graz, Jan 2021

#include "VRCharacterPlugin/Public/CircularDriveActor.h"
#include "VRCharacterStatics.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


ACircularDriveActor::ACircularDriveActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(BaseStaticMesh);
	BaseStaticMesh->SetCollisionObjectType(ECC_Grabbable);
	BaseStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseStaticMesh->SetGenerateOverlapEvents(true);
	BaseStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BaseStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	BaseStaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	BaseStaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	BaseStaticMesh->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Block);
	BaseStaticMesh->SetEnableGravity(false);
	BaseStaticMesh->SetConstraintMode(EDOFMode::SixDOF);

	RotationBeginPoint = CreateDefaultSubobject<USphereComponent>(TEXT("BeginPoint"));
	RotationBeginPoint->SetupAttachment(BaseStaticMesh);
	RotationBeginPoint->SetSphereRadius(5.0f);
	RotationBeginPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	RotationBeginPoint->ShapeColor = FColor::Green;
	RotationBeginPoint->SetHiddenInGame(true);

	CustomAttachPoint = CreateDefaultSubobject<USphereComponent>(TEXT("CustomAttachPoint"));
	CustomAttachPoint->SetupAttachment(BaseStaticMesh);
	CustomAttachPoint->SetSphereRadius(10.0f);
	CustomAttachPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	CustomAttachPoint->ShapeColor = FColor::Black;
	CustomAttachPoint->SetHiddenInGame(true);

	RotationAxis = CreateDefaultSubobject<UArrowComponent>(TEXT("RotationAxis"));
	RotationAxis->SetupAttachment(BaseStaticMesh);
	RotationAxis->SetHiddenInGame(true);

	HighlightMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Highlight");
	HighlightMeshComponent->SetupAttachment(BaseStaticMesh);

	BaseStaticMesh->OnComponentEndOverlap.AddDynamic(this, &ACircularDriveActor::StaticMeshEndOverlapped);
	BaseStaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ACircularDriveActor::StaticMeshBeginOverlapped);
}

void ACircularDriveActor::BeginPlay()
{
	Super::BeginPlay();

	LocationInitialization();
	RotationLimitInitialization();

	GenerateHighlightMesh();
}

void ACircularDriveActor::RotationAction()
{
	if (ControllerComponent == nullptr) { return; }

	ToggleHighlight(false);
	
	const auto ProjectedLocation = FVector::PointPlaneProject(ControllerComponent->GetComponentLocation(),
	                                                          InitialRotationPivot,
	                                                          InitialRotationAxis);

	const auto CurrentLocationVector = ProjectedLocation - InitialRotationPivot;
	const auto CurrentCross = FVector::CrossProduct(CurrentLocationVector.GetSafeNormal(),
	                                                BaseLocationVector.GetSafeNormal());
	const auto CurrentDir = FVector::DotProduct(CurrentCross, InitialRotationAxis);
	if (CurrentDir > 0) { return; }

	RotationRatio = FVector::DotProduct(CurrentLocationVector.GetSafeNormal(), BaseLocationVector.GetSafeNormal());
	CurrentRotation = FMath::GetMappedRangeValueClamped(FVector2D(1, -1), FVector2D(0, 180), RotationRatio);

	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black,
		                                 FString::Printf(TEXT("CurrentRotation: %f"), CurrentRotation));
	}*/

	const auto ClampedRotationAngle = FMath::ClampAngle(CurrentRotation, 0, MaxRotation);
	const auto NewRotationVector = UKismetMathLibrary::RotateAngleAxis(InitialForwardAxis, ClampedRotationAngle,
	                                                                   InitialRotationAxis);
	const auto CurrentRotator = UKismetMathLibrary::MakeRotFromXY(NewRotationVector, InitialRotationAxis);
	BaseStaticMesh->SetWorldRotation(CurrentRotator);

	/*const auto CurrentRotator = UKismetMathLibrary::RotatorFromAxisAndAngle(
		InitialRotationAxis, -1 * FMath::ClampAngle(CurrentRotation, 0, MaxRotation));
	BaseStaticMesh->SetWorldRotation(CurrentRotator+InitialDriveRotation);*/
}

bool ACircularDriveActor::CheckForHandleAction() const
{
	if (CurrentRotation >= ActivateRotation) { return true; }
	else { return false; }
}

void ACircularDriveActor::ReactiveHandle()
{
	if (CurrentRotation <= DeactivateRotation)
	{
		bIsActiveForAction = true;
		OnHandleDeactivate.Broadcast();
	}
}

void ACircularDriveActor::RotationLimitInitialization()
{
	if (MaxRotation - ActivateRotation < 1) { ActivateRotation = MaxRotation - 1; }
	if (DeactivateRotation < 1) { DeactivateRotation = 1; }
}

void ACircularDriveActor::StaticMeshBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                    bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentTags.Contains(TEXT("GrabSphere")))
	{
		HighlightMeshComponent->SetVisibility(true);
	}
}

void ACircularDriveActor::StaticMeshEndOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->ComponentTags.Contains(TEXT("GrabSphere")))
	{
		HighlightMeshComponent->SetVisibility(false);
	}
}

void ACircularDriveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRotating)
	{
		RotationAction();
		if (bIsActiveForAction && CheckForHandleAction())
		{
			bIsActiveForAction = false;
			OnHandleAction.Broadcast();
		}
		else if (!bIsActiveForAction)
		{
			ReactiveHandle();
		}
	}
}

void ACircularDriveActor::GrabPressed(UVRHandMotionController* AttachTo)
{
	if (!bIsActiveForInteraction) { return; }

	bIsRotating = true;
	ControllerComponent = AttachTo->GrabSphere;
}

void ACircularDriveActor::GrabReleased()
{
	bIsRotating = false;
	ControllerComponent = nullptr;
}

int ACircularDriveActor::GetGrabType()
{
	return TypeOfGrab;
}

void ACircularDriveActor::LocationInitialization()
{
	InitialRotationBegin = RotationBeginPoint->GetComponentLocation();
	InitialRotationPivot = RotationAxis->GetComponentLocation();
	InitialRotationAxis = RotationAxis->GetForwardVector();
	BaseLocationVector = InitialRotationBegin - InitialRotationPivot;
	InitialDriveRotation = BaseStaticMesh->GetComponentRotation();
	InitialForwardAxis = BaseStaticMesh->GetForwardVector();
}

void ACircularDriveActor::ToggleHighlight(bool bIsActivatingHighlight) const
{
	if (!bHasHighlight) { return; }
	HighlightMeshComponent->SetVisibility(bIsActivatingHighlight);
}

void ACircularDriveActor::GenerateHighlightMesh() const
{
	HighlightMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightMeshComponent->SetStaticMesh(BaseStaticMesh->GetStaticMesh());
	HighlightMeshComponent->SetMaterial(0, HighlightMaterial);
	HighlightMeshComponent->SetVisibility(false);
}
