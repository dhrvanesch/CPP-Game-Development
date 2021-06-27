// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFrameWork/InputSettings.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "TST/Player/SCharacter.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"


// Sets default values for this component's properties
UInteractorComponent::UInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bCanInteract = true;
}

// Send line trace and get hit actor
FHitResult UInteractorComponent::LineTraceSingle(const FVector& StartLocation, const FVector& EndLocation)
{	// Debug Line
	const FName TraceTag("Tracer");
	GetWorld()->DebugDrawTraceTag;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.TraceTag = TraceTag;

	if (bCanInteract)
	{
		GetWorld()->LineTraceSingleByObjectType(HitResult, StartLocation, EndLocation, FCollisionObjectQueryParams(), Params);
		/*bCanInteract = false;
		GetWorld()->GetTimerManager().SetTimer(InteractCooldown, this, &UInteractorComponent::CanInteract, .8f, false);*/
		
	}
	
	return HitResult;
	
}

void UInteractorComponent::CanInteract()
{
	bCanInteract = !bCanInteract;
}

// Called when the game starts
void UInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
		
	
}

