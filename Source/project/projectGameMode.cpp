// Copyright Epic Games, Inc. All Rights Reserved.

#include "projectGameMode.h"
#include "projectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AprojectGameMode::AprojectGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
