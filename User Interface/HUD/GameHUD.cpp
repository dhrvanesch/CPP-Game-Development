// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "TST/Game/SGameState.h"




AGameHUD::AGameHUD() {

	
}

FString AGameHUD::GetTimeString(float TimeSeconds)
{
	// only minutes and seconds are relevant
	const int32 TotalSeconds = FMath::Max(0, FMath::TruncToInt(TimeSeconds) % 3600);
	const int32 NumMinutes = TotalSeconds / 60;
	const int32 NumSeconds = TotalSeconds % 60;

	const FString TimeDesc = FString::Printf(TEXT("%02d:%02d"), NumMinutes, NumSeconds);
	return TimeDesc;
}

void AGameHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	
	InitHUD();

}

void AGameHUD::Tick(float DeltaSeconds) 
{
	Super::Tick(DeltaSeconds);
}

void AGameHUD::InitHUD()
{
	if (GameHUDWidgetClass)
	{
		GameHUDWidget = CreateWidget<UGameHUDWidget>(GetWorld(), GameHUDWidgetClass);
	
		if (GameHUDWidget)
		{
			GameHUDWidget->AddToViewport();
		}
	}
}

void AGameHUD::UpdateGameTimer(int32 TimeSeconds)
{
	FString Text = GetTimeString(TimeSeconds);
	
	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateGameTimer(Text);
	}
}

void AGameHUD::UpdateAmmo(int32 Value1, int32 Value2)
{
	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateAmmo(Value1, Value2);
	}
}

void AGameHUD::UpdateInteractPrompt(FString Value)
{
	if (GameHUDWidget)
	{
		GameHUDWidget->SetInteractionText(Value);
	}
}

void AGameHUD::UpdateHealth(float Value)
{

	if (GameHUDWidget)
	{
		GameHUDWidget->UpdateHealth(Value);
	}
}
