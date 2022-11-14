//EndDebugBlock
// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserSegment.h"
#include "ConstructorHelpers.h"
#include "ParticleDefinitions.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "icore.h"

ALaserSegment::ALaserSegment(){
    LOGINITLEVEL(VeryVerbose);
    bReplicates = false;
    PrimaryActorTick.bCanEverTick = false; //segments don't need to tick
    
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = SceneRoot;

    ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle System Component"));
    ParticleComponent->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
    //ParticleComponent->

    Source = FVector(0,0,0);
    Target = FVector(0,0,0);
}

void ALaserSegment::BeginPlay(){
    LOGINITLEVEL(VeryVerbose);
    Super::BeginPlay();
    SetSource(Source);
    SetTarget(Target);
    Update();
}

void ALaserSegment::BeginDestroy(){
    LOGEVENT(Verbose);
    Super::BeginDestroy();
    // Before: m_Prev <-> this <-> m_Next
    if(m_Next){
        if(m_Next->IsValidLowLevel()){
            auto p = m_Next->GetPrev();
            if(p == this || p == nullptr){
                m_Next->SetPrev(m_Prev);
                goto mPREV;
            }
        }
        logWarning(IndraLasers,"Failed to re-weave m_Next -> m_Prev.")
    }
mPREV:
    if(m_Prev){
        if(m_Prev->IsValidLowLevel()){
            auto p = m_Prev->GetNext();
            if(p == this || p == nullptr){
                m_Prev->SetNext(m_Next);
                return;
            }
        }
        logWarning(IndraLasers,"Failed to re-weave m_Prev -> m_Next.")
    }
    // After: m_Prev <-> m_Next
}

void ALaserSegment::Stop(){
    LOGEVENT(Info);
    if(ParticleComponent){
        ParticleComponent->KillParticlesForced();
        ParticleComponent->DeactivateSystem();
    }
}

void ALaserSegment::Update(){
    LOGEVENT(VeryVerbose);
    if(ParticleComponent){
        if(ParticleMaterial){
            if(!ParticleComponent->GetNumMaterials()){
                ParticleComponent->SetMaterial(0,ParticleMaterial);
            }
        } else {
            logWarning(IndraLasers,"Material is null.");
        }
        ParticleComponent->KillParticlesForced();
        ParticleComponent->ActivateSystem();
    } else {
        logWarning(IndraLasers,"ParticleComponent is null.");
    }
    debugVeryVerbose(1,IndraLasers,"\n  Source: %s\n  Target: %s",*Source.ToString(),*Target.ToString());
}

void ALaserSegment::Move(const FVector &offset){
    SetSource(Source + offset);
    SetTarget(Target + offset);
    Update();
}

#pragma region "Property Interface"

void ALaserSegment::SetNext(ALaserSegment* next){
    //check if this is being called from Destroyed()
    if(m_Next){
        //don't log warning if this segment is being destroyed
        if(!this->IsActorBeingDestroyed()){
            logWarning(IndraLasers,"The current m_Next LaserSegment may become orphaned following this operation. this:(%s), prev:(%s)",*GetDebugName(this),*GetDebugName(m_Next));
        }
    }
    m_Next = next;
}

void ALaserSegment::SetPrev(ALaserSegment* prev){
    //check if this is being called from Destroyed()
    if(m_Prev){
        //don't log warning if this segment is being destroyed
        if(!this->IsActorBeingDestroyed()){
            logWarning(IndraLasers,"The current m_Prev LaserSegment may become orphaned following this operation. this:(%s), prev:(%s)",*GetDebugName(this),*GetDebugName(m_Prev));
        }
    }
    m_Prev = prev;
}

void ALaserSegment::SetSource(const FVector &src){
    Source = src;
    if(ParticleComponent){
        ParticleComponent->SetVectorParameter(TEXT("Source"),Source);
    }
    debugVerbose(4,IndraLasers,"Source: %s",*Source.ToString());
}

void ALaserSegment::SetTarget(const FVector &tgt){
    Target = tgt;
    if(ParticleComponent){
        ParticleComponent->SetVectorParameter(TEXT("Target"),Target);
    }
    debugVerbose(5,IndraLasers,"Target: %s",*Target.ToString());
}

void ALaserSegment::SetWidth(float value){
    Width = value;
    if(ParticleComponent){
        ParticleComponent->SetVectorParameter(TEXT("Width"),FVector(Width,Width,Width));
        debugVerbose(3,IndraLasers,"Width: %f",Width);
    }
}

void ALaserSegment::SetAngle(float value) {
    Angle = value;
    //todo: implement angle? maybe, maybe not..
}

void ALaserSegment::SetSoftness(float value){
    Softness = value;
    if(ParticleMaterial){
        ParticleMaterial->SetScalarParameterValue(TEXT("Softness"),Softness);
        debugVerbose(2,IndraLasers,"Softness: %f",Softness);
    }
}

void ALaserSegment::SetIntensity(float value) {
    Intensity = value;
    if (ParticleMaterial) {
        ParticleMaterial->SetScalarParameterValue(TEXT("Intensity"), Intensity);
        debugVerbose(2, IndraLasers, "Intensity: %f", Intensity);
    }
}

#pragma endregion
