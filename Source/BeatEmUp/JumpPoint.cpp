// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPoint.h"

#include "NavLinkCustomComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/NavLinkProxy.h"

void AJumpPoint::BeginPlay()
{
	Super::BeginPlay();

	OnSmartLinkReached.AddDynamic(this, &AJumpPoint::LinkReached);
	CopyLinkPoints();
}

void AJumpPoint::LinkReached(AActor* Agent, const FVector& Destination)
{
	if (ACharacter* Enemy = (Cast<ACharacter>(Agent)))
    {
		FVector LaunchVelocity;
		UGameplayStatics::SuggestProjectileVelocity_CustomArc(
			GetWorld(),
			LaunchVelocity,
			Agent->GetActorLocation(),
			Destination);
		
		Enemy->LaunchCharacter(LaunchVelocity * 1.1, true, true);
    }
}

void AJumpPoint::CopyLinkPoints()
{
	if (PointLinks.Num() && GetSmartLinkComp()) {
		GetSmartLinkComp()->Modify();
		GetSmartLinkComp()->SetLinkData(
			PointLinks[0].Left,
			PointLinks[0].Right,
			PointLinks[0].Direction);
	}
}
