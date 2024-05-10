// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InGameUI.h"
#include "InputAction.h"
#include "Interactable.h"
#include "PauseMenu.h"
#include "UsePromptUI.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BeatEmUpCharacter.generated.h"

class AEnemySpawner;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABeatEmUpCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ABeatEmUpCharacter();

	UPROPERTY(EditAnywhere)
    int MaxHealth = 100;
	
    UPROPERTY(EditAnywhere)
    int CurrentHealth = MaxHealth;
	
    UPROPERTY(EditAnywhere)
    UInputAction* PunchAction;
	
    UPROPERTY(EditAnywhere)
    float PunchDistance = 250;
	
    UPROPERTY(EditAnywhere)
    float PunchForce = 50000;
	
    UPROPERTY(EditAnywhere)
    float PunchDamage = 50;
	
    UPROPERTY(EditAnywhere)
    float PunchCooldown = 1.f;
	
	bool bPunchReady = true;
	
    FTimerHandle PunchCooldownTimerHandle;

	UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* UseAction;
	
    UPROPERTY(EditAnywhere)
    float UseDistance = 1000;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUsePromptUI> UsePromptClass;

	UUsePromptUI* UsePrompt = nullptr;
	
	AActor* UseTarget;
	
	UPROPERTY(EditAnywhere)
    TSubclassOf<UInGameUI> InGameUIClass;
	
    UPROPERTY()
    UInGameUI* InGameUI;
	
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ThrusterChargeAction;

	UPROPERTY(EditAnywhere)
	uint32 ThrusterForceScalar = 350000;

	UPROPERTY(EditAnywhere)
	int ThrusterFuelLimit = 5000;
	
	UPROPERTY(EditAnywhere)
	float FuelConsumptionRate = 30.f;

	UPROPERTY(EditAnywhere)
	float RefuelRate = 1000.f;

	UPROPERTY(EditAnywhere)
	float ThrusterChargeDamage = 30.f;

	UPROPERTY(EditAnywhere)
	uint32 ThrusterKnockBackScalar = 100000;

	UPROPERTY(EditAnywhere)
	float ThrusterFuelDepletedPauseTime = 8.f;
	
	UPROPERTY(EditAnywhere)
	float ThrusterFuelRefillPauseTime = 1.5f;
	
	float ThrusterFuel = ThrusterFuelLimit;

	bool bThrustersReady = true;

	bool bThrustersEngaged = false;

	bool bThrusterFuelDepleted = false;

	bool bThrustersRefuelling = false;
	
	FTimerHandle ThrusterRefuelTimerHandle;

	UPROPERTY(EditAnywhere)
    int CurrentEXP = 0;
	
    UPROPERTY(EditAnywhere)
    int EXPToLevel = 50;
	
    UPROPERTY(EditAnywhere)
    float IncreaseMultiplier = 1.5;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemySpawner> EnemySpawnerClass;
	
	UPROPERTY(EditAnywhere)
    UInputAction* PauseAction;
	
    UPROPERTY(EditAnywhere)
    TSubclassOf<UPauseMenu> PauseMenuClass;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
    virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION()
    void Punch();
	
    UFUNCTION()
    void ResetPunch();
	
    void DealDamage(float Damage);
	
    UFUNCTION()
    void Use();

	UFUNCTION()
	void UseThrusters(const FInputActionInstance& Instance);

	UFUNCTION()
	void DisengageThrusters();	

	UFUNCTION()
	void UnpauseThrusterRefuel();

	void FindUseTarget();

	void AddEXP(int EXPToAdd);

	UFUNCTION()
    void PauseGame();
};

