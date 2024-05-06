// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatEmUpGameMode.h"
#include "BeatEmUpCharacter.h"
#include "BeatEmUpGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ABeatEmUpGameMode::ABeatEmUpGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABeatEmUpGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
    GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ABeatEmUpGameMode::PostBeginPlay);
}

void ABeatEmUpGameMode::Load(UBeatEmUpSaveGame* LoadedGame)
{
	ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
    PlayerCharacter->SetActorLocationAndRotation(LoadedGame->PlayerPosition, LoadedGame->PlayerRotation);
    PlayerCharacter->PunchDamage = LoadedGame->PlayerPunchDamage;
    PlayerCharacter->CurrentHealth = LoadedGame->PlayerCurrentHealth;
    PlayerCharacter->MaxHealth = LoadedGame->PlayerMaxHealth;
    PlayerCharacter->CurrentEXP = LoadedGame->PlayerCurrentEXP;
    PlayerCharacter->EXPToLevel = LoadedGame->PlayerEXPToLevel;
    PlayerCharacter->InGameUI->UpdateValues();
	
    for (int i = 0; i < LoadedGame->EnemyLocations.Num(); i++)
    {
		AEnemy* SpawnedEnemy = Cast<AEnemy>( GetWorld()->SpawnActor(EnemyClass));
    	
		if (SpawnedEnemy)
		{
			SpawnedEnemy->SetActorLocationAndRotation(LoadedGame->EnemyLocations[i], LoadedGame->EnemyRotations[i]);
			SpawnedEnemy->CurrentHealth = LoadedGame->EnemyCurrentHealths[i];
			SpawnedEnemy->MaxHealth = LoadedGame->EnemyMaxHealths[i];
			
			if (LoadedGame->EnemyRagdollStates[i])
			{
				SpawnedEnemy->Ragdoll();
				SpawnedEnemy->GetCharacterMovement()->GravityScale = 0;
				SpawnedEnemy->GetMesh()->SetWorldLocation(LoadedGame->EnemyMeshLocations[i], false, nullptr, ETeleportType::TeleportPhysics);
				SpawnedEnemy->GetMesh()->SetAllPhysicsLinearVelocity(LoadedGame->EnemyMeshVelocities[i], true);
			}
		}
    }
}

void ABeatEmUpGameMode::Save(UBeatEmUpSaveGame* SaveGame)
{
	ABeatEmUpCharacter* PlayerCharacter = Cast<ABeatEmUpCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
    SaveGame->PlayerPosition = PlayerCharacter->GetActorLocation();
    SaveGame->PlayerRotation = PlayerCharacter->GetActorRotation();
    SaveGame->PlayerCurrentHealth = PlayerCharacter->CurrentHealth;
    SaveGame->PlayerMaxHealth = PlayerCharacter->MaxHealth;
    SaveGame->PlayerPunchDamage = PlayerCharacter->PunchDamage;
    SaveGame->PlayerCurrentEXP = PlayerCharacter->CurrentEXP;
    SaveGame->PlayerEXPToLevel = PlayerCharacter->EXPToLevel;
	
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	
    for (int i = 0; i < Enemies.Num(); i++)
    {
		AEnemy* CurrentEnemy = Cast<AEnemy>(Enemies[i]);
    	
		SaveGame->EnemyLocations.Add(CurrentEnemy->GetActorLocation());
		SaveGame->EnemyRotations.Add(CurrentEnemy->GetActorRotation());
		SaveGame->EnemyCurrentHealths.Add(CurrentEnemy->CurrentHealth);
		SaveGame->EnemyMaxHealths.Add(CurrentEnemy->MaxHealth);
    	
		if (CurrentEnemy->GetMesh()->GetCollisionProfileName() == "Ragdoll") {
			SaveGame->EnemyRagdollStates.Add(true);
		} else {
			SaveGame->EnemyRagdollStates.Add(false);
		}
    	
		SaveGame->EnemyMeshLocations.Add(CurrentEnemy->GetMesh()->GetComponentLocation());
		SaveGame->EnemyMeshVelocities.Add(CurrentEnemy->GetMesh()->GetComponentVelocity());
    }
}

void ABeatEmUpGameMode::PostBeginPlay()
{
	UBeatEmUpGameInstance* GameInstance = Cast<UBeatEmUpGameInstance>(GetGameInstance());
	
    if (GameInstance) {
		if (GameInstance->bLoadSave) {
			UBeatEmUpSaveGame* LoadedGame = Cast<UBeatEmUpSaveGame>(UGameplayStatics::LoadGameFromSlot("TestSave",0));
			if (LoadedGame) {
				Load(LoadedGame);
			}
		}
    }
}
