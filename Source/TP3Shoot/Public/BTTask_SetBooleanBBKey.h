// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTask_SetBooleanBBKey.generated.h"

/**
 * Task Node that sets a Boolean Blackboard Key to a specified value.
 */
UCLASS()
class TP3SHOOT_API UBTTask_SetBooleanBBKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Constructor
	UBTTask_SetBooleanBBKey();

protected:
	// Execute Task function to set the Blackboard Key
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// The key selector for the boolean Blackboard key to modify
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BooleanKey;

	// The boolean value to set
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	bool bValueToSet;
};
