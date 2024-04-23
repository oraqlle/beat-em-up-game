// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPoint.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

void AJumpPoint::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &AJumpPoint::LinkReached);
	CopyEndPointsFromSimpleLinkToSmartLink();
}

void AJumpPoint::LinkReached(AActor* Agent, const FVector& Destination)
{
	if (ACharacter* Enemy = (Cast<ACharacter>(Agent)))
    {
		FVector LaunchVelocity;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			GetWorld(),
			LaunchVelocity,
			GetActorLocation(),
			Destination);
		
		Enemy->LaunchCharacter(LaunchVelocity * 1.1, true, true);
    }
}
