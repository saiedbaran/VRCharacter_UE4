#include "SnappingArea.h"
#include "Components/SphereComponent.h"


ASnappingArea::ASnappingArea()
{
	SnapArea = CreateDefaultSubobject<UBoxComponent>("SnapArea");
	SnapArea->ComponentTags.Add("SnapArea");
	SnapArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SnapArea->SetCollisionResponseToAllChannels(ECR_Overlap);
	
	SnapLocation = CreateDefaultSubobject<USphereComponent>("SnapLocation");
	SnapLocation->SetSphereRadius(0.1f);
	SnapLocation->SetupAttachment(SnapArea);
	SnapLocation->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SnapLocation->SetCollisionResponseToAllChannels(ECR_Ignore);
}
