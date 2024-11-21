// Copyright Epic Games, Inc. All Rights Reserved.

#include "projectGameMode.h"
#include "projectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

AprojectGameMode::AprojectGameMode()
    : Super() // super() is part of C++ constructor initialzation, specifically for calling the constructor of the parent (or base) class.
    // in this scenario Super() refers to AGameModeBase
{
    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AprojectGameMode::BeginPlay()
{
    Super::BeginPlay();

    // collect all actor data
    GetAllActorData();

    // collect player-specific data
    GetPlayerData();
}

void AprojectGameMode::GetAllActorData()
{
    //UWorld is UE's class representing the game world. A pointer is
    // used becuase GetWorld() returns a pointer to the current world object
    //Get World() ==> gets the current game world where all actors exist
    UWorld* World = GetWorld();

    /*
     if World is null or not valid, the function is stopped
     */
    if (!World)
    {
        // LogTemp is a temporary log category for general messages
        // why warning: to make the log more clear
        UE_LOG(LogTemp, Warning, TEXT("World context is invalid"));
        return; // exit function to prevent crash
    }

    /*
     iterate thorugh all actors in the world:
     - TActoriterator<AActor> is a UE iterator used to loop through
     all actors of a specified type. here its AActor
     AActor => The base class for all objects that can exist in the
     game world.

     ActorItr(World) => Initializes the iterator to work with the
     provided World. It will iterate through all actors in that world.

     Iteration is done in the for looop below:
     */
    for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
    {
        /* Below we use a pointer to the current actor being iterated.
         *ActorItr - dereferences the iterator to access the actual
         actor object
         */
        AActor* Actor = *ActorItr;

        /*Get actor name and location:

         ActorName: Retrieves the name of the actor as a Fstring
         ActorLocation: Returns the actors location in world space as an FVector (3d coordinate with x, y, z)
         ActorClass: Returns the UClass of the actor, which is its type (for ex, StaticMeshActor or PointLight)
         */
        FString ActorName = Actor->GetName();
        FVector ActorLocation = Actor->GetActorLocation();
        FString ActorClass = Actor->GetClass()->GetName();

        // log the actor data for test?
        UE_LOG(LogTemp, Log, TEXT("Actor Name: %s, CLass %s, Location %s"), *ActorName, *ActorClass, *ActorLocation.ToString());
    }
}

void AprojectGameMode::GetPlayerData()
{
    // Same as GetActors with some modifications
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("World context is invalid!"));
        return;
    }

    /*
     Difference between APlayerCotnroller and APawn:
     - APlayerController is responsible for handling player input
     and directing the player's actions. Its the brain that interprets input and sends commands to the controlled object which is the pawn here.
     - APawn : is the physical entity in the game world that the player can control. Its basically the body that exists in the world and carries out actions based on commands from hte PlayerController.
     */

     /* get the player controller (single player):

      PlayerCtronoller is a class responsible for handling player inout and controlling the player's pawn.
      World->GetFirstController() --> retrieves the first player controller in the world. This assumes a single-player environment.
     */
    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player controller not found"));
        return;
    }

    /* get the controller pawn:

     PlayerPawn refers to the body in the game (player).
     PlayerController->GetPawn() : gets the pawn
     */

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Playerpawn not found"));
        return;
    }

    /* Player specific data:

     PlayerName - the name of the player stored in FString. to identify the player
     PlayerLocation - the location of the player, stored in FVector
     PlayerClass - gets the pawn class. dont know if this is nessecary yet.

     */
    FString PlayerName = PlayerPawn->GetName();
    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FString PlayerClass = PlayerPawn->GetClass()->GetName();

    // Log the player data
    UE_LOG(LogTemp, Log, TEXT("Player Name: %s, Class: %s, Location: %s"),
        *PlayerName, *PlayerClass, *PlayerLocation.ToString());
}
