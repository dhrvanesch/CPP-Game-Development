#pragma once

#include "CoreMinimal.h"
#include "TST/Interface/Interactable.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class TST_API AWeaponBase : public AActor /*,public IInteractable*/
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		int32 TotalAmmoCapacity;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		int32 MagazineCapacity;

	UPROPERTY(BlueprintReadOnly, Replicated,  Category = "Weapon")
		int32 CurrentAmmo;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Weapon")
		int32 AmmoRemainder;



	UPROPERTY(EditAnywhere, Category = "Weapon")
	float MaxRange;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PerformHit(FHitResult HitResult);
	FHitResult PerformLineTrace(FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Fire(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);
	bool Server_Fire_Validate(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);
	void Server_Fire_Implementation(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void Multi_Fire(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);
	bool Multi_Fire_Validate(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);
	void Multi_Fire_Implementation(FVector_NetQuantize10 SpawnLocation, FVector_NetQuantize10 MuzzleRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Reload();
	bool Server_Reload_Validate();
	void Server_Reload_Implementation();

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void Multi_Reload();
	bool Multi_Reload_Validate();
	void Multi_Reload_Implementation();



public:
	void UpdateAmmo(int32 Value1, int32 Value2);
	void OnEquip();
	void Reload();
	void Fire();

	


};
