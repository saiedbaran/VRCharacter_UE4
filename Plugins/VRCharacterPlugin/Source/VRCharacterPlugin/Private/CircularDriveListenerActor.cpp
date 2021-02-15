// Listener for CircularDriveActor class
// Game Lab Graz, Feb 2021


#include "VRCharacterPlugin/Public/CircularDriveListenerActor.h"


ACircularDriveListenerActor::ACircularDriveListenerActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACircularDriveListenerActor::BeginPlay()
{
	Super::BeginPlay();

	// TODO: Modify the execution order later to remove this part
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ACircularDriveListenerActor::SetReferenceToCircularDrive, 1);
}

void ACircularDriveListenerActor::SetReferenceToCircularDrive()
{
	if(ReferenceCircularDrive)
	{
		ReferenceCircularDrive->OnHandleAction.AddUObject(this, &ACircularDriveListenerActor::OnHandleActivation);
		ReferenceCircularDrive->OnHandleDeactivate.AddUObject(this, &ACircularDriveListenerActor::OnHandleDeactivate);
	}
}

void ACircularDriveListenerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

