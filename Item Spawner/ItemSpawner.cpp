// Fill out your copyright notice in the Description page of Project Settings.



#include "ItemSpawner.h"
#include "GameFramework/WorldSettings.h"
#include "TST\Spawn\WeaponSpawn.h"
#include "TST/Weapons/WeaponBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bWeaponSpawnsReady = false;
}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		SpawnWeapons();
	}
}

bool AItemSpawner::Server_SpawnWeapons_Validate()
{
	return true;
}

void AItemSpawner::Server_SpawnWeapons_Implementation()
{

	if (bWeaponSpawnsReady == false)
		GetWeaponSpawns();

	for (AWeaponSpawn* WeaponSpawn : WeaponSpawnPoints)
	{
		if (!WeaponSpawn->IsTaken())
		{
			FRotator SpawnRotation = WeaponSpawn->GetActorRotation();
			FVector SpawnLocation = WeaponSpawn->GetActorLocation();
			if (AActor* Weapon = GetWorld()->SpawnActor<AActor>(WeaponToSpawn, SpawnLocation, SpawnRotation))
			{
				{
					WeaponSpawn->SetTaken(true);
					UE_LOG(LogTemp, Warning, TEXT("Spawned Weapon"));
				}
			}
		}
	}
}

void AItemSpawner::GetWeaponSpawns()
{
	TArray<AActor*> WeaponSpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponSpawn::StaticClass(), WeaponSpawnActors);
	if (WeaponSpawnActors.Num())
	{
		for (AActor* Actor : WeaponSpawnActors)
		{
			if (AWeaponSpawn* WeaponSpawn = Cast<AWeaponSpawn>(Actor))
			{
				WeaponSpawnPoints.Add(WeaponSpawn);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Weaponspawns: %d"), WeaponSpawnActors.Num());
	bWeaponSpawnsReady = true;
}

void AItemSpawner::SpawnWeapons()
{

	Server_SpawnWeapons();
}



