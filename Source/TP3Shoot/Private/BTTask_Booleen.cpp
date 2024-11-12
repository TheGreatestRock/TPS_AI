// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Booleen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_Booleen::UBTTask_Booleen()
{
	// Initialiser le nom de la t�che
	NodeName = "Set Bool to False";
}

EBTNodeResult::Type UBTTask_Booleen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// V�rifier si le Blackboard et la cl� sont valides
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// R�cup�rer la cl� bool�enne et la d�finir sur false
	BlackboardComp->SetValueAsBool(BoolKey.SelectedKeyName, false);

	// Retourner le succ�s de l'ex�cution
	return EBTNodeResult::Succeeded;
}
