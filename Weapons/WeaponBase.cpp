// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include <TST/Player/SCharacter.h>
#include "Net/UnrealNetwork.h"

#include "TST/UI/HUD/GameHUD.h"
#include "TST/Player/SPlayerController.h"
#include <Runtime/Engine/Public/DrawDebugHelpers.h>

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent "));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionProfileName(TEXT("BlockAll"));
	
	MaxRange = 3000.0f;

	TotalAmmoCapacity = 60;
	MagazineCapacity = 10;

	AmmoRemainder = TotalAmmoCapacity;
	CurrentAmmo = MagazineCapacity;

	
	
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	CurrentAmmo = MagazineCapacity;


}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, AmmoRemainder, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, CurrentAmmo, COND_OwnerOnly);
}




void AWeaponBase::Fire()
{
	if (CurrentAmmo > 0)
	{
		//Decrement Ammo on client for widget purposes
		--CurrentAmmo;
		UpdateAmmo(CurrentAmmo, AmmoRemainder);

		FVector SpawnLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
		FRotator SpawnRotation = WeaponMesh->GetSocketRotation(FName("Muzzle"));

		//Convert the FRotator to Vector based for Optimization
		FVector_NetQuantize10 MuzzleRotation = FVector_NetQuantize10(SpawnRotation.Pitch, SpawnRotation.Yaw, SpawnRotation.Roll);

		if (!HasAuthority()) //not server
		{
			if (APawn* Character = Cast<APawn>(GetOwner()))
			{
				if (Character->IsLocallyControlled())// is the client that tried to fire
				{
					UE_LOG(LogTemp, Warning, (TEXT("Weapon Fire Called")));
					Server_Fire(FVector_NetQuantize10(SpawnLocation), MuzzleRotation);
				}
			}
		}
		else
		{
			Multi_Fire(FVector_NetQuantize10(SpawnLocation), MuzzleRotation);
		}
	}
}

FHitResult AWeaponBase::PerformLineTrace(FVector SpawnLocation, FRotator SpawnRotation)
{
	const FName TraceTag("Tracer");
	GetWorld()->DebugDrawTraceTag;

	FVector EndLocation = SpawnLocation + SpawnRotation.Vector() * MaxRange;
	DrawDebugLine(GetWorld(), SpawnLocation, EndLocation, FColor::Green, false, 3.0f, 0, 2.0f);
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.TraceTag = TraceTag;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByObjectType(HitResult, SpawnLocation, EndLocation, FCollisionObjectQueryParams(), QueryParams);

	PerformHit(HitResult);


	return HitResult;
}

void AWeaponBase::PerformHit(FHitResult HitResult)
{
	if (AActor* HitActor = HitResult.GetActor())
	{
		if (ASCharacter* ShotPlayer = Cast<ASCharacter>(HitActor))
		{
			if (GetWorld()->IsServer())
			{
				if (AActor* OwningWeapon = GetOwner())
				{
					if (ASCharacter* Shooter = Cast<ASCharacter>(OwningWeapon))
					{
						ShotPlayer->TakeDamage(25.0f, FDamageEvent(), nullptr, Shooter);
					}
				}
			}
		}
	}
}

bool AWeaponBase::Server_Fire_Validate(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation)
{

	return true;
}
void AWeaponBase::Server_Fire_Implementation(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation)
{
	if (CurrentAmmo > 0)
	{
		--CurrentAmmo;
		
		Multi_Fire(SpawnLocation, MuzzleRotation);
	}
}

bool AWeaponBase::Multi_Fire_Validate(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation)
{
	return true;
}
void AWeaponBase::Multi_Fire_Implementation(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation)
{
	if (APawn* Character = Cast<APawn>(GetOwner()))
	{ 
		if (Character->IsLocallyControlled()) {
			return;
		}
	}
	FRotator SpawnRotation = FRotator(MuzzleRotation.X, MuzzleRotation.Y, MuzzleRotation.Z);
	PerformLineTrace(SpawnLocation, SpawnRotation);
}




void AWeaponBase::UpdateAmmo(int32 Value1, int32 Value2)
{
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ASPlayerController* PlayerController = Cast<ASPlayerController>(*It);
		if (PlayerController != nullptr)
		{
			AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());

			if (GameHUD != nullptr)
			{
				GameHUD->UpdateAmmo(Value1, Value2);
				return;
			}
		}
	}
}

void AWeaponBase::OnEquip()
{
	UpdateAmmo(CurrentAmmo, AmmoRemainder);
}

void AWeaponBase::Reload()
{
	int32 AmmoDifference = MagazineCapacity - CurrentAmmo;
	int32 MinMaxAmmo = FMath::Clamp(AmmoDifference, 0, AmmoRemainder);

	if (MinMaxAmmo > 0)
	{
		AmmoRemainder -= MinMaxAmmo;
		CurrentAmmo += MinMaxAmmo;
		UpdateAmmo(CurrentAmmo, AmmoRemainder);

		if (HasAuthority())
		{
			Multi_Reload();
		}
		else
		{
			Server_Reload();
		}
	}
}

bool AWeaponBase::Server_Reload_Validate()
{
	return true;
}

void AWeaponBase::Server_Reload_Implementation()
{
	int32 AmmoDifference = MagazineCapacity - CurrentAmmo;
	int32 MinMaxAmmo = FMath::Clamp(AmmoDifference, 0, AmmoRemainder);

		AmmoRemainder -= MinMaxAmmo;
		CurrentAmmo += MinMaxAmmo;
		UE_LOG(LogTemp, Warning, (TEXT("Reloaded")));
		Multi_Reload();
}

bool AWeaponBase::Multi_Reload_Validate()
{
	return true;
}

void AWeaponBase::Multi_Reload_Implementation()
{

	if (ASCharacter* Character = Cast<ASCharacter>(GetOwner()))
	{
		if (Character->IsLocallyControlled())
		{
			return;
		}
	}
}





