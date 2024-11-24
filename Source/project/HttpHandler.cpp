#include "HttpHandler.h"
#include "HttpModule.h"
#include "Http.h"
#include "Json.h"
#include "JsonUtilities.h"

// Konstruktor: Aktiverar Tick-funktionen
AHttpHandler::AHttpHandler()
{
    PrimaryActorTick.bCanEverTick = true; // Gör så att Tick() körs varje frame om det behövs
}

// Körs när spelet börjar eller när objektet skapas
void AHttpHandler::BeginPlay()
{
    Super::BeginPlay();

    // Startar en timer för att skicka HTTP-förfrågningar regelbundet
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AHttpHandler::SendHttpRequest, 5.0f, true);
    // Skickar en förfrågan var 5:e sekund
}

// Skickar en HTTP POST-förfrågan
void AHttpHandler::SendHttpRequest()
{
    FString JsonPayload = GenerateJsonData(); // Genererar JSON-data

    // Skapa och konfigurera HTTP-förfrågan
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("https://your-llm-server.com/api")); // Ersätt med din server-URL
    Request->SetVerb(TEXT("POST"));                          // HTTP-metod är POST
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json")); // Anger att innehållet är JSON
    Request->SetContentAsString(JsonPayload);                // Lägger till JSON-data som innehåll

    // Callback som körs när svaret mottagits
    Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr, FHttpResponsePtr Response, bool bWasSuccessful) {
        if (bWasSuccessful && Response.IsValid())
        {
            // Loggar serverns svar
            UE_LOG(LogTemp, Log, TEXT("LLM Response: %s"), *Response->GetContentAsString());
        }
        else
        {
            // Loggar om förfrågan misslyckades
            UE_LOG(LogTemp, Error, TEXT("HTTP Request failed"));
        }
    });

    Request->ProcessRequest(); // Skickar HTTP-förfrågan
}

// Genererar JSON-data som ska skickas
FString AHttpHandler::GenerateJsonData()
{
    // Skapa ett JSON-objekt
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    
    // Lägg till fält i JSON-objektet
    JsonObject->SetStringField(TEXT("player_name"), TEXT("Player1"));      // Exempel: Spelarnamn
    JsonObject->SetNumberField(TEXT("score"), 1234);                      // Exempel: Poäng
    JsonObject->SetNumberField(TEXT("time"), GetWorld()->GetTimeSeconds()); // Tid i spelet

    // Konvertera JSON-objektet till en sträng
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return JsonString; // Returnerar JSON som sträng
}
