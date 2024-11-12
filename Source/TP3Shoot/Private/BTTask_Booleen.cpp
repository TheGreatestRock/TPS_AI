// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Booleen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_Booleen::UBTTask_Booleen()
{
	// Initialiser le nom de la tâche
	NodeName = "Set Bool to False";
}

EBTNodeResult::Type UBTTask_Booleen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Vérifier si le Blackboard et la clé sont valides
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// Récupérer la clé booléenne et la définir sur false
	BlackboardComp->SetValueAsBool(BoolKey.SelectedKeyName, false);

	// Retourner le succès de l'exécution
	return EBTNodeResult::Succeeded;
}
