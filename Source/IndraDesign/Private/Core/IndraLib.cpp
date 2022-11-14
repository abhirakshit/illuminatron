// Fill out your copyright notice in the Description page of Project Settings.

#include "IndraLib.h"
#include "Engine/World.h"
#include "icore.h"
#include "INA_PawnBase.h"
#include "INA_PawnController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

UWorld* UIndraLib::World = nullptr;
inline UWorld* RetrieveWorld(UObject* WorldContextObject){
    if(WorldContextObject){
        return WorldContextObject->GetWorld();
    }
    return nullptr;
}

UClass* &UIndraLib::LaserProjectileType() {
    static UClass* type = nullptr;
    return type;
}

UClass* &UIndraLib::LaserSegmentType() {
    static UClass* type = nullptr;
    return type;
}

void UIndraLib::SetWorld(UObject* WorldContextObject){
    if(WorldContextObject && WorldContextObject->GetWorld()){
        World = RetrieveWorld(WorldContextObject);
    }
}


APlayerController* UIndraLib::GetPlayerController_impl(UObject* WorldContextObject){
    UWorld* world = RetrieveWorld(WorldContextObject);
    if(GEngine && world){
        return GEngine->GetFirstLocalPlayerController(world);
    }
    return nullptr;
}

AINA_PawnController* UIndraLib::GetINAController_impl(UObject* WorldContextObject){
    return Cast<AINA_PawnController>(GetPlayerController_impl(WorldContextObject));
}

AINA_PawnBase* UIndraLib::GetINAPawn_impl(UObject* WorldContextObject){
    APlayerController* pc = GetPlayerController_impl(WorldContextObject);
    if(pc){
        auto p = pc->GetPawn();
        return Cast<AINA_PawnBase>(p);
    }
    return nullptr;
}

void UIndraLib::QuitGame_impl(UWorld* world){
    if(GEngine && world){
        GEngine->GetFirstLocalPlayerController(world)->ConsoleCommand("quit");
    } else {
        IN_STATIC_LOG(IndraCriticalErrors,Error,true,"Invalid pointer");
        FGenericPlatformMisc::RequestExit(false);
    }
}

bool UIndraLib::isPIEWorld_impl(UWorld* world){
    if(world){
        return world->WorldType == EWorldType::PIE;
    }
    return false;
}

void UIndraLib::QuitGame(){
    QuitGame_impl(World);
}

bool UIndraLib::isPIEWorld(){
    return isPIEWorld_impl(World);
}

void UIndraLib::assert(bool assertion, const FString &msg){
    if(!assertion){
        logFatalF(IndraCriticalErrors,msg);
    }
}

void UIndraLib::TestCppMath() {
    FRandomStream RNGS;
    RNGS.GenerateNewSeed();
    auto i = FVector(1, 1, 0);
    i.Normalize();
    IndraMath::Refract(i, FVector(0, -1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
    IndraMath::Refract(RNGS.GetUnitVector(), FVector(0, 1, 0), 1.0f, 1.33f);
}
