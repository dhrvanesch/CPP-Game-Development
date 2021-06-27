// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawner.generated.h"

UCLASS()
class TST_API AItemSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawner();
public:
	UPROPERTY(EditAnywhere)
		TArray<class AWeaponSpawn*> WeaponSpawnPoints;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> WeaponToSpawn;
		/*TArray<class AActor*> WeaponToSpawn;*/

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bWeaponSpawnsReady;

public:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnWeapons();
	bool Server_SpawnWeapons_Validate();
	void Server_SpawnWeapons_Implementation();

	void GetWeaponSpawns();
	void SpawnWeapons();


};
