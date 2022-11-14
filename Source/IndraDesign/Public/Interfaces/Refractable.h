// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/LaserProjectileComp.h"
#include "Refractable.generated.h"

UINTERFACE(MinimalAPI)
class URefractable : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class INDRADESIGN_API IRefractable
{
    GENERATED_BODY()
public:
    virtual float GetOpacity() const = 0;
    virtual float GetRefractionIndex() const = 0;
    //void SetRefractionIndex(float value);
};
