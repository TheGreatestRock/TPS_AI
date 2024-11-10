#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UBTTask_IdentifyEnemy.generated.h" // This must be the first include

UCLASS()
class TP3SHOOT_API UBTTask_IdentifyEnemy : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_IdentifyEnemy();

protected:
    // Make sure the signature matches the base class exactly
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

    // Blackboard key for the target location
    UPROPERTY(EditAnywhere, Category = "Blackboard")
    FBlackboardKeySelector TargetLocationKey;
};
