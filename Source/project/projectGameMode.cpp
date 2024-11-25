// Copyright Epic Games, Inc. All Rights Reserved.

#include "projectGameMode.h"
#include "projectCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h" 

/*
    Why I've written the json-converter in the GameMode script:
    - Here we can collect data about all actors and player in the game world.
    - We can easily use this data and convert it into a json-format
    - Easily log the json data to the UE console for testing

    JsonWriter.h - has FJsonObject (UE's own json object representation)
    JsonSerializer.h - provides functions for loading to json string
*/

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
    /*
        BeginPlay() - is called when the game starts or when this game mode is first activated.
        GetAllActordata() - function to gather data about all actors in the game world
        GetPlayerData() - also custom function to gather data bout the player
        GenerateJSON() - A customer function that collects the data and formats it as a JSON string
    */
    Super::BeginPlay();

    // Export to JSON
    FString JSONString = GenerateJSON();
    UE_LOG(LogTemp, Log, TEXT("Generated JSON: %s"), *JSONString); // log for testing and debugging
}

FString AprojectGameMode::GenerateJSON()
{
    /*
        Root JSON Object - creates the root object to hold all data
        Actors Data - Iterates through actors, creating a JSON object for each actor and adding it to an arrya
        Player data - Creates json object for the player and adds it to the root object
        Serialize to string - converts the json object into a string using UE's serializer

    */
    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject());
    TArray<TSharedPtr<FJsonValue>> ActorsArray;
    TArray<TSharedPtr<FJsonValue>> LightingArray;
    TArray<TSharedPtr<FJsonValue>> GameplayArray;

    // Actor Data
    UWorld* World = GetWorld();

    if (World)
    {
        for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
        {
            AActor* Actor = *ActorItr;

            /*
                FJsonObject - represents a JSON object in ue. can be used to create or parse json objects
                TSharedPtr<FJsonObject> - A shared pointer used for handling FJsonObject instances
            */

            // new json object for each actor
            TSharedPtr<FJsonObject> ActorObject = MakeShareable(new FJsonObject());

            // basic variables
            ActorObject->SetStringField(TEXT("Name"), Actor->GetName());
            ActorObject->SetStringField(TEXT("Class"), Actor->GetClass()->GetName());

            // location nested object (easier for LLM to understand than using string):
            TSharedPtr<FJsonObject> LocationObject = MakeShareable(new FJsonObject());
            FVector ActorLocation = Actor->GetActorLocation();
            LocationObject->SetNumberField(TEXT("X"), ActorLocation.X);
            LocationObject->SetNumberField(TEXT("Y"), ActorLocation.Y);
            LocationObject->SetNumberField(TEXT("Z"), ActorLocation.Z);
            ActorObject->SetObjectField(TEXT("Location"), LocationObject);

            TArray<TSharedPtr<FJsonValue>> TagsArray;

            // categorize / tags
            if (Actor->GetClass()->GetName() == TEXT("SkyLight"))
            {
                TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("Environment"))));
                TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("Lighting"))));
                LightingArray.Add(MakeShareable(new FJsonValueObject(ActorObject)));
            }
            else if (Actor->GetClass()->GetName() == TEXT("PlayerStart"))
            {
                TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("Gameplay"))));
                TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("Spawn"))));
                ActorObject->SetStringField(TEXT("Role"), TEXT("Spawn Point"));
                GameplayArray.Add(MakeShareable(new FJsonValueObject(ActorObject)));
            }
            else
            {
                TagsArray.Add(MakeShareable(new FJsonValueString(TEXT("Actor"))));
                ActorsArray.Add(MakeShareable(new FJsonValueObject(ActorObject)));
            }
            ActorObject->SetArrayField(TEXT("Tags"), TagsArray);
        }
        // add to rootobject
        RootObject->SetArrayField(TEXT("Actors"), ActorsArray);
        RootObject->SetArrayField(TEXT("Lighting"), LightingArray);
        RootObject->SetArrayField(TEXT("Gameplay"), GameplayArray);
    }

    // Player Data
    TSharedPtr<FJsonObject> PlayerObject = MakeShareable(new FJsonObject());
    APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn)
        {
            // Create a JSON object for the player
            PlayerObject->SetStringField(TEXT("Name"), PlayerPawn->GetName());
            PlayerObject->SetStringField(TEXT("Class"), PlayerPawn->GetClass()->GetName());

            // nested location object
            TSharedPtr<FJsonObject> PlayerLocationObject = MakeShareable(new FJsonObject());
            FVector PlayerLocation = PlayerPawn->GetActorLocation();
            PlayerLocationObject->SetNumberField(TEXT("X"), PlayerLocation.X);
            PlayerLocationObject->SetNumberField(TEXT("Y"), PlayerLocation.Y);
            PlayerLocationObject->SetNumberField(TEXT("Z"), PlayerLocation.Z);
            PlayerObject->SetObjectField(TEXT("Location"), PlayerLocationObject);

            // tags and player states ( not sure if state is nesseccary)
            TArray<TSharedPtr<FJsonValue>> PlayerTags;
            PlayerTags.Add(MakeShareable(new FJsonValueString(TEXT("Player"))));
            PlayerTags.Add(MakeShareable(new FJsonValueString(TEXT("Controllable"))));
            PlayerObject->SetArrayField(TEXT("Tags"), PlayerTags);

            TSharedPtr<FJsonObject> PlayerStateObject = MakeShareable(new FJsonObject());
            PlayerStateObject->SetNumberField(TEXT("Health"), 100); // Example value 
            PlayerStateObject->SetStringField(TEXT("Status"), TEXT("Idle")); // Example state 
            PlayerObject->SetObjectField(TEXT("State"), PlayerStateObject);
        }
    }
    RootObject->SetObjectField(TEXT("Player"), PlayerObject);


    // FString JSONString - holds the final JSON formatted string.
    FString JSONString;
    // TJsonWriterFactory<>::Create(&JSONString) - Cretaes an instance of a JSON writer.
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JSONString);
    // the serialization: serializes the rootobjects into the JSONString
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    UE_LOG(LogTemp, Log, TEXT("Generated JSON: %s"), *JSONString);
    return JSONString;
}