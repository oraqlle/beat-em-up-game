// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bomb.generated.h"

UCLASS()
class BEATEMUP_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* MovementComponent;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	float MovementSpeed = 1000;

	UPROPERTY(EditAnywhere)
	float Damage = 7;

	UPROPERTY(EditAnywhere)
	float DamageRadius = 1000;

	UPROPERTY(EditAnywhere)
	float KnockBackForce;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExplosionEffectClass;
	
	UPROPERTY(EditAnywhere)
	float FuseTime = 2.7f;

	FTimerHandle FuseTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Explode();
};
