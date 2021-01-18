// Base Actor class for Linear Drive items, e.g. Drawer
// Game Lab Graz, Jan 2021


#include "VRCharacterPlugin/Public/LinearDriveActor.h"

#include "VRCharacterStatics.h"
#include "Components/SphereComponent.h"


ALinearDriveActor::ALinearDriveActor()
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
	BaseStaticMesh->SetSimulatePhysics(true);
	BaseStaticMesh->SetEnableGravity(false);
	BaseStaticMesh->SetConstraintMode(EDOFMode::SixDOF);

	BeginPoint = CreateDefaultSubobject<USphereComponent>(TEXT("BeginPoint"));
	BeginPoint->SetupAttachment(BaseStaticMesh);
	BeginPoint->SetSphereRadius(10.0f);
	BeginPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	BeginPoint->ShapeColor = FColor::Green;
	BeginPoint->SetHiddenInGame(true);

	EndPoint = CreateDefaultSubobject<USphereComponent>(TEXT("EndPoint"));
	EndPoint->SetupAttachment(BaseStaticMesh);
	EndPoint->SetSphereRadius(10.0f);
	EndPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	EndPoint->ShapeColor = FColor::Black;
	EndPoint->SetHiddenInGame(true);

	CustomAttachPoint = CreateDefaultSubobject<USphereComponent>(TEXT("CustomAttachPoint"));
	CustomAttachPoint->SetupAttachment(BaseStaticMesh);
	CustomAttachPoint->SetSphereRadius(10.0f);
	CustomAttachPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	CustomAttachPoint->ShapeColor = FColor::Black;
	CustomAttachPoint->SetHiddenInGame(true);
}

void ALinearDriveActor::BeginPlay()
{
	Super::BeginPlay();

	LocationInitialization();

	SlideDistance = FVector::Distance(InitialLocation,InitialGoal);
	SliderDirection = (InitialGoal - InitialLocation).GetSafeNormal();
}

FVector ALinearDriveActor::GetCustomAttachLocation() const
{
	return CustomAttachPoint->GetComponentLocation();
}

FRotator ALinearDriveActor::GetCustomAttachRotation() const
{
	return CustomAttachPoint->GetComponentRotation();
}

void ALinearDriveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsSliding)
	{
		SlidingAction();
	}
}

void ALinearDriveActor::GrabPressed(USceneComponent* AttachTo)
{
	bIsSliding = true;
	ControllerComponent = AttachTo;
}

void ALinearDriveActor::GrabReleased()
{
	bIsSliding = false;
	ControllerComponent = nullptr;
}

int ALinearDriveActor::GetGrabType()
{
	return TypeOfGrab;
}

void ALinearDriveActor::LocationInitialization()
{
	InitialLocation = BeginPoint->GetComponentLocation();
	InitialGoal = EndPoint->GetComponentLocation();
}

void ALinearDriveActor::SlidingAction()
{
	if(ControllerComponent == nullptr){return;}
	const auto ProjectedLocation = ControllerComponent->GetComponentLocation().ProjectOnTo(SliderDirection);
	const auto DistanceToBegin = FVector::Distance(ProjectedLocation, InitialLocation.ProjectOnTo(SliderDirection));
	const auto DistanceToEnd = FVector::Distance(ProjectedLocation, InitialGoal.ProjectOnTo(SliderDirection));
	SlidingRatio = FMath::Clamp(DistanceToBegin / SlideDistance, 0.0f,1.0f);
	if(DistanceToEnd > SlideDistance || DistanceToBegin > SlideDistance)
	{
		if(DistanceToEnd > DistanceToBegin){ SlidingRatio = 0;}
		else{SlidingRatio = 1;}
	}

	const auto UpdatedSliderLocation = InitialLocation + SlidingRatio * SlideDistance * SliderDirection;
	BaseStaticMesh->SetWorldLocation(UpdatedSliderLocation);

	if(FVector::Distance(ControllerComponent->GetComponentLocation(), CustomAttachPoint->GetComponentLocation()) > MaxDistanceToDetach)
	{
		GrabReleased();
	}
}

