// Base Actor class for Pickup items
// Game Lab Graz, Jan 2021

#include "VRCharacterPlugin/Public/PickupActor.h"
#include "VRCharacterStatics.h"
#include "Components/SphereComponent.h"


void APickupActor::GenerateHighlightMesh()
{
	HighlightMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HighlightMeshComponent->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
	HighlightMeshComponent->SetMaterial(0,HighlightMaterial);
	HighlightMeshComponent->SetVisibility(false);
}

// Sets default values
APickupActor::APickupActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->BodyInstance.bAutoWeld = false;
	StaticMeshComponent->SetCollisionObjectType(ECC_Grabbable);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Block);
	StaticMeshComponent->SetSimulatePhysics(true);

	HighlightMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Highlight");
	HighlightMeshComponent->SetupAttachment(StaticMeshComponent);

	CustomAttachPoint = CreateDefaultSubobject<USphereComponent>(TEXT("CustomAttachPoint"));
	CustomAttachPoint->SetupAttachment(StaticMeshComponent);
	CustomAttachPoint->SetSphereRadius(10.0f);
	CustomAttachPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	CustomAttachPoint->ShapeColor = FColor::Emerald;
	CustomAttachPoint->SetHiddenInGame(true);

	StaticMeshComponent->OnComponentEndOverlap.AddDynamic(this, &APickupActor::StaticMeshEndOverlapped);
	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::StaticMeshBeginOverlapped);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	GenerateHighlightMesh();
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupActor::GrabPressed(UVRHandMotionController* AttachTo)
{
	if(!bIsActiveForInteraction) {return;}

	ToggleHighlight(false);
	
	StaticMeshComponent->SetSimulatePhysics(false);

	if (bSnapToHand)
	{
		StaticMeshComponent->AttachToComponent(AttachTo->GrabSphere, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		StaticMeshComponent->AttachToComponent(AttachTo->GrabSphere, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void APickupActor::GrabReleased()
{
	StaticMeshComponent->SetSimulatePhysics(true);
}

int APickupActor::GetGrabType()
{
	return TypeOfGrab;
}

void APickupActor::ToggleHighlight(bool bIsActivatingHighlight) const
{
	if(!bHasHighlight) {return;}
	HighlightMeshComponent->SetVisibility(bIsActivatingHighlight);
}

FVector APickupActor::GetCustomAttachLocation() const
{
	return CustomAttachPoint->GetComponentLocation();
}

FRotator APickupActor::GetCustomAttachRotation() const
{
	return CustomAttachPoint->GetComponentRotation();
}

void APickupActor::StaticMeshBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherComp->ComponentTags.Contains(TEXT("GrabSphere")))
	{
		HighlightMeshComponent->SetVisibility(true);
	}
}

void APickupActor::StaticMeshEndOverlapped(UPrimitiveComponent* OverlappedComp, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherComp->ComponentTags.Contains(TEXT("GrabSphere")))
	{
		HighlightMeshComponent->SetVisibility(false);
	}
}

