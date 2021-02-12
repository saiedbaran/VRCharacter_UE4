#include "VRCharacterPlugin/Public/PickupActorPhysicBase.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


APickupActorPhysicBase::APickupActorPhysicBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraintComponent"));
	PhysicsConstraintComponent->SetupAttachment(GetRootComponent());
	PhysicsConstraintComponent->SetAngularSwing1Limit(ACM_Limited, 15.0f);
	PhysicsConstraintComponent->SetAngularSwing2Limit(ACM_Limited, 10.0f);
	PhysicsConstraintComponent->SetAngularTwistLimit(ACM_Locked, 0.0f);
	PhysicsConstraintComponent->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	PhysicsConstraintComponent->SetAngularVelocityDriveTwistAndSwing(false, true);
	PhysicsConstraintComponent->SetAngularOrientationDrive(true, false);
	PhysicsConstraintComponent->SetAngularDriveParams(2500.0f, 100.0f, 0.0f);

	bSnapToHand = true;
}

void APickupActorPhysicBase::BeginPlay()
{
	Super::BeginPlay();
}

void APickupActorPhysicBase::GrabPressed(UVRHandMotionController* AttachTo)
{
	if(!bIsActiveForInteraction) {return;}
	
	if (bSnapToHand)
	{
		FVector AttachLocation = AttachTo->GrabSphere->GetComponentLocation();
		FRotator AttachRotation = AttachTo->GrabSphere->GetComponentRotation();

		if (bUseCustomLocation)
		{
			const FVector DeltaLocation = GetCustomAttachLocation() - StaticMeshComponent->GetComponentLocation();

			AttachLocation -= DeltaLocation;
		}
		if (bUseCustomRotation)
		{
			const FRotator DeltaRotation = GetCustomAttachRotation() - StaticMeshComponent->GetComponentRotation();
			AttachRotation -= DeltaRotation;
		}
		SetActorLocationAndRotation(AttachLocation, AttachRotation);

		if (!StaticMeshComponent->IsSimulatingPhysics())
		{
			StaticMeshComponent->SetSimulatePhysics(true);
		}

		PhysicsConstraintComponent->ConstraintActor1 = this;
		PhysicsConstraintComponent->ConstraintActor2 = AttachTo->GetOwner();

		// Note: Static Mesh Component should be the root component otherwise during simulation it will detach from root
		PhysicsConstraintComponent->SetConstrainedComponents(StaticMeshComponent, NAME_None,
                                                             Cast<UPrimitiveComponent>(AttachTo), NAME_None);
	}
}

void APickupActorPhysicBase::GrabReleased()
{
	PhysicsConstraintComponent->BreakConstraint();
}

	
