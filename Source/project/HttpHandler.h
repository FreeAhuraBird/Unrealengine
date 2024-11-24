#pragma once 

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

protected:

    virtual void BeginPlay() override;

public:
    
    void SendHttpRequest();

};
