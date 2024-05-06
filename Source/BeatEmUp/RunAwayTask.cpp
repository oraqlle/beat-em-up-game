// Fill out your copyright notice in the Description page of Project Settings.


#include "RunAwayTask.h"

#include "EnemyBTController.h"

EBTNodeResult::Type URunAwayTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBehaviorTreeComponent* BTComp = &OwnerComp;
    
    if (!BTComp)
    {
        return EBTNodeResult::Failed;
    }
    
    AEnemyBTController* BTController = Cast<AEnemyBTController>(BTComp->GetOwner());
    
    if (!BTController)
    {
        return EBTNodeResult::Failed;
    }
    
    BTController->GenerateRunAwayLocation();
    
    return EBTNodeResult::Succeeded;
}
