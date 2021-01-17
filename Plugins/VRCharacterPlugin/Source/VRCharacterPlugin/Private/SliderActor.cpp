// Base Actor class for Slider items
// Game Lab Graz, Jan 2021


#include "VRCharacterPlugin/Public/SliderActor.h"
#include "VRCharacterStatics.h"
#include "Components/SphereComponent.h"


ASliderActor::ASliderActor()
{
	PrimaryActorTick.bCanEverTick = true;

	BaseStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(BaseStaticMesh);
	BaseStaticMesh->SetCollisionObjectType(ECC_Grabbable);
	BaseStaticMesh->SetSimulatePhysics(false);

	HandleStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("HandleStaticMesh");
	HandleStaticMesh->SetupAttachment(BaseStaticMesh);
	HandleStaticMesh->BodyInstance.bAutoWeld = false;
	HandleStaticMesh->SetCollisionObjectType(ECC_Grabbable);
	HandleStaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HandleStaticMesh->SetGenerateOverlapEvents(true);
	HandleStaticMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HandleStaticMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	HandleStaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	HandleStaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	HandleStaticMesh->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Block);
	HandleStaticMesh->SetSimulatePhysics(true);
	HandleStaticMesh->SetEnableGravity(false);
	HandleStaticMesh->SetConstraintMode(EDOFMode::SixDOF);

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
	CustomAttachPoint->SetupAttachment(HandleStaticMesh);
	CustomAttachPoint->SetSphereRadius(10.0f);
	CustomAttachPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	CustomAttachPoint->ShapeColor = FColor::Black;
	CustomAttachPoint->SetHiddenInGame(true);
}

void ASliderActor::BeginPlay()
{
	Super::BeginPlay();

	SlideDistance = FVector::Distance(EndPoint->GetComponentLocation(),BeginPoint->GetComponentLocation());
	SliderDirection = (EndPoint->GetComponentLocation() - BeginPoint->GetComponentLocation()).GetSafeNormal();

	// To be removed
	if(TestActor){bIsSliding = true;}

	
}

void ASliderActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsSliding)
	{
		SlidingAction();
	}
}

void ASliderActor::GrabPressed(USceneComponent* AttachTo)
{
}

void ASliderActor::GrabReleased()
{
}

int ASliderActor::GetGrabType()
{
	return TypeOfGrab;
}

FVector ASliderActor::GetCustomAttachLocation() const
{
	return CustomAttachPoint->GetComponentLocation();
}

FRotator ASliderActor::GetCustomAttachRotation() const
{
	return CustomAttachPoint->GetComponentRotation();
}

void ASliderActor::SlidingAction()
{
	const auto ProjectedLocation = TestActor->GetActorLocation().ProjectOnTo(SliderDirection);
	const auto DistanceToBegin = FVector::Distance(ProjectedLocation, BeginPoint->GetComponentLocation().ProjectOnTo(SliderDirection));
	const auto DistanceToEnd = FVector::Distance(ProjectedLocation, EndPoint->GetComponentLocation().ProjectOnTo(SliderDirection));
	SlidingRatio = FMath::Clamp(DistanceToBegin / SlideDistance, 0.0f,1.0f);
	if(DistanceToEnd > SlideDistance || DistanceToBegin > SlideDistance)
	{
		if(DistanceToEnd > DistanceToBegin){ SlidingRatio = 0;}
		else{SlidingRatio = 1;}
	}

	const auto UpdatedSliderLocation = BeginPoint->GetComponentLocation() + SlidingRatio * SlideDistance * SliderDirection;
	HandleStaticMesh->SetWorldLocation(UpdatedSliderLocation);
}
