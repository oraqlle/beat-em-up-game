// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBTController.h"

#include "Enemy.h"

AEnemyBTController::AEnemyBTController()
{
	PrimaryActorTick.bCanEverTick = true;
    SightConfiguration = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Configuration"));
    SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
	
    SightConfiguration->SightRadius = SightRadius;
    SightConfiguration->LoseSightRadius = LoseSightRadius;
    SightConfiguration->PeripheralVisionAngleDegrees = FieldOfView;
    SightConfiguration->SetMaxAge(SightAge);
	
    SightConfiguration->DetectionByAffiliation.bDetectEnemies = true;
    SightConfiguration->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfiguration->DetectionByAffiliation.bDetectNeutrals = true;
	
    GetPerceptionComponent()->SetDominantSense(*SightConfiguration->GetSenseImplementation());
    GetPerceptionComponent()->ConfigureSense(*SightConfiguration);
}

void AEnemyBTController::BeginPlay()
{
	Super::BeginPlay();

	NavigationSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	
    UseBlackboard(AIBlackboard, BlackboardComponent);
    RunBehaviorTree(BehaviourTree);
    
    GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBTController::OnSensesUpdated);
	BlackboardComponent->SetValueAsBool("HasAmmo", true);
	// BlackboardComponent->SetValueAsBool("RunAway", true);
}

void AEnemyBTController::Tick(float DeltaSeconds)
{	
	Super::Tick(DeltaSeconds);
	
	if (TargetPlayer)
    {
		BlackboardComponent->SetValueAsVector("PlayerPosition", TargetPlayer->GetActorLocation());

		const ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(TargetPlayer);
		BlackboardComponent->SetValueAsBool("RunAway", Player->bThrustersEngaged);
    }
}

FRotator AEnemyBTController::GetControlRotation() const
{
	if (GetPawn())
	{
		return FRotator(0, GetPawn()->GetActorRotation().Yaw,0);
	}
	
	return Super::GetControlRotation();
}

void AEnemyBTController::GenerateNewRandomLocation()
{
	if (NavigationSystem)
    {
		FNavLocation ReturnLocation;
		
		NavigationSystem->GetRandomReachablePointInRadius(
			GetPawn()->GetActorLocation(),
			PatrolDistance,
			ReturnLocation);

		
		BlackboardComponent->SetValueAsVector("PatrolPoint", ReturnLocation.Location);
    }
}

void AEnemyBTController::OnSensesUpdated(AActor* UpdatedActor, FAIStimulus Stimulus)
{
	if (APawn* SensedPawn = Cast<APawn>(UpdatedActor))
    {
		if (SensedPawn->IsPlayerControlled())
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				TargetPlayer = SensedPawn;
				const ABeatEmUpCharacter* Player = Cast<ABeatEmUpCharacter>(TargetPlayer);
			
				BlackboardComponent->SetValueAsBool("RunAway", Player->bThrustersEngaged);
				BlackboardComponent->SetValueAsBool("ChasePlayer", !Player->bThrustersEngaged);
				// BlackboardComponent->SetValueAsBool("ChasePlayer", true);
				BlackboardComponent->SetValueAsVector(
					"PlayerPosition",
					TargetPlayer->GetActorLocation());
			} else {
				TargetPlayer = nullptr;
				BlackboardComponent->SetValueAsBool("ChasePlayer", false);
				BlackboardComponent->SetValueAsBool("RunAway", false);
			}
		}
    }
}

void AEnemyBTController::Shoot()
{
	const FVector ShootDirection = TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation();
    Cast<AEnemy>(GetPawn())->Shoot(ShootDirection);
    Ammo--;
    BlackboardComponent->SetValueAsBool("HasAmmo", Ammo > 0);
}

void AEnemyBTController::GenerateRunAwayLocation()
{
	if (NavigationSystem && TargetPlayer != nullptr) {
		FNavLocation NavLocation;

		const FVector PlayerLocation = TargetPlayer->GetActorLocation();
		const FVector EnemyLocation = GetPawn()->GetActorLocation();
		FVector PlayerEnemyVector = EnemyLocation - PlayerLocation;
		PlayerEnemyVector.Normalize();
		const float PlayerEnemyDist = PlayerEnemyVector.Size();
		FVector Origin =  PlayerEnemyVector * -PlayerEnemyDist;
		const float FleeDist = Origin.Size();
		
		NavigationSystem->GetRandomReachablePointInRadius(
			Origin,
			FleeDist,
			NavLocation);

		const FVector FleeLocation = NavLocation.Location;
		GetBlackboardComponent()->SetValueAsVector("RunAwayLocation", FleeLocation);

		// while (!(EnemyFleeDistance < PlayerFleeDistance)) {
		// 	const FVector PlayerLocation = TargetPlayer->GetActorLocation();
		// 	const FVector EnemyLocation = GetPawn()->GetActorLocation();
		// 	FVector PlayerEnemyVector = EnemyLocation - PlayerLocation;
		// 	PlayerEnemyVector.Normalize();
		// 	PlayerEnemyVector *= -20;
		// 	
		// 	NavigationSystem->GetRandomReachablePointInRadius(
		// 		PlayerEnemyVector,
		// 		PatrolDistance,
		// 		NavLocation);
		//
		// 	FleeLocation = NavLocation.Location;
		// 	PlayerFleeDistance = FVector::Distance(PlayerLocation, FleeLocation);
		// 	EnemyFleeDistance = FVector::Distance(EnemyLocation, FleeLocation);
		// }
		//
		// if (EnemyFleeDistance < PlayerFleeDistance) {
		// 	GetBlackboardComponent()->SetValueAsVector("RunAwayLocation", FleeLocation);	
		// }
	}
}
