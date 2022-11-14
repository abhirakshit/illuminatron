// Fill out your copyright notice in the Description page of Project Settings.

#include "Laser.h"
#include "LaserManager.h"
#include "LaserSegment.h"
#include "Refractable.h"
#include "INA_PawnController.h"
#include "LaserProjectileComp.h"
#include "INA_PawnBase.h"
#include "ShellComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({ {1,2},{7,7} });
//debugInfo(1, IndraLasers, "Segment Type: %s", *segmentType->GetName());
//debugInfo(2,IndraLasers,"Segment Type: %s",*UIndraLib::LaserSegmentType()->GetName());
//debugEvent(3, VeryVerbose, "");
//debugVerbose(4, IndraLasers, "Num laser segments = %d", LaserParts.size());
//debugEvent(5, VeryVerbose, "Updating laser segments");
//debugVerbose(6, IndraLasers, "laser segment at: (%s)", *(p->GetActorLocation().ToString()));
//debugVerbose(6, IndraLasers, "laser segment at: (%s)", *(p->GetActorLocation().ToString()));
//debugEvent(7, VeryVerbose, "setting new laser segment's source/target");
//debugInfo(8, IndraLasers, "%s", *ToString(props));
//debugInfo(9, IndraLasers, "%s", *ToString(p->props));
//debugVeryVerbose(10, IndraLasers, "Moving LaserSegment at (%s) offset (%s)", *lp->GetActorLocation().ToString(), *offset.ToString());
//EndDebugBlock

static FString ToString(const FLaserProperties &props) {
    return Debug::sprintf(L"\nIntensity: %f\nSoftness: %f\nWidth: %f\nWidthScalar: %f\nLength: %f", props.Intensity, props.Softness, props.Width, props.WidthScalar, props.Length);
}

#pragma region "Object Managment"

ALaser::ALaser(){
    LOGINIT();
    bReplicates = true;
    NetUpdateFrequency = 0.666f;
    PrimaryActorTick.bCanEverTick = true;
    //ForceNetUpdate();
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
    CollisionComponent->InitSphereRadius(1.f);
    CollisionComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
    CollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->bDynamicObstacle = true;
    CollisionComponent->bTraceComplexOnMove = true;
    CollisionComponent->CanCharacterStepUpOn = ECB_No;
    CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    RootComponent = CollisionComponent;

    ProjectileComp = CreateDefaultSubobject<ULaserProjectileComp>(TEXT("ProjMovementComp"));
    ProjectileComp->bRotationFollowsVelocity = true;
    ProjectileComp->bShouldBounce = true;
    ProjectileComp->Bounciness = 1.f;
    ProjectileComp->Friction = 0.f;
    ProjectileComp->MaxSpeed = 0.f;
    ProjectileComp->InitialSpeed = 100.f;
    ProjectileComp->ProjectileGravityScale = 0.f;
}

void ALaser::SetSegmentType(UClass* segmentType) {
    LOGEVENT(Info);
    if (segmentType && segmentType->IsValidLowLevel()) {
        UIndraLib::LaserSegmentType() = segmentType;
        debugInfo(1, IndraLasers, "Segment Type: %s", *segmentType->GetName());
    } else {
        logWarning(IndraLasers, "Segment Type is null or invalid");
    }
}

void ALaser::Initialize() {
    for (auto part : LaserParts) {
        part->SetWidth(props.WidthScalar * props.Width);
        part->SetAngle(props.Angle);
        part->SetSoftness(props.Softness);
        part->SetIntensity(props.Intensity);
    }
}

void ALaser::BeginPlay(){
    Super::BeginPlay();
    LOGINIT();
    if(UIndraLib::LaserSegmentType() && UIndraLib::LaserSegmentType()->IsValidLowLevel()){
        debugInfo(2,IndraLasers,"Segment Type: %s",*UIndraLib::LaserSegmentType()->GetName());
        Reflect();
    } else {
        logWarning(IndraLasers,"Segment Type is null or invalid");
    }
}

void ALaser::Destroyed(){
    Super::Destroyed();
    LOGEVENT(Info);
    while(!LaserParts.empty()){
        LaserParts.front()->Destroy();
        LaserParts.pop_front();
    }
}

#pragma endregion

void ALaser::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    debugEvent(3, VeryVerbose, "");
    if (!LaserParts.empty()) {
        debugVerbose(4, IndraLasers, "Num laser segments = %d", LaserParts.size());
        UpdateLength();
        auto p = LaserParts.front();
        float prop1 = FMath::Abs(LaserParts.back()->GetWidth() - (props.WidthScalar * props.Width));
        float prop2 = FMath::Abs(LaserParts.back()->GetSoftness() - props.Softness);
        float prop3 = FMath::Abs(LaserParts.back()->GetIntensity() - props.Intensity);
        bool skiplerping = FMath::IsNearlyZero(prop1) && FMath::IsNearlyZero(prop2) && FMath::IsNearlyZero(prop3);
        if (!skiplerping) {
            debugEvent(5, VeryVerbose, "Updating laser segments");
            p->SetWidth(FMath::Lerp(p->GetWidth(), props.WidthScalar * props.Width, DeltaTime));
            p->SetSoftness(FMath::Lerp(p->GetSoftness(), props.Softness, DeltaTime));
            p->SetIntensity(FMath::Lerp(p->GetIntensity(), props.Intensity, DeltaTime));
            p->Update();
        }
    }
    //todo: implement tail length & Segment max
}

