#include "TST\Player\SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/InputSettings.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "TST/Interface/ShowInteract.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "TST/Player/PlayerStatsComponent.h"
#include "TST/Components/InteractorComponent.h" 
#include "TST/Interface/Interactable.h"
#include <TST/Weapons/WeaponBase.h>
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include <TST/UI/HUD/GameHUD.h>
#include <TST/Player/SPlayerController.h>




// Sets default values
ASCharacter::ASCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(30.f, 95.f);
	/*GetCharacterMovement()->MaxWalkSpeed = 400.0f;*/
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent -> SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent -> SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));
	FirstPersonCameraComponent -> bUsePawnControlRotation = true;
	FirstPersonCameraComponent -> SetIsReplicated(true);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(0.0f, 0.0f, -38.0f));
	Mesh1P->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->CastShadow = true;
	PlayerStatsComp = CreateDefaultSubobject<UPlayerStatsComponent>("PlayerStatComponent");
	InteractorComp = CreateDefaultSubobject<UInteractorComponent>("InteractComponent");
	
	InteractionRange = 200.0f;
	bShowPrompt = false;

	bIsDead = false;
	//CurrentWeapon = 0;
	
}
// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	//Movement Bindings
	PlayerInputComponent->BindAxis("Move Forward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Strafe", this, &ASCharacter::Strafe);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAxis("Pitch", this, &ASCharacter::Pitch);
	PlayerInputComponent->BindAxis("PitchRate", this, &ASCharacter::PitchRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASCharacter::TurnRate);

	//Weapon Bindings
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::OnFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::Reload);
	PlayerInputComponent->BindAction("Drop Weapon", IE_Pressed, this, &ASCharacter::OnRep_DropWeapon);

	//Interact Binding
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASCharacter::Interact);

}


void ASCharacter::TraceInteractable()
{
	FVector StartLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector Angle = GetControlRotation().Vector();
	FVector EndLocation = StartLocation + Angle * InteractionRange;


	if (AActor* HitActor = InteractorComp->LineTraceSingle(StartLocation, EndLocation).GetActor())
	{
		if (IShowInteract* Interface = Cast<IShowInteract>(HitActor))
		{
			bShowPrompt = true;

			AGameHUD* GameHUD = Cast<AGameHUD>(UGameplayStatics::GetPlayerController(this->GetOwner(), 0)->GetHUD());

			if (GameHUD != nullptr)
			{
				FString ActorText = Interface->GetText();
				GameHUD->UpdateInteractPrompt(ActorText);
			}
		}
		else if (bShowPrompt)
		{
			bShowPrompt = false;
			AGameHUD* GameHUD = Cast<AGameHUD>(UGameplayStatics::GetPlayerController(this->GetOwner(), 0)->GetHUD());

			if (GameHUD != nullptr)
			{
				GameHUD->UpdateInteractPrompt("");
			}
		}
	}
}

void ASCharacter::Interact()
{
	
    if (!HasAuthority())
    {
        Server_Interact();
        return;
    }
    FVector StartLocation = GetFirstPersonCameraComponent()->GetComponentLocation();
    FVector Angle = GetControlRotation().Vector();
    FVector EndLocation = StartLocation + Angle * InteractionRange;
	//Check for component hit, if hit RETRURN
	if (USceneComponent* HitComp = InteractorComp->LineTraceSingle(StartLocation, EndLocation).GetComponent())
	{

		if (IInteractable* Interactable = Cast<IInteractable>(HitComp))
		{
			Interactable->Use();
			return;
		}
	}
	//Check for Actor Hit
     if (AActor* HitActor = InteractorComp->LineTraceSingle(StartLocation, EndLocation).GetActor())
    {
		
		if (IInteractable* Interactable = Cast<IInteractable>(HitActor))
		{
			Interactable->Use();
		}
		else if (AWeaponBase* HitWeapon = Cast<AWeaponBase>(HitActor))
		{
			if (HitWeapon)
			{
				if (!Weapon)
				{
					Weapon = HitWeapon;
					Weapon->DisableComponentsSimulatePhysics();
					Weapon->SetOwner(this);
					OnRep_AttachWeapon();
				}
			}
		}
    } 
		
}




// Server Interact
void ASCharacter::Server_Interact_Implementation()
{
	Interact();
}

bool ASCharacter::Server_Interact_Validate()
{
	return true;
}

//MOVEMENT
void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::Strafe(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::Pitch(float Value)
{
	if (Value != 0.f && Controller && Controller->IsLocalPlayerController())
	{
		APlayerController* const PC = CastChecked<APlayerController>(Controller);
		PC->AddPitchInput(Value);
	}
}

void ASCharacter::PitchRange()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->ViewPitchMin = -60.0;
			PlayerController->PlayerCameraManager->ViewPitchMax = 60.0;
		}
	}
}

void ASCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASCharacter::PitchRate(float Rate)
{

	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ASCharacter::OnRep_AttachWeapon()
{
	if (Weapon)
	{
		Weapon->OnEquip();
		Weapon->SetActorEnableCollision(false);
		if (IsLocallyControlled())//remove true
		{
			
				Weapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket_WeaponGrip"));
		}
		else
		{
			
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Socket_WeaponGrip"));
		}
	}
}

void ASCharacter::OnRep_DropWeapon()
{
	if (Weapon)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->SetActorEnableCollision(true);
		Weapon = nullptr;
	}
	
}

void ASCharacter::OnFire()
{
	if (Weapon)
	{
		Weapon->Fire();
	} 
}

void ASCharacter::Reload()
{
	if (Weapon)
	{
		Weapon->Reload();
	}
	
}


float ASCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Role < ROLE_Authority || bIsDead)

		return 0.0f;

	if (ActualDamage > 0.0f)
	{
		PlayerStatsComp->DecrementHealth(ActualDamage);
		UE_LOG(LogTemp, Warning, (TEXT("Shot!")));
		if (PlayerStatsComp->GetHealth() <= 0.0f)
		{
			bIsDead = true;
			OnRep_IsDead();
			UE_LOG(LogTemp, Warning, (TEXT("You Died")));
		}
	}
	return ActualDamage;
}

void ASCharacter::Heal(float HealAmount)
{
	UE_LOG(LogTemp, Warning, (TEXT("Heal Called")));
	if (PlayerStatsComp->GetHealth() == PlayerStatsComp->GetMaxHealth())
	{
		return;
	}
	else
	{
		PlayerStatsComp->AddHealth(HealAmount);
	}
}

void ASCharacter::OnRep_IsDead()
{
	if (HasAuthority())
	{
		FTimerHandle DestroyHandle;
		GetWorldTimerManager().SetTimer(DestroyHandle, this, &ASCharacter::DestroyCharacter, 1.0f, false);
	}

}


void ASCharacter::DestroyCharacter()
{
	Destroy();
}

void ASCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

				
void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, Weapon);
	DOREPLIFETIME(ASCharacter, bIsDead);
}


// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		PrimaryActorTick.SetTickFunctionEnable(true);
	}
	else
	{
		PrimaryActorTick.SetTickFunctionEnable(false);
	}
	PitchRange();


	GetWorld()->GetTimerManager().SetTimer(TimerHandle_TraceInteractable, this, &ASCharacter::TraceInteractable, 0.9f, true);

}


