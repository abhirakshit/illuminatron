// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LaserProjectileComp.generated.h"

class ALaser;

UCLASS()
class INDRADESIGN_API ULaserProjectileComp : public UProjectileMovementComponent
{
    GENERATED_BODY()

protected:
    virtual void HandleImpact(const FHitResult& Hit, float TimeSlice=0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

    //aborts if the object is dead
    //virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;
    //deflection as in sliding along a wall (into the wall), see also `HandleSliding`
    //virtual bool HandleDeflection(FHitResult& Hit, const FVector& OldVelocity, const uint32 NumBounces, float& SubTickTimeRemaining) override;

public: 
    UFUNCTION(Category = ".INA|Laser|ProjectileComponent", BlueprintCallable)
    ALaser* GetLaserActor() const;
};
