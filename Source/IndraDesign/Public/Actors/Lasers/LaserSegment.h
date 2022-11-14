// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "LaserSegment.generated.h"

UCLASS()
class INDRADESIGN_API ALaserSegment : public AActor
{
    GENERATED_BODY()

public:
    ALaserSegment();

private:
    UPROPERTY(Category = ".INA|Laser-Segment|Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    USceneComponent* SceneRoot;
    UPROPERTY(Category = ".INA|Laser-Segment|Components", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UParticleSystemComponent* ParticleComponent;
    UPROPERTY(Category = ".INA|Laser-Segment|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UMaterialInstanceDynamic* ParticleMaterial;

protected:
    UPROPERTY(Category = ".INA|Laser-Segment|Properties", EditInstanceOnly, BlueprintReadWrite, meta=(BlueprintProtected = "true"))
    ALaserSegment* m_Next = nullptr;
    UPROPERTY(Category = ".INA|Laser-Segment|Properties", EditInstanceOnly, BlueprintReadWrite, meta=(BlueprintProtected = "true"))
    ALaserSegment* m_Prev = nullptr;
    UPROPERTY(Category = ".INA|Laser-Segment|Properties", EditAnywhere, BlueprintReadWrite, meta=(BlueprintProtected = "true"))
    FVector Source;
    UPROPERTY(Category = ".INA|Laser-Segment|Properties", EditAnywhere, BlueprintReadWrite, meta=(BlueprintProtected = "true"))
    FVector Target;
    float Width;
    float Angle;
    float Softness;
    float Intensity;

public:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;

    //UFUNCTION(Category = ".INA|Laser-Segment",BlueprintCallable)
    void Stop();
    void Update();
    void Move(const FVector &offset);

    FVector GetDirection() const {return Target - Source;}
    float Length() const { return GetDistance(); }
    float GetDistance() const { return FVector::Distance(Source,Target); }
    float GetDistanceSq() const { return FVector::DistSquared(Source,Target); }

#pragma region "Property Interface"
    void SetNext(ALaserSegment* next);
    void SetPrev(ALaserSegment* prev);
    void SetSource(const FVector &src);
    void SetTarget(const FVector &tgt);
    void SetWidth(float value);
    void SetAngle(float value);
    void SetSoftness(float value);
    void SetIntensity(float value);

    ALaserSegment* GetNext() const { return m_Next; }
    ALaserSegment* GetPrev() const { return m_Prev; }
    FVector GetSource() const { return Source; }
    FVector GetTarget() const { return Target; }
    float GetWidth() const { return Width; }
    float GetAngle() const { return Angle; }
    float GetSoftness() const { return Softness; }
    float GetIntensity() const { return Intensity; }
#pragma endregion

};
