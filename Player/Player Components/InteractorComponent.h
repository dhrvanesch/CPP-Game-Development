
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "InteractorComponent.generated.h"
/*
Spacer
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TST_API UInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractorComponent();

	UPROPERTY()
		bool bCanInteract;

public:// Linetrace With Start and End
	FHitResult LineTraceSingle(const FVector& StartLocation, const FVector& EndLocation);

public:
	FTimerHandle InteractCooldown;

	void CanInteract();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	
};
