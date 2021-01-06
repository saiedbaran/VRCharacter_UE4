#pragma once

#include "CoreMinimal.h"

#include "VRHandMotionController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "VRHandAnimationInstance.generated.h"

USTRUCT(BlueprintType)
struct FVRHandAnimationInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere)
	float Grip = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere)
	int TypeOfGrab = 0.0f;
};

UCLASS()
class VRCHARACTERPLUGIN_API UVRHandAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	UVRHandMotionController* HandMotionController;

protected:

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVRHandAnimationInstanceProxy GripProxy;

	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override
	{
		return &GripProxy;
	}

	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override
	{
	}

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditDefaultsOnly)
	bool bIsRightHanded = false;

	friend struct FVRHandAnimationInstanceProxy;
};
