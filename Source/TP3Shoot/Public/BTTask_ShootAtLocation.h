// BTTTask_ShootAtLocation.h

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ShootAtLocation.generated.h"

UCLASS()
class UBTTask_ShootAtLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ShootAtLocation();

	// Executes the task, initiates the shooting logic
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// The key in the blackboard to get the target location
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector TargetLocationKey;
};