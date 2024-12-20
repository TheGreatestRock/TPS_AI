// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP3ShootGameMode.h"
#include "TP3ShootCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATP3ShootGameMode::ATP3ShootGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprint/BP_TP3ShootPlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}