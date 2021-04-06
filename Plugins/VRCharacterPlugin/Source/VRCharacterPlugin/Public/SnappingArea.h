#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "SnappingArea.generated.h"

class USphereComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class VRCHARACTERPLUGIN_API ASnappingArea : public AActor
{
	GENERATED_BODY()
	
	public:
	ASnappingArea();
	
	protected:

	public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature]: Component")
	UBoxComponent* SnapArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Feature]: Component")
	USphereComponent* SnapLocation;
	
};
