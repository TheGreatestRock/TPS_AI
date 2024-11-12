// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h" // inclure Blackboard
#include "BTTask_Booleen.generated.h"

UCLASS()
class TP3SHOOT_API UBTTask_Booleen : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Booleen();

protected:
	// Override de la fonction ExecuteTask pour d�finir notre logique
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// S�lecteur de cl� Blackboard pour le bool�en
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector BoolKey;
};
