// Copyright Epic Games, Inc. All Rights Reserved.

#include "CatchCashGameMode.h"
#include "CatchCashCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACatchCashGameMode::ACatchCashGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
