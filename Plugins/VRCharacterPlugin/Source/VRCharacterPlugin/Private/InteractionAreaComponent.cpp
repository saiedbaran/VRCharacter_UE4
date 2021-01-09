#include "VRCharacterPlugin/Public/InteractionAreaComponent.h"
#include "VRCharacterPlugin/Public/VRCharacterStatics.h"


UInteractionAreaComponent::UInteractionAreaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	this->ComponentTags.Add(InteractionArea);

	this->UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->UPrimitiveComponent::SetCollisionObjectType(ECC_Grabbable);
	this->UPrimitiveComponent::SetCollisionResponseToAllChannels(ECR_Overlap);
}


void UInteractionAreaComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionAreaComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

