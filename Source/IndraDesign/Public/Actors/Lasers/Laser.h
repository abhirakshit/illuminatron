// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <deque>
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "icore.h"
#include "UnrealNetwork.h"
#include "Laser.generated.h"

class ALaserSegment;
class USphereComponent;
class ULaserProjectileComp;
class IRefractable;

USTRUCT(BlueprintType)
struct FLaserProperties {
    GENERATED_BODY()
    UPROPERTY() float Length = 1000.f;
    UPROPERTY() float Width;
    UPROPERTY() float WidthScalar = 1.f;
    UPROPERTY() float Angle; //beam angle
    UPROPERTY() float Softness; //edge softness
    UPROPERTY() float Intensity; //overall opacity?
    UPROPERTY() bool ConvexCone;
    UPROPERTY() bool ConcaveCone; //If both are true, it is an X shape (a second material may be the easiest way to accomplish this well)
};

UCLASS()
class INDRADESIGN_API ALaser : public AActor
{
    GENERATED_BODY()
private:
    std::deque<ALaserSegment*> LaserParts;
    float LastRefractTime = 0.f;

protected:
    UPROPERTY(Category = ".INA|Laser|Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USphereComponent* CollisionComponent;
    UPROPERTY(Category = ".INA|Laser-Projectile|Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    ULaserProjectileComp* ProjectileComp;


public:
    FLaserProperties props;
    ALaser();
    UFUNCTION(Category = ".INA|Laser|Methods",BlueprintCallable, meta=(BlueprintProtected = "true"))
    void SetSegmentType(UClass* segmentType);
    
    void Initialize();
    virtual void BeginPlay() override;
    virtual void Destroyed() override;
    virtual void Tick(float DeltaTime) override;

protected:
    void UpdateLength();
    float Length() const;
    void ClampLength();

public:
    void Reflect();
    ALaser* Refract(const FHitResult& Hit, IRefractable* shell);

    UFUNCTION(NetMulticast, Unreliable, Category = ".INA|Laser-Projectile|Methods", BlueprintCallable)
    void OffsetLaser(const FVector &offset);
    void OffsetLaser_Implementation(const FVector &offset);

    UFUNCTION(Category = ".INA|Laser|Methods",BlueprintCallable)
    ALaserSegment* PeekHead() const { return LaserParts.front(); }
    UFUNCTION(Category = ".INA|Laser|Methods",BlueprintCallable)
    ALaserSegment* PeekTail() const { return LaserParts.back(); }

    void SetVelocity(const FVector& velocity);
    
};
