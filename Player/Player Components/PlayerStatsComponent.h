#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "PlayerStatsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TST_API UPlayerStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPlayerStatsComponent();

protected:

	UPROPERTY(Replicated)
	float Health;

	UPROPERTY(EditAnywhere)
		float MaxHealth;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_DecrementHealth(float Value);
	bool Server_DecrementHealth_Validate(float Value);
	void Server_DecrementHealth_Implementation(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddHealth(float Value);
	bool Server_AddHealth_Validate(float Value);
	void Server_AddHealth_Implementation(float Value);

public:
	float GetHealth();
	float GetMaxHealth();
	void AddHealth(float Value);
	void DecrementHealth(float Value);
};
