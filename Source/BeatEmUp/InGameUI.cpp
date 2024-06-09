// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"
#include "BeatEmUpCharacter.h"

void UInGameUI::UpdateValues()
{
	HealthBar->SetPercent(static_cast<float>(Player->CurrentHealth) / Player->MaxHealth);
    CurrentHealthText->SetText(FText::FromString(FString::FromInt(Player->CurrentHealth)));
	
    float PunchTimePercentage = 0;
	
    if (GetWorld()->GetTimerManager().TimerExists(Player->PunchCooldownTimerHandle)) {
		PunchTimePercentage = GetWorld()
    		->GetTimerManager()
    		.GetTimerRemaining(Player->PunchCooldownTimerHandle) / Player->PunchCooldown;
    }
	
    PunchCooldown->SetPercent(1 - PunchTimePercentage);

	// Updates the fuel bar according to how much fuel is left
	ThrusterFuelBar->SetPercent(Player->ThrusterFuel / Player->ThrusterFuelLimit);

	MaxHealthText->SetText(FText::FromString(FString::SanitizeFloat(Player->MaxHealth)));
	EXPBar->SetPercent(static_cast<float>(Player->CurrentEXP) / Player->EXPToLevel);

	float BombReloadPercentage = 0.f;
	if (GetWorld()->GetTimerManager().TimerExists(Player->BombReloadTimerHandle)) {
		BombReloadPercentage = GetWorld()
			->GetTimerManager()
			.GetTimerRemaining(Player->BombReloadTimerHandle) / Player->BombReloadTime;
	}

	BombReload->SetPercent(1 - BombReloadPercentage);
	BombCountText->SetText(FText::FromString(FString::Printf(TEXT("Bombs: %d"), Player->BombAmmo)));
}
