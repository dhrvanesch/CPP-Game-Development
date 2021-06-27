// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUDWidget.h"

UGameHUDWidget::UGameHUDWidget(const FObjectInitializer& ObjectInitalizer) : Super(ObjectInitalizer)
{

}

void UGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameHUDWidget::UpdateAmmo(int32 Value1, int32 Value2)
{
	if (TB_AmmoText)
	{
		FString CurrentAmmoText = FString::FromInt(Value1);
		FString AmmoRemainderText = FString::FromInt(Value2);
		FString AmmoText = CurrentAmmoText + " / " + AmmoRemainderText;
		TB_AmmoText->SetText(FText::FromString(AmmoText));
	}
}


void UGameHUDWidget::SetInteractionText(FString Value)
{
	if (TB_InteractionText)
	{
		TB_InteractionText->SetText(FText::FromString(Value));
	}
}

void UGameHUDWidget::UpdateGameTimer(FString Value)
{
	TB_GameTimer->SetText(FText::FromString(Value));
}

void UGameHUDWidget::UpdateHealth(float Value)
{

	PB_HealthBar->SetPercent(Value);
}

