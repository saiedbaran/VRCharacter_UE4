// Motion Controller Class for VR Hand
// Game Lab Graz, Jan 2021

#include "VRCharacterPlugin/Public/VRHandMotionController.h"

#include "MotionControllerComponent.h"
#include "VRHandAnimationInstance.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "VRCharacterPlugin/Public/VRCharacterStatics.h"


UVRHandMotionController::UVRHandMotionController()
{
	PrimaryComponentTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(
		*FString::Printf(TEXT("MotionController %s"), *this->GetName()));
	MotionController->SetupAttachment(GetAttachmentRoot());
	MotionController->SetCollisionProfileName("NoCollision");
	MotionController->SetGenerateOverlapEvents(false);

	HandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		*FString::Printf(TEXT("HandSkeletalMesh %s"), *this->GetName()));
	HandSkeletalMesh->SetupAttachment(GetAttachmentRoot());
	HandSkeletalMesh->SetCollisionProfileName("PhysicsActor");
	HandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	HandPivot = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("HandPivot %s"), *this->GetName()));
	HandPivot->SetupAttachment(MotionController);
	HandPivot->SetCollisionObjectType(ECC_WorldDynamic);
	HandPivot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HandPivot->SetCollisionResponseToAllChannels(ECR_Overlap);
	HandPivot->SetGenerateOverlapEvents(false);
	HandPivot->SetMobility(EComponentMobility::Movable);
	HandPivot->SetHiddenInGame(true);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(
		*FString::Printf(TEXT("PhysicsConstraint %s"), *this->GetName()));
	PhysicsConstraint->SetupAttachment(MotionController);


	GrabSphere = CreateDefaultSubobject<USphereComponent>(*FString::Printf(TEXT("GrabSphere %s"), *this->GetName()));
	GrabSphere->SetupAttachment(HandSkeletalMesh);
	GrabSphere->SetSphereRadius(7.0f);
	GrabSphere->ShapeColor = FColor::Red;
	GrabSphere->SetCollisionObjectType(ECC_WorldDynamic);
	GrabSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrabSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	GrabSphere->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Overlap);
	GrabSphere->SetHiddenInGame(true);
	GrabSphere->ComponentTags.Add(TEXT("GrabSphere"));

	ArchDirection = CreateDefaultSubobject<UArrowComponent>(
		*FString::Printf(TEXT("ArchDirection %s"), *this->GetName()));
	ArchDirection->SetupAttachment(GrabSphere);
	ArchDirection->ArrowSize = 0.1f;
	ArchDirection->ArrowLength = 100;

	TeleportSpline = CreateDefaultSubobject<USplineComponent>(
		*FString::Printf(TEXT("TeleportSpline %s"), *this->GetName()));
	TeleportSpline->SetupAttachment(HandSkeletalMesh);
}


void UVRHandMotionController::BeginPlay()
{
	Super::BeginPlay();

	Initialization();

	GrabSphere->OnComponentBeginOverlap.AddDynamic(this, &UVRHandMotionController::GrabSphereOverlapEvent);
}

void UVRHandMotionController::Initialization()
{
	SetMesh();

	HandSkeletalMesh->SetAnimInstanceClass(Animation);

	const auto Anim = Cast<UVRHandAnimationInstance>(HandSkeletalMesh->GetAnimInstance());
	if (Anim != nullptr) { Anim->HandMotionController = this; }

	HandPivot->SetSimulatePhysics(false);
	HandSkeletalMesh->SetSimulatePhysics(true);
	HandSkeletalMesh->SetCollisionObjectType(ECC_VRCharacter);
	HandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandSkeletalMesh->SetGenerateOverlapEvents(true);
	HandSkeletalMesh->SetNotifyRigidBodyCollision(true);


	SetupPhysicsConstraint();

	SetGrabSphereOffset();
}

void UVRHandMotionController::SetMesh()
{
	if (HandMeshVisualisation) { HandSkeletalMesh->SetSkeletalMesh(HandMeshVisualisation); }
	if (HandPivotVisualisation) { HandPivot->SetStaticMesh(HandPivotVisualisation); }
}

void UVRHandMotionController::SetupPhysicsConstraint()
{
	FConstraintProfileProperties Profile;
	Profile.bEnableProjection = false;
	Profile.bDisableCollision = true;
	PhysicsConstraint->ConstraintInstance.CopyProfilePropertiesFrom(Profile);

	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetLinearPositionDrive(true, true, true);
	PhysicsConstraint->SetLinearVelocityDrive(true, true, true);
	PhysicsConstraint->SetLinearDriveParams(200000000.0, 20000000.0, 0);
	PhysicsConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	PhysicsConstraint->SetAngularOrientationTarget(FRotator(0));
	PhysicsConstraint->SetAngularDriveParams(500, 50, 0.0);
	PhysicsConstraint->SetConstrainedComponents(HandPivot, NAME_None, HandSkeletalMesh, NAME_None);
}

void UVRHandMotionController::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVRHandMotionController::ToggleGrabSphereHiddenInGame(const bool bIsHiddenInGame) const
{
	GrabSphere->SetHiddenInGame(bIsHiddenInGame);
}

