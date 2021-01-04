// Motion Controller Class for VR Hand
// Game Lab Graz, Jan 2021

#include "VRCharacter/Public/VRHandMotionController.h"

#include "MotionControllerComponent.h"
#include "VRHandAnimationInstance.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "VRCharacter/Public/VRCharacterStatics.h"


UVRHandMotionController::UVRHandMotionController()
{
	PrimaryComponentTick.bCanEverTick = true;

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(GetAttachmentRoot());
	MotionController->SetCollisionProfileName("NoCollision");
	MotionController->SetGenerateOverlapEvents(false);

	HandSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("HandSkeletalMesh");
	HandSkeletalMesh->SetupAttachment(GetAttachmentRoot());
	HandSkeletalMesh->SetCollisionProfileName("PhysicsActor");
	HandSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	HandPivot = CreateDefaultSubobject<UStaticMeshComponent>("HandPivot");
	HandPivot->SetupAttachment(MotionController);
	HandPivot->SetCollisionObjectType(ECC_WorldDynamic);
	HandPivot->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HandPivot->SetCollisionResponseToAllChannels(ECR_Overlap);
	HandPivot->SetGenerateOverlapEvents(false);
	HandPivot->SetMobility(EComponentMobility::Movable);

	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>("PhysicsConstraint");
	PhysicsConstraint->SetupAttachment(MotionController);


	GrabSphere = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	GrabSphere->SetupAttachment(HandSkeletalMesh);
	GrabSphere->SetSphereRadius(7.0f);
	GrabSphere->ShapeColor = FColor::Red;
	GrabSphere->SetCollisionObjectType(ECC_WorldDynamic);
	GrabSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GrabSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	GrabSphere->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Overlap);
	GrabSphere->SetHiddenInGame(true);

	ArchDirection = CreateDefaultSubobject<UArrowComponent>("ArchDirection");
	ArchDirection->SetupAttachment(GrabSphere);
	ArchDirection->ArrowSize = 0.1f;
	ArchDirection->ArrowLength = 100;

	TeleportSpline = CreateDefaultSubobject<USplineComponent>("TeleportSpline");
	TeleportSpline->SetupAttachment(HandSkeletalMesh);
}

void UVRHandMotionController::BeginPlay()
{
	Super::BeginPlay();

	const auto Anim = Cast<UVRHandAnimationInstance>(HandSkeletalMesh->GetAnimInstance());
	if (Anim != nullptr) { Anim->HandMotionController = this; }

	//SetupPhysicsConstraint();
}

void UVRHandMotionController::SetupPhysicsConstraint()
{
	PhysicsConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 90.0f);
	PhysicsConstraint->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);
	PhysicsConstraint->SetLinearPositionDrive(true, true, true);
	PhysicsConstraint->SetLinearVelocityDrive(true, true, true);
	PhysicsConstraint->SetLinearDriveParams(2000, 200, 0);
	PhysicsConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
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

void UVRHandMotionController::SetTypeOfGrab(int TOG)
{
	TypeOfGrab = TOG;
}

float UVRHandMotionController::GetGripStat() const
{
	return GripState;
}

int UVRHandMotionController::GetTypeOfGrab() const
{
	return TypeOfGrab;
}
