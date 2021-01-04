#include "VRCharacter/Public/VRHandAnimationInstance.h"

void FVRHandAnimationInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, const float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	UVRHandAnimationInstance* AnimInstance = Cast<UVRHandAnimationInstance>(InAnimInstance);
	if (IsValid(AnimInstance))
	{
		if (IsValid(AnimInstance->HandMotionController))
		{
			Grip = AnimInstance->HandMotionController->GetGripStat();
			TypeOfGrab = AnimInstance->HandMotionController->GetTypeOfGrab();
		}
	}
}

void UVRHandAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}
