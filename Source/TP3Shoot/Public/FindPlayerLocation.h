// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FindPlayerLocation.generated.h"

/**
 * Custom BT Task Node to find player location and set it in the blackboard.
 */
UCLASS()
class TP3SHOOT_API UFindPlayerLocation : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UFindPlayerLocation();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector EnemyCharacter;  // The enemy character key in the blackboard.

    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector TargetLocationKey;  // The key for the target location in the blackboard.
};
