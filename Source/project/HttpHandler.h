#pragma once // Säkerställer att filen bara inkluderas en gång vid kompilering

#include "CoreMinimal.h"        // Importerar grundläggande funktioner och typer från Unreal Engine
#include "HttpModule.h"         // Ger stöd för HTTP-förfrågningar
#include "Http.h"               // Hanterar HTTP-klasser och metoder
#include "Json.h"               // Stöd för att skapa och hantera JSON-data
#include "JsonUtilities.h"      // Verktyg för att enkelt arbeta med JSON i Unreal Engine
#include "HttpHandler.generated.h" // Hanterar UE-specifika funktioner för UCLASS och andra makron

/**
 * Klass för att hantera HTTP-förfrågningar
 * Ärver från AActor, vilket gör att den kan placeras i spelet
 */
UCLASS()
class PROJECT_API AHttpHandler : public AActor
{
    GENERATED_BODY()

public:
    /** Konstruktor - körs när klassen skapas */
    AHttpHandler();

protected:
    /** Körs när spelet börjar eller när objektet skapas */
    virtual void BeginPlay() override;

public:
    /**
     * Funktion för att skicka en HTTP POST-förfrågan
     */
    void SendHttpRequest();

    /**
     * Funktion för att generera JSON-data som ska skickas
     * @return JSON-data som en FString
     */
    FString GenerateJsonData();
};
