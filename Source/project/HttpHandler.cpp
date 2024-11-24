#include "HttpHandler.h"
#include "HttpModule.h"
#include "Http.h"
#include "projectGameMode.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

void AHttpHandler::BeginPlay()
{
    /*
        Timer function the calls SendHttpRequest
    */
    Super::BeginPlay();

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("World is null! Cannot start timer."));
        return;
    }

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,                                         // Timer handle
        this,                                                // Calling object
        &AHttpHandler::SendHttpRequest,                      // Function to call
        1.0f,                                                // Delay in seconds
        true                                                 // Repeat indefinitely
    );
    UE_LOG(LogTemp, Log, TEXT("Timer set: world data is collected and sent to LLM every 1 second."));
}


void AHttpHandler::SendHttpRequest()
{
    /*
        Function calls GenerateJSON from projectGameMode.h to generate json string from world environment paramters.
        Function then sends the return json string to LLM using api in LmStudios.
    */

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("World is null! Cannot get GameMode."));
        return;
    }

    FString JSONString = TEXT("No data available") // default value

    AprojectGameMode* GameMode = World->GetAuthGameMode<AprojectGameMode>();
    if (GameMode)
    {
        FString JSONString = GameMode->GenerateJSON();

        UE_LOG(LogTemp, Log, TEXT("Generated JSON: %s"), *JSONString);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode is not of type AprojectGameMode!"));
    }

    // Skapa och konfigurera HTTP-förfrågan
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://your-llm-server.com/api")); 
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("text/plain")); 
    Request->SetContentAsString(JSONString);

 
    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) {
        if (bWasSuccessful && Response.IsValid())
        {
            int32 ResponseCode = Response->GetResponseCode();
            if (ResponseCode == 200)
            {
                UE_LOG(LogTemp, Log, TEXT("LLM Response: %s"), *Response->GetContentAsString());
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("HTTP Request failed with response code: %d"), ResponseCode);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
        }
    });

    Request->ProcessRequest(); 
}

