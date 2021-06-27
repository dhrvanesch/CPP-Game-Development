
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "SCharacter.generated.h"
/*
Spacer
*/
UCLASS()
class TST_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

	FTimerHandle TimerHandle_TraceInteractable;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;
		
		UPROPERTY(BlueprintReadOnly, Category = Interactable)
		bool bShowPrompt;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera)
			float InteractionRange;

		//Death
		UPROPERTY(ReplicatedUsing = OnRep_IsDead)
			bool bIsDead;

		UFUNCTION()
			void OnRep_IsDead();


		/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
			float BaseTurnRate;
		/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
			float BaseLookUpRate;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
			float BaseSpeed;

		protected:
		UPROPERTY(EditAnywhere)
			TSubclassOf<class AWeaponBase> WeaponClass;
		
		UPROPERTY(ReplicatedUsing = OnRep_AttachWeapon)
			class AWeaponBase* Weapon;
	
			
public:
	class UPlayerStatsComponent* PlayerStatsComp;
	class UInteractorComponent* InteractorComp;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Heal(float HealAmount);

	void DestroyCharacter();

protected:	// Called when the game starts or when spawned
	
	virtual void BeginPlay() override;
	
	// Interact
	void TraceInteractable();
	void Interact();
	// Movement
	void MoveForward(float Value);
	void Strafe(float Value);
	void BeginCrouch();
	void EndCrouch();
	void Pitch(float Value);
	void PitchRange();
	void TurnRate(float Rate);
	void PitchRate(float Rate);

	// Weapon  
	UFUNCTION()
	void OnRep_AttachWeapon();
	UFUNCTION()
	void OnRep_DropWeapon();

protected:
	void OnFire();
	void Reload();

	void Tick(float DeltaSeconds);




public:
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Interact();
	bool Server_Interact_Validate();
	void Server_Interact_Implementation();

	

public:	// Called to bind functionality to input
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:	//Get Camera Component
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
