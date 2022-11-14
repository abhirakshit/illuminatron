#include "LaserManager.h"
#include "INA_PawnBase.h"
#include "INA_PawnController.h"
#include "Kismet/GameplayStatics.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({ {4,6},{20,21} });
//debugEvent(30, Info, "");
//debugInfoC(20, IndraLasers, DColor::Yellow, "Blocked beam spawn, returning nullptr.");
//debugInfo(1, IndraLasers, "Spawning beam");
//debugInfo(1, IndraLasers, "Beam spawned");
//debugEvent(31, Info, "");
//debugInfo(2, IndraLasers, "Spawning beam with velocity, and Pawn Laser's default properties.");
//debugInfoC(21, IndraLasers, DColor::Yellow, "Returning nullptr.");
//debugEvent(31, Info, "");
//debugInfo(3, IndraLasers, "Beam spawned, setting properties.");
//debugInfoC(21, IndraLasers, DColor::Yellow, "Returning nullptr.");
//debugEvent(30, Info, "");
//debugInfo(4, IndraLasers, "Erasing beam from ActiveList.");
//debugInfo(4, IndraLasers, "Destroying beam.");
//debugEvent(30, Info, "");
//debugInfo(5, IndraLasers, "Erasing beam from ActiveList.");
//debugEvent(30, Info, "");
//debugInfo(6, IndraLasers, "Setting Pawn Laser's default property values.");
//debugInfo(6, IndraLasers, "Updating beam's properties.");
//debugEvent(30, Info, "");
//EndDebugBlock

#define SPAWNRATELIMIT 5

ALaser* LaserManager::SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation) {
    debugEvent(30, Info, "");
    if (Owner && Owner->GetINAPawn()) {
        int &count = SpawnInfo[Owner].second;
        float &lastTime = SpawnInfo[Owner].first;
        float time = UGameplayStatics::GetRealTimeSeconds(Owner);
        if (time - lastTime < 0.2f) {
            if (count < SPAWNRATELIMIT) {
                count++;
                lastTime = time;
            } else {
                debugInfoC(20, IndraLasers, DColor::Yellow, "Blocked beam spawn, returning nullptr.");
                return nullptr;
            }
        } else {
            count -= (int)((time - lastTime) / (1.f / SPAWNRATELIMIT));
            count = FMath::Clamp(count, 0, SPAWNRATELIMIT);
            count++;
            lastTime = time;
        }
        auto LPType = UIndraLib::LaserProjectileType();
        if (LPType && LPType->IsValidLowLevel()) {
            debugInfo(1, IndraLasers, "Spawning beam");
            ALaser* p = World->SpawnActor<ALaser>(LPType, wPosition, Rotation);
            auto iter = PawnLaserProps.find(Owner->GetINAPawn());
            if (iter != PawnLaserProps.end()) {
                p->props = iter->second;
            }
            ActiveList.emplace(Owner, p);
            if (p) {
                debugInfo(1, IndraLasers, "Beam spawned");
            }
            return p;
        } else {
            logError(IndraLasers, "Laser Projectile Type was invalid");
        }
    }
    return nullptr;
}

ALaser* LaserManager::SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation, const FVector& Velocity) {
    debugEvent(31, Info, "");
    if (Owner && Owner->GetINAPawn()) {
        debugInfo(2, IndraLasers, "Spawning beam with velocity, and Pawn Laser's default properties.");
        return SpawnBeam(Owner, World, wPosition, Rotation, Velocity, GetLaserProperties(Owner->GetINAPawn()));
    }
    debugInfoC(21, IndraLasers, DColor::Yellow, "Returning nullptr.");
    return nullptr;
}

ALaser* LaserManager::SpawnBeam(const AINA_PawnController* Owner, UWorld* World, const FVector& wPosition, const FRotator& Rotation, const FVector& Velocity, const FLaserProperties& Properties) {
    debugEvent(31, Info, "");
    if (Owner && Owner->GetINAPawn()) {
        auto p = SpawnBeam(Owner, World, wPosition, Rotation);
        if (p) {
            debugInfo(3, IndraLasers, "Beam spawned, setting properties.");
            p->SetVelocity(Velocity);
            p->props = Properties;
            PawnLaserProps[Owner->GetINAPawn()] = p->props;
            p->Initialize();
            return p;
        }
    }
    debugInfoC(21, IndraLasers, DColor::Yellow, "Returning nullptr.");
    return nullptr;
}

void LaserManager::DespawnBeams(const AINA_PawnController* Owner) {
    debugEvent(30, Info, "");
    auto iter = ActiveList.lower_bound(Owner);
    while (iter != ActiveList.end() && iter->first == Owner) {
        debugInfo(4, IndraLasers, "Erasing beam from ActiveList.");
        if (IsValid(iter->second)) {
            debugInfo(4, IndraLasers, "Destroying beam.");
            iter->second->Destroy(); //todo ensure full destruction
        }
        iter = ActiveList.erase(iter);
    }
}

void LaserManager::EraseBeam(const AINA_PawnController* Owner, ALaser* Beam) {
    debugEvent(30, Info, "");
    auto range = ActiveList.equal_range(Owner);
    for (auto iter = range.first; iter != range.second; ++iter) {
        if (iter->second == Beam) {
            debugInfo(5, IndraLasers, "Erasing beam from ActiveList.");
            ActiveList.erase(iter);
            return;
        }
    }
}

void LaserManager::SetLaserProperties(const AINA_PawnBase* Vessel, const FLaserProperties& props) {
    debugEvent(30, Info, "");
    PawnLaserProps[Vessel] = props;
    debugInfo(6, IndraLasers, "Setting Pawn Laser's default property values.");
    auto iter = ActiveList.find(Cast<AINA_PawnController>(Vessel->GetController()));
    if (iter != ActiveList.end()) {
        debugInfo(6, IndraLasers, "Updating beam's properties.");
        iter->second->props = props;
    }
}

FLaserProperties LaserManager::GetLaserProperties(const AINA_PawnBase* Vessel) {
    debugEvent(30, Info, "");
    return PawnLaserProps[Vessel];
}
