// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadButton.h"

#include "BeatEmUpGameInstance.h"
#include "Kismet/GameplayStatics.h"

ULoadButton::ULoadButton()
{
	OnClicked.AddDynamic(this, &ULoadButton::OnButtonClick);
}

void ULoadButton::OnButtonClick()
{
	if (UGameplayStatics::DoesSaveGameExist("TestSave", 0))
    {
		UGameplayStatics::OpenLevel(GetWorld(), MapToOpen);
		UBeatEmUpGameInstance* GameInstance = Cast<UBeatEmUpGameInstance>(GetGameInstance());
		
		if (GameInstance)
		{
			GameInstance->bLoadSave = true;
		}
    }
}
