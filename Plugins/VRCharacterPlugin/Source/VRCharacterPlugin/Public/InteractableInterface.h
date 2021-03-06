﻿// Interface for All interactable objects
// Game Lab Graz, Jan 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class UVRHandMotionController;

UENUM()
enum EGrabType
{
	Controller UMETA(DisplayName = "Controller"),
    Box UMETA(DisplayName = "Box"),
    Bow UMETA(DisplayName = "Bow"),
    Sphere UMETA(DisplayName = "Sphere"),
    Pointing UMETA(DisplayName = "Pointing"),
	Lever UMETA(DisplayName = "Lever"),
};

UINTERFACE()
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class VRCHARACTERPLUGIN_API IInteractableInterface
{
	GENERATED_BODY()

public:

	virtual void GrabPressed(UVRHandMotionController* AttachTo)
	{
	}

	virtual void GrabReleased()
	{
	}

	virtual int GetGrabType() { return 0; }
};
