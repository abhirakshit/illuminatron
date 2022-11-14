// Fill out your copyright notice in the Description page of Project Settings.

#include "LaserProjectileComp.h"
#include "ProceduralMeshComponent.h"
#include "Refractable.h"
#include "Laser.h"
#include "LaserSegment.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({{1,64}});
//debugInfo(1, IndraEvents, "%s", *Debug::who(this));
//EndDebugBlock

class UProceduralMeshComponent;

void ULaserProjectileComp::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) {
    Super::HandleImpact(Hit, TimeSlice, MoveDelta);
    debugEvent(1, Verbose, "%s", *Debug::who(this));
    if (GetLaserActor() && GetLaserActor()->IsValidLowLevel()) {
        //todo: implement logic to decide if the beam should just refract, reflect, or both
        GetLaserActor()->Reflect();
        //can the surface refract the beam?
        if (Hit.GetComponent()) {
            if (Hit.GetComponent()->Implements<URefractable>()) {
                GetLaserActor()->Refract(Hit, Cast<IRefractable>(Hit.GetComponent()));
            } else if (Hit.GetComponent()->GetAttachParent()) {
                if (Hit.GetComponent()->GetAttachParent()->Implements<URefractable>()) {
                    GetLaserActor()->Refract(Hit, Cast<IRefractable>(Hit.GetComponent()->GetAttachParent()));
                } else {
                    logWarning(IndraLasers, "Collision can't be acted upon. hit.component.parent = %s", *Hit.GetComponent()->GetAttachParent()->GetFullName());
                }
            } else {
                logWarning(IndraLasers, "Collision can't be acted upon. hit.component = %s", *Hit.GetComponent()->GetFullName());
            }
        } else {
            logError(IndraLasers, "Collided with nothing?? No idea what just happened, because this line should never print");
        }
    }
}

ALaser* ULaserProjectileComp::GetLaserActor() const {
    return Cast<ALaser>(GetOwner());
}
