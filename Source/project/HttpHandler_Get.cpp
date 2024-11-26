// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpHandler_Get.h"
#include "Components/TextBlock.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"

void UHttpHandler_Get::NativeConstruct()
{
    Super::NativeConstruct();

    FetchGameState();
}

void UHttpHandler_Get::FetchGameState()
{
    FHttpModule* Http = &FHttpModule::Get();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

    // Set up the HTTP request
    Request->SetURL(TEXT("http://127.0.0.1:1234/v1/completions")); // LM Studio URL
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Create the JSON payload
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
    JsonObject->SetStringField(TEXT("prompt"), TEXT("What is the current game state?"));

    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    // Attach the JSON payload to the request
    Request->SetContentAsString(RequestBody);

    // Bind the response handler
    Request->OnProcessRequestComplete().BindUObject(this, &UHttpHandler_Get::OnResponseReceived);

    // Send the request
    Request->ProcessRequest();
}

void UHttpHandler_Get::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("HTTP request was successful."));

        TSharedPtr<FJsonObject> JsonResponse;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

        if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
        {
            UE_LOG(LogTemp, Warning, TEXT("JSON response deserialized successfully."));

            const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
            if (JsonResponse->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray && ChoicesArray->Num() > 0)
            {
                UE_LOG(LogTemp, Warning, TEXT("'choices' array found with %d elements."), ChoicesArray->Num());

                // Access the first element of the "choices" array
                TSharedPtr<FJsonObject> ChoiceObject = ChoicesArray->operator[] (0)->AsObject();
                if (ChoiceObject.IsValid())
                {
                    FString AIResponse;
                    if (ChoiceObject->TryGetStringField(TEXT("text"), AIResponse))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("AI response: %s"), *AIResponse);

                        // Update the text block with the response
                        if (GameStateText)
                        {
                            GameStateText->SetText(FText::FromString(AIResponse));
                            UE_LOG(LogTemp, Warning, TEXT("GameStateText updated successfully."));
                        }
                        else
                        {
                            UE_LOG(LogTemp, Error, TEXT("GameStateText is null."));
                        }
                        return; // Successfully processed
                    }
                    else
                    {
                        UE_LOG(LogTemp, Error, TEXT("Failed to extract 'text' field from 'choices[0]'."));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("ChoiceObject is invalid."));
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to find or parse 'choices' array."));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON response."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HTTP request failed or response is invalid."));
    }

    // Fallback: Handle errors and update UI with a failure message
    if (GameStateText)
    {
        GameStateText->SetText(FText::FromString(TEXT("Failed to fetch or parse game state.")));
        UE_LOG(LogTemp, Warning, TEXT("Fallback: Displayed failure message in GameStateText."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameStateText is null in fallback."));
    }
}


