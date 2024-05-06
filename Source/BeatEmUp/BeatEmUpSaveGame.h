// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BeatEmUpSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UBeatEmUpSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
    FString SaveSlotName;
	
    UPROPERTY()
    uint32 UserIndex;
	
	UPROPERTY()
	FVector PlayerPosition;

	UPROPERTY()
    FRotator PlayerRotation;

	UPROPERTY()
    float PlayerCurrentHealth;

	UPROPERTY()
    float PlayerMaxHealth;

	UPROPERTY()
    float PlayerCurrentEXP;

	UPROPERTY()
    float PlayerEXPToLevel;

	UPROPERTY()
    float PlayerPunchDamage;

	UPROPERTY()
	TArray<FVector> EnemyLocations;

	UPROPERTY()
    TArray<FRotator> EnemyRotations;

	UPROPERTY()
    TArray<float> EnemyCurrentHealths;

	UPROPERTY()
    TArray<float> EnemyMaxHealths;

	UPROPERTY()
    TArray<bool> EnemyRagdollStates;

	UPROPERTY()
    TArray<FVector> EnemyMeshLocations;

	UPROPERTY()
    TArray<FVector> EnemyMeshVelocities;
};
