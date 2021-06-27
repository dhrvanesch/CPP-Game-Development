#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "TST/UI/GameHUDWidget.h"
#include "GameHUD.generated.h"
/*
 Spacer
 */
UCLASS()
class TST_API AGameHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGameHUD();

	
	FString GetTimeString(float TimeSeconds);

	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void InitHUD();

	UFUNCTION()
		void UpdateGameTimer(int32 TimeSeconds);

		void UpdateAmmo(int32 Value1, int32 Value2);

		void UpdateInteractPrompt(FString Value);

		void UpdateHealth(float Value);

	UPROPERTY(EditAnywhere, Category = "Widgets")
		TSubclassOf<UUserWidget> GameHUDWidgetClass;

	

private:

	UGameHUDWidget* GameHUDWidget;
};
