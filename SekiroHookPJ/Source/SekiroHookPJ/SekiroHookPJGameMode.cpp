// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SekiroHookPJGameMode.h"
#include "SekiroHookPJCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASekiroHookPJGameMode::ASekiroHookPJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Sekiro_Hook_PJ/Core/Character/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
