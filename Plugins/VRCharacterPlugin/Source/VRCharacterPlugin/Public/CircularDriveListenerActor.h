// Listener for CircularDriveActor class
// Game Lab Graz, Feb 2021

#pragma once

#include "CoreMinimal.h"
#include "CircularDriveActor.h"
#include "GameFramework/Actor.h"
#include "CircularDriveListenerActor.generated.h"

UCLASS()
class VRCHARACTERPLUGIN_API ACircularDriveListenerActor : public AActor
{
	GENERATED_BODY()

public:
	ACircularDriveListenerActor();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHandleActivation();

	UFUNCTION(BlueprintImplementableEvent)
	void OnHandleDeactivate();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetReferenceToCircularDrive();

	// Members
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "00_Property: References")
	ACircularDriveActor* ReferenceCircularDrive;
};
