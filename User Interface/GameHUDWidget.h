// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "UObject/UObjectGlobals.h"
#include "GameHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class TST_API UGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UGameHUDWidget(const FObjectInitializer& ObjectInitializer);
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TB_GameTimer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TB_InteractionText;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TB_AmmoText;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* PB_HealthBar;
	




	virtual void NativeConstruct() override;

	void UpdateAmmo(int32 Value1, int32 Value2);

	void UpdateGameTimer(FString Value);

	void UpdateHealth(float Value);

	void SetInteractionText(FString Value);


};