void UVRHandMotionController::SetGrabSphereOffset()
{
	GrabSphere->SetRelativeLocation(GrabSphere->GetRelativeLocation() + GrabSphereOffset);
	//if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Black, TEXT("GrabSphereOffset"));}
}

void UVRHandMotionController::SetTypeOfGrab(int TOG)
{
	TypeOfGrab = TOG;
}

float UVRHandMotionController::GetGripStat() const
{
	if (bIsTrackingHandPose) { return GripState; }
	if (bTrackDistanceBaseGripStat && InteractionAreaComponent)
	{
		const auto Distance = FVector::Distance(GrabSphere->GetComponentLocation(),
		                                        InteractionAreaComponent->GetComponentLocation());
		const auto ComponentRadius = InteractionAreaComponent->GetScaledSphereRadius();
		const auto StatValue = FMath::GetMappedRangeValueClamped(FVector2D(0, ComponentRadius), FVector2D(1, 0),
		                                                         Distance);
		
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, FString::Printf(TEXT("Grip stat: %f"), StatValue));
		}
		*/

		return StatValue;
	}
	return 1;
}

int UVRHandMotionController::GetTypeOfGrab() const
{
	return TypeOfGrab;
}

void UVRHandMotionController::ChangePhysicalBehaviour(bool isCollisionEnabled, bool isSimulatingPhysics)
{
	if(isCollisionEnabled){HandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);}
	else {HandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);}
	
	//HandSkeletalMesh->SetSimulatePhysics(isSimulatingPhysics);
}

UPrimitiveComponent* UVRHandMotionController::GetNearestOverlappingComponent() const
{
	UPrimitiveComponent* NearestInteractionArea = nullptr;
	TArray<UPrimitiveComponent*> OverlappingComponents;

	GrabSphere->GetOverlappingComponents(OverlappingComponents);

	float ShortestDistance = TNumericLimits<float>::Max();
	for (auto Component : OverlappingComponents)
	{
		if (Component->ComponentTags.Contains(InteractionArea))
		{
			const auto Distance = FVector::Distance(GrabSphere->GetComponentLocation(),
                                                    Component->GetComponentLocation());
			if (Distance < ShortestDistance)
			{
				ShortestDistance = Distance;
				NearestInteractionArea = Component;
			}
		}
	}

	return NearestInteractionArea;
}

void UVRHandMotionController::SetFixInteractionPose(UPrimitiveComponent* const interactionArea)
{
	InteractionAreaComponent = Cast<UInteractionAreaComponent>(interactionArea);
	if (InteractionAreaComponent)
	{
		bIsTrackingHandPose = false;
		if (InteractionAreaComponent->bDistanceBaseAnimation)
		{
			bTrackDistanceBaseGripStat = true;
		}
		SetTypeOfGrab(InteractionAreaComponent->TypeOfGrab);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,10, FColor::Red, TEXT("Cast failed"));
	}
}

void UVRHandMotionController::GrabSphereOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                     bool bFromSweep, const FHitResult& SweepResult)
{
	/*auto Tags = OtherComp->ComponentTags;
	for (auto Tag : Tags)
	{
		if (Tag == InteractionArea)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Hand begin overlapped"))

			InteractionAreaComponent = Cast<UInteractionAreaComponent>(OtherComp);
			if (InteractionAreaComponent)
			{
				bIsTrackingHandPose = false;
				if (InteractionAreaComponent->bDistanceBaseAnimation)
				{
					bTrackDistanceBaseGripStat = true;
				}
				SetTypeOfGrab(InteractionAreaComponent->TypeOfGrab);
			}
		}
	}*/

	const auto interactionArea = GetNearestOverlappingComponent();
	
	if(interactionArea == nullptr)
	{
		bIsTrackingHandPose = true;
		bTrackDistanceBaseGripStat = false;

		GEngine->AddOnScreenDebugMessage(-1,10, FColor::Black, TEXT("Clear Interaction Pose"));
	}
	else
	{
		SetFixInteractionPose(interactionArea);
		GEngine->AddOnScreenDebugMessage(-1,10, FColor::Blue, TEXT("Change Interaction Pose"));
	}
	
	GEngine->AddOnScreenDebugMessage(-1,10, FColor::Green, TEXT("Set Interaction Pose"));
}

void UVRHandMotionController::GrabSphereEndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const auto interactionArea = GetNearestOverlappingComponent();

	if(interactionArea == nullptr)
	{
		bIsTrackingHandPose = true;
		bTrackDistanceBaseGripStat = false;

		GEngine->AddOnScreenDebugMessage(-1,10, FColor::Black, TEXT("Clear Interaction Pose"));
	}
	else
	{
		SetFixInteractionPose(interactionArea);
		GEngine->AddOnScreenDebugMessage(-1,10, FColor::Blue, TEXT("Change Interaction Pose"));
	}
	
	/*auto Tags = OtherComp->ComponentTags;
	for (auto Tag : Tags)
	{
		if (Tag == InteractionArea)
		{
			bIsTrackingHandPose = true;
			bTrackDistanceBaseGripStat = false;
		}
	}*/
}
