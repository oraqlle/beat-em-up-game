// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "BouncePad.generated.h"

class UBoxComponent;

UCLASS()
class BEATEMUP_API ABouncePad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABouncePad();

	UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Base;
	
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Pad;
	
    UPROPERTY(EditAnywhere)
    UPhysicsConstraintComponent* Spring;

	UPROPERTY(EditAnywhere)
    UBoxComponent* SpringReleaseTrigger;

	float CompressedTargetZPosition;
	
	float SpringResetTime = 1.0;
	
	FTimerHandle SpringResetTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpringReset();

	UFUNCTION()
	void OnPadCollision(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);
};
