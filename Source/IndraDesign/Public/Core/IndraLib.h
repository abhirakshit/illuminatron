// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IndraLib.generated.h"

class AINA_PawnBase;
class AINA_PawnController;

UCLASS()
class INDRADESIGN_API UIndraLib : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    static UWorld* World;
    UFUNCTION(Category = ".Indra-types", BlueprintCallable)
    static UClass* &LaserProjectileType();
    UFUNCTION(Category = ".Indra-types", BlueprintCallable)
    static UClass* &LaserSegmentType();
    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static void SetWorld(UObject* WorldContextObject);
    UFUNCTION(Category = ".Indra-fn|impl",BlueprintCallable/*,meta = (BlueprintProtected = "true")/**/)
    static APlayerController* GetPlayerController_impl(UObject* WorldContextObject);
    UFUNCTION(Category = ".Indra-fn|impl",BlueprintCallable/*,meta = (BlueprintProtected = "true")/**/)
    static AINA_PawnController* GetINAController_impl(UObject* WorldContextObject);
    UFUNCTION(Category = ".Indra-fn|impl",BlueprintCallable/*,meta = (BlueprintProtected = "true")/**/)
    static AINA_PawnBase* GetINAPawn_impl(UObject* WorldContextObject);
    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static void QuitGame_impl(UWorld* world);
    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static bool isPIEWorld_impl(UWorld* world);

    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static void QuitGame();
    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static bool isPIEWorld();
    UFUNCTION(Category = ".Indra-fn",BlueprintCallable)
    static void assert(bool assertion,const FString &msg);

    UFUNCTION(Category = ".Indra-fn", BlueprintCallable)
    static void TestCppMath();
    /*UFUNCTION(Category = ".Indra-fn",BlueprintNativeEvent)
    static void DoOnce();*/
};
