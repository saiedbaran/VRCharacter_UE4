// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacterPlugin/Public/PickupActor.h"
#include "VRCharacterStatics.h"
#include "Components/SphereComponent.h"


// Sets default values
APickupActor::APickupActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);
	StaticMeshComponent->SetCollisionObjectType(ECC_Grabbable);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMeshComponent->SetGenerateOverlapEvents(true);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Grabbable, ECR_Block);
	StaticMeshComponent->SetSimulatePhysics(true);

	CustomAttachPoint = CreateDefaultSubobject<USphereComponent>(TEXT("CustomAttachPoint"));
	CustomAttachPoint->SetupAttachment(StaticMeshComponent);
	CustomAttachPoint->SetSphereRadius(10.0f);
	CustomAttachPoint->SetCollisionResponseToAllChannels(ECR_Ignore);
	CustomAttachPoint->ShapeColor = FColor::Emerald;
	CustomAttachPoint->SetHiddenInGame(true);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupActor::GrabPressed(USceneComponent* AttachTo)
{
	StaticMeshComponent->SetSimulatePhysics(false);

	if (bSnapToHand)
	{
		StaticMeshComponent->AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		StaticMeshComponent->AttachToComponent(AttachTo, FAttachmentTransformRules::KeepWorldTransform);
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

FVector APickupActor::GetCustomAttachLocation() const
{
	return CustomAttachPoint->GetComponentLocation();
}

FRotator APickupActor::GetCustomAttachRotation() const
{
	return CustomAttachPoint->GetComponentRotation();
}