void ALaser::UpdateLength() {
    LOGEVENT(VeryVerbose);
    auto p = LaserParts.front();
    float deltaLen = FVector::Distance(p->GetTarget(), GetActorLocation());
    p->SetTarget(GetActorLocation());
    //p->Update();
    if (Length() >= props.Length) {
        auto lp = LaserParts.back();
        float backLen = lp->Length();
        if (backLen >= deltaLen) {
            lp->SetSource(FMath::Lerp(lp->GetSource(), lp->GetTarget(), deltaLen / backLen));
        } else {
            ClampLength();
        }
    }
}

float ALaser::Length() const {
    if (!LaserParts.empty()) {
        float Length = 0.f;
        auto lp = LaserParts.back();
        do {
            Length += lp->Length();
            lp = lp->GetNext();
        } while (lp);
        return Length;
    }
    return 0.f;
}

void ALaser::ClampLength() {
    LOGEVENT(Verbose);
    float len = Length();
    auto lp = LaserParts.back();
    float seglen = lp->Length();
    while ((len - seglen) > props.Length) {
        len -= seglen;
        lp->Destroy();
        LaserParts.pop_back(); //std::deque, gotta remove it ourselves
        lp = LaserParts.back();
        seglen = lp->Length();
    }
    float extra = len - props.Length;
    float ratio = extra / seglen;
    lp->SetSource(FMath::Lerp(lp->GetSource(), lp->GetTarget(), ratio));
}

void ALaser::Reflect() {
    LOGEVENT(Verbose);
    auto LSType = UIndraLib::LaserSegmentType();
    if (LSType && LSType->IsValidLowLevel()) {
        ALaserSegment* p = GetWorld()->SpawnActor<ALaserSegment>(LSType, this->GetActorLocation(), this->GetActorRotation());
        debugVerbose(6, IndraLasers, "laser segment at: (%s)", *(p->GetActorLocation().ToString()));
        p->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
        debugVerbose(6, IndraLasers, "laser segment at: (%s)", *(p->GetActorLocation().ToString()));

        if (!LaserParts.empty()) {
            auto front = LaserParts.front();
            front->SetTarget(GetActorLocation());
            front->SetNext(p);
            p->SetPrev(front);
        }
        LaserParts.push_front(p);
        p->SetWidth(props.WidthScalar * props.Width);
        p->SetAngle(props.Angle);
        p->SetSoftness(props.Softness);
        p->SetIntensity(props.Intensity);
        debugEvent(7, VeryVerbose, "setting new laser segment's source/target");
        p->SetSource(GetActorLocation());
        p->SetTarget(GetActorLocation());
        p->Update();
    } else {
        logFatal(IndraLasers, "LaserSegmentType is null or invalid");
    }
}

ALaser* ALaser::Refract(const FHitResult& Hit, IRefractable* surface) {
    LOGEVENT(Verbose);
    float time = UGameplayStatics::GetRealTimeSeconds(this);
    if (time - LastRefractTime >= 1.f) {
        //todo: add debug line
        LastRefractTime = time;
        auto v_inc = ProjectileComp->Velocity;
        auto& n_inc = Hit.ImpactNormal;
        float speed = v_inc.Size();
        v_inc /= speed;
        //todo: integrate thickness into rindex
        FVector v_refract = speed * IndraMath::Refract(v_inc, n_inc, 1.00f, surface->GetRefractionIndex());
        FVector pos = GetActorLocation() + (-0.33 * n_inc);

        AINA_PawnController* pc = Cast<AINA_PawnController>(Cast<AINA_PawnBase>(Hit.GetActor())->GetController());
        ALaser* p = LaserManager::get().SpawnBeam(pc, GetWorld(), pos, v_refract.Rotation(), v_refract, props);
        debugInfo(8, IndraLasers, "%s", *ToString(props));
        LOGEVENT(Info);
        debugInfo(9, IndraLasers, "%s", *ToString(p->props));
        //Destroy();
        return p;
        //ALaserProjectile* p = GetWorld()->SpawnActor<ALaserProjectile>(LPType, pos, v_refract.Rotation());
        /*v_refract *= speed;
        p->SetVelocity(v_refract);
        p->GetLaserActor()->SoftnessTarget = laser->GetLaser()->SoftnessTarget;
        p->GetLaserActor()->WidthTarget = laser->GetLaser()->WidthTarget;   */
    } else {
        //todo: add debug line
    }
    return nullptr;
}

void ALaser::OffsetLaser_Implementation(const FVector &offset) {
    LOGEVENT(VeryVerbose);
    if (!LaserParts.empty()) {
        auto lp = LaserParts.back();
        do {
            debugVeryVerbose(10, IndraLasers, "Moving LaserSegment at (%s) offset (%s)", *lp->GetActorLocation().ToString(), *offset.ToString());
            lp->Move(offset);
            lp = lp->GetNext();
        } while (lp);
    }
}

void ALaser::SetVelocity(const FVector& velocity) {
    if (ProjectileComp) {
        ProjectileComp->Velocity = velocity;
    }
}
