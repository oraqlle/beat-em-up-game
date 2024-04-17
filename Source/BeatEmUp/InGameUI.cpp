// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameUI.h"
#include "BeatEmUpCharacter.h"

void UInGameUI::UpdateValues()
{
	HealthBar->SetPercent(static_cast<float>(Player->CurrentHealth)/Player->MaxHealth);
    CurrentHealthText->SetText(FText::FromString(FString::FromInt(Player->CurrentHealth)));
    float PunchTimePercentage = 0;
	
    if(GetWorld()->GetTimerManager().TimerExists(Player->PunchCooldownTimerHandle))
    {
		PunchTimePercentage = GetWorld()
    		->GetTimerManager()
    		.GetTimerRemaining(Player->PunchCooldownTimerHandle) / Player->PunchCooldown;
    }
	
    PunchCooldown->SetPercent(1 - PunchTimePercentage);
}
