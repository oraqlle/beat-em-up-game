// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"

AEnemyController::AEnemyController()
{
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	NavigationSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	PossessedEnemy = Cast<AEnemy>(InPawn);
}

void AEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TimeInState += DeltaSeconds;
	
    switch (CurrentState)
    {
	case IDLE:
		OnIdleTick(DeltaSeconds);
		break;
	case PATROL:
		OnPatrolTick(DeltaSeconds);
		break;
	case CHASE:
		OnChaseTick(DeltaSeconds);
		break;
    }
}

FRotator AEnemyController::GetControlRotation() const
{
	if (GetPawn())
    {
		return FRotator(0, GetPawn()->GetActorRotation().Yaw, 0);
    }
	
	return Super::GetControlRotation();
}

void AEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (CurrentState == PATROL && PossessedEnemy)
    {
		NavigationSystem->GetRandomPointInNavigableRadius(
			PossessedEnemy->GetActorLocation(),
			PatrolRange,
			PatrolTarget);
		
		MoveToLocation(PatrolTarget);
    }
}

void AEnemyController::ChangeState(EnemyState NewState)
{
	TimeInState = 0;
	
    switch (CurrentState)
    {
    case IDLE:
		OnIdleExit();
		break;
    case PATROL:
		OnPatrolExit();
		break;
    case CHASE:
		OnChaseExit();
		break;
    }
	
	switch (NewState)
    {
    case IDLE:
		OnIdleEnter();
		break;
    case PATROL:
		OnPatrolEnter();
		break;
    case CHASE:
		OnChaseEnter();
		break;
    }
	
    CurrentState = NewState;
}

void AEnemyController::OnIdleEnter()
{
}

void AEnemyController::OnIdleTick(float DeltaTime)
{
	if (TimeInState > 1)
    {
		ChangeState(PATROL);
    }
}

void AEnemyController::OnIdleExit()
{
}

void AEnemyController::OnPatrolEnter()
{
	if (PossessedEnemy)
    {
		NavigationSystem->GetRandomPointInNavigableRadius(
			PossessedEnemy->GetActorLocation(),
			PatrolRange,
			PatrolTarget);
		
		MoveToLocation(PatrolTarget);
    }
}

void AEnemyController::OnPatrolTick(float DeltaTime)
{
	if(PossessedEnemy)
    {
		if(FVector::Dist(PossessedEnemy->Player->GetActorLocation(), PossessedEnemy->GetActorLocation()) < DetectionRange)
		{
			ChangeState(CHASE);
			return;
		}
    }
}

void AEnemyController::OnPatrolExit()
{
}

void AEnemyController::OnChaseEnter()
{
}

void AEnemyController::OnChaseTick(float DeltaTime)
{
	if(PossessedEnemy)
    {
		FVector TowardsPlayer = PossessedEnemy->Player->GetActorLocation() - PossessedEnemy->GetActorLocation();
		
		if (TowardsPlayer.Size() <= DetectionRange)
		{
			MoveToLocation(PossessedEnemy->Player->GetActorLocation());
		} else {
			StopMovement();
			ChangeState(PATROL);
		}
    }
}

void AEnemyController::OnChaseExit()
{
}
