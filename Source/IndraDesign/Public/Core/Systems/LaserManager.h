// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "singleton.h"
#include "Laser.h" //FLaserProperties
#include "icore.h"
#include <unordered_map>

class AINA_PawnBase;
class AINA_PawnController;
//class ALaser;
class APawn;
class UWorld;
class INDRADESIGN_API LaserManager : public Singleton<LaserManager>
{
private:
    friend Singleton<LaserManager>;
    LaserManager() {}
protected:
    std::unordered_multimap<const AINA_PawnController*, ALaser*> ActiveList;
    std::unordered_map<const AINA_PawnController*, std::pair<float,int>> SpawnInfo;
    std::unordered_map<const AINA_PawnBase*, FLaserProperties> PawnLaserProps;

public:
    //Controller->Pawn->LaserManager->SpawnBeam - ie. get owner on Pawn (should work for replicated pawns)
    ALaser* SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation);
    ALaser* SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation, const FVector& Velocity);
    ALaser* SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation, const FVector& Velocity, const FLaserProperties& Properties);
    void DespawnBeams(const AINA_PawnController* Owner);
    void EraseBeam(const AINA_PawnController* Owner, ALaser* Beam);
    void SetLaserProperties(const AINA_PawnBase* Vessel, const FLaserProperties& props);
    FLaserProperties GetLaserProperties(const AINA_PawnBase* Vessel);
};
