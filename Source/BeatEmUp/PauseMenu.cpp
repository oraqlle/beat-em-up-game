// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "BeatEmUpGameMode.h"
#include "BeatEmUpSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ContinueButton->OnClicked.AddDynamic(this, &UPauseMenu::ContinueButtonClick);
    SaveButton->OnClicked.AddDynamic(this, &UPauseMenu::SaveButtonClick);
    QuitButton->OnClicked.AddDynamic(this, &UPauseMenu::QuitButtonClick);
}

void UPauseMenu::ContinueButtonClick()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
    GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
    GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
    RemoveFromParent();
}

void UPauseMenu::SaveButtonClick()
{
	if (UBeatEmUpSaveGame* SaveGameInstance = Cast<UBeatEmUpSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UBeatEmUpSaveGame::StaticClass()))) {
		if (ABeatEmUpGameMode* BeatEmUpGameMode = Cast<ABeatEmUpGameMode>(GetWorld()->GetAuthGameMode())) {
			BeatEmUpGameMode->Save(SaveGameInstance);
			UGameplayStatics::SaveGameToSlot(SaveGameInstance,"Save", 0);
		}
    }
}

void UPauseMenu::QuitButtonClick()
{
	UKismetSystemLibrary::QuitGame(
		GetWorld(),
		GetWorld()->GetFirstPlayerController(),
		EQuitPreference::Quit,
		true);
}
