#include "PlayerStatsComponent.h"
#include "Engine/Engine.h"
#include "TST/UI/HUD/GameHUD.h"
#include "TST/Player/SPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"


UPlayerStatsComponent::UPlayerStatsComponent()
{
	Health = 100.0f;
	MaxHealth = 100.0f;
}

void UPlayerStatsComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME_CONDITION(UPlayerStatsComponent, Health, COND_OwnerOnly);
}

// Called when the game starts
void UPlayerStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	SetIsReplicated(true);
}

bool UPlayerStatsComponent::Server_DecrementHealth_Validate(float Value)
{
	return true;
}

void UPlayerStatsComponent::Server_DecrementHealth_Implementation(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		DecrementHealth(Value);
	}
}

bool UPlayerStatsComponent::Server_AddHealth_Validate(float Value)
{
	return true;
}

void UPlayerStatsComponent::Server_AddHealth_Implementation(float Value)
{
	UE_LOG(LogTemp, Warning, (TEXT("Server AddHealth Called")));
	if (GetOwnerRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, (TEXT("Authority")));
		AddHealth(Value);
	}
}

float UPlayerStatsComponent::GetHealth()
{
	return Health;
}

float UPlayerStatsComponent::GetMaxHealth()
{
	return MaxHealth;
}

void UPlayerStatsComponent::AddHealth(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (Health + Value > 100.0f)
		{
			Health = 100.0f;
		}
		else
		{
			Health += Value;
			UE_LOG(LogTemp, Warning, (TEXT("Healed by Server")));
		}
	}
	else
	{
		Server_AddHealth(Value);
	}
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			ASPlayerController* PlayerController = Cast<ASPlayerController>(*It);
			if (PlayerController != nullptr)
			{
			AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());
				
			if (GameHUD != nullptr)
			{
				GameHUD->UpdateHealth(GetHealth() / 100);
			}
		}
	}
}

void UPlayerStatsComponent::DecrementHealth(float Value)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (Health < 0.0f)
		{
		Health = 0.0f;
		}
		else
		{		
		Health -= Value;
		UE_LOG(LogTemp, Warning, (TEXT("Server Lowered Health")));
		}
	}
	else
	{
		Server_DecrementHealth(Value);
	}
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ASPlayerController* PlayerController = Cast<ASPlayerController>(*It);
		if (PlayerController != nullptr)
		{

			AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD());

			if (GameHUD != nullptr)
			{
				
				GameHUD->UpdateHealth(GetHealth() / 100);
			}
		}
	}
}
