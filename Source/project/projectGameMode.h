// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "projectGameMode.generated.h"

UCLASS(minimalapi)
class AprojectGameMode : public AGameModeBase
{
	GENERATED_BODY() // Constructor (the class function which is automatically called when the an object of the class is created.
    // its primary function is to initialize the object - set up default values and allocate resources, and prepare it for use.

public:
	AprojectGameMode();
    
    // Function to gather and log all actor data
    void GetAllActorData();
    
    // Function to gather and log player-specific data
    void GetPlayerData();
    
protected:
    // beginplay is overriden when the functions above are called
    virtual void BeginPlay() override;
};



 
