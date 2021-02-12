#pragma once

#include "CoreMinimal.h"
#include "PickupActor.h"
#include "VRHandMotionController.h"

#include "PickupActorPhysicBase.generated.h"

class UPhysicsConstraintComponent;
UCLASS()
class VRCHARACTERPLUGIN_API APickupActorPhysicBase : public APickupActor
{
	GENERATED_BODY()

public:
	APickupActorPhysicBase();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	UPhysicsConstraintComponent* PhysicsConstraintComponent;

	virtual void GrabPressed(UVRHandMotionController* AttachTo) override;
	virtual void GrabReleased() override;
};
