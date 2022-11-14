// Fill out your copyright notice in the Description page of Project Settings.

#include "INA_PawnBase.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"

#include "INA_PawnController.h"
#include "ShellComponent.h"
#include "LaserManager.h"
#include "Laser.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({ {3,3},{5,64} });
//debugEvent(1,Verbose,"");
//debugEvent(2,Verbose,"");
//debugInfo(3,Indra,"Projectile Type: %s",*projectileType->GetName());
//debugVeryVerbose(4,Indra,"Moving pawn. current: [%s], dir: [%s], mag: %f",*GetActorLocation().ToString(),*Direction.ToString(),Magnitude);
//debugInfo(5, IndraLasers, "Spawning laser projectile at: (%s)", *(ThoughtStream->GetActorLocation().ToString()));
//debugInfo(6,IndraGeometry,"new radius = %f",radius);
//EndDebugBlock

AINA_PawnBase::AINA_PawnBase(){
    LOGINIT();
    PrimaryActorTick.bCanEverTick = true;
    SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
    bReplicates = true;
    NetPriority = 3.f;

    bCanBeDamaged = false;
    BaseEyeHeight = 0.f;
    bCollideWhenPlacing = true;
    LaserRNG.GenerateNewSeed();
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ShellComponent = CreateDefaultSubobject<UShellComponent>(TEXT("OutterShell"));
    ShellComponent->SetRadius(1.1f);
    RootComponent = ShellComponent;

    CameraRotatorArm = CreateDefaultSubobject<USpringArmComponent>( TEXT("CameraRotatorArm") );
    if(CameraRotatorArm){
        CameraRotatorArm->SetupAttachment(RootComponent);
        CameraRotatorArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        CameraRotatorArm->bDoCollisionTest = false;
        CameraRotatorArm->TargetArmLength = 1.1f + 15.f;
    } else {
        goto FATAL;
    }

    CameraZoomArm = CreateDefaultSubobject<USpringArmComponent>( TEXT("CameraZoomArm") );
    if(CameraZoomArm){
        CameraZoomArm->SetupAttachment(CameraRotatorArm);
        CameraZoomArm->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        CameraZoomArm->TargetArmLength = 300.f;
    } else {
        goto FATAL;
    }
    return;

    FATAL:
    logFatal(IndraInit,"Failed to construct components");
}

void AINA_PawnBase::BeginDestroy(){
    LOGEVENT(Info);
    Super::BeginDestroy();
    StopLaser();
}

void AINA_PawnBase::BeginPlay(){
    LOGINIT();
    UIndraLib::World = GetWorld();
    Super::BeginPlay();
}

void AINA_PawnBase::DisableProjectileCollisions(){
    debugEvent(1,Verbose,"");
    if (ThoughtStream) {
        ThoughtStream->SetActorEnableCollision(false);
    }
}

void AINA_PawnBase::EnableProjectileCollisions(){
    debugEvent(2,Verbose,"");
    if (ThoughtStream) {
        ThoughtStream->SetActorEnableCollision(true);
    }
}

//Shell Mesh
void AINA_PawnBase::SetShellProperties(UShellComponent* shell, uint8 subdivisions, float radius, float noise_power) {
    LOGEVENT(Info);
    if (shell) {
        shell->SetRadius(radius);
        shell->Noise = noise_power;
        shell->Subdivisions = subdivisions;
    }
}

void AINA_PawnBase::SetLaserProjectileType(UClass* projectileType){
    LOGEVENT(Info);
    if(projectileType && projectileType->IsValidLowLevel()){
        UIndraLib::LaserProjectileType() = projectileType;
        debugInfo(3,Indra,"Projectile Type: %s",*projectileType->GetName());
    } else {
        logWarning(IndraLasers,"Projectile Type is null or invalid");
    }
}

void AINA_PawnBase::Move(const FVector &Direction, const float &Magnitude){
    debugVeryVerbose(4,Indra,"Moving pawn. current: [%s], dir: [%s], mag: %f",*GetActorLocation().ToString(),*Direction.ToString(),Magnitude);
    if (FMath::IsNearlyZero(Magnitude) || Direction.IsNearlyZero()) {
        return;
    } else {
        FVector Loc = GetActorLocation();
        FHitResult hit;
        DisableProjectileCollisions();
        SetActorLocation(GetActorLocation() + (Direction * Magnitude * MoveSpeed), true, &hit);
        if (hit.bBlockingHit) {
            FVector correction = Direction - Direction.ProjectOnToNormal(hit.ImpactNormal);
            SetActorLocation(GetActorLocation() + (correction * Magnitude * MoveSpeed), true);
        }
        if (ThoughtStream) {
            ThoughtStream->OffsetLaser(GetActorLocation() - Loc);
            EnableProjectileCollisions();
        }
    }
}

static FString ToString(const FLaserProperties &props) {
    return Debug::sprintf(L"\nIntensity: %f\nSoftness: %f\nWidth: %f\nLength: %f", props.Intensity, props.Softness, props.Width, props.Length);
}

void AINA_PawnBase::StartLaser(){
    LOGEVENT(Info);
    if (!ThoughtStream) {
        if (ShellComponent) {
            auto LPType = UIndraLib::LaserProjectileType();
            if (LPType && LPType->IsValidLowLevel()) {
                auto pc = Cast<AINA_PawnController>(this->GetController());
                float radius = ShellComponent->GetRadius();
                pc->SetBeamLength(radius);

                FVector pos = GetActorLocation() + (LaserRNG.GetUnitVector() * FMath::RandRange(0.8f * radius, 0.95f * radius));
                FRotator rot = LaserRNG.GetUnitVector().Rotation();
                ThoughtStream = LaserManager::get().SpawnBeam(pc, GetWorld(), pos, rot);
                
                debugInfo(5, IndraLasers, "Spawning laser projectile at: (%s)", *(ThoughtStream->GetActorLocation().ToString()));
                ThoughtStream->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            } else {
                logFatal(IndraLasers, "LaserSegmentType is null or invalid");
            }
        } else {
            logError(IndraLasers, "ShellComponent is null");
        }
    }
    
}

void AINA_PawnBase::StopLaser(){
    LOGEVENT(Info);
    if (IsValid(ThoughtStream)) {
        auto pc = Cast<AINA_PawnController>(this->GetController());
        LaserManager::get().EraseBeam(pc, ThoughtStream);
        ThoughtStream->Destroy();
    }
}


void AINA_PawnBase::SetRadius(float radius){
    if(ShellComponent && CameraRotatorArm){
        debugInfo(6,IndraGeometry,"new radius = %f",radius);
        ShellComponent->SetRadius(radius);
        CameraRotatorArm->TargetArmLength = radius * 1.1f;
    }
}

void InitializeINA_PawnInputBindings();
// Called to bind functionality to input
void AINA_PawnBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
    LOGEVENT(Info);
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    //check(PlayerInputComponent);
    InitializeINA_PawnInputBindings();
    auto pc = Cast<AINA_PawnController>(GetController());
    if (pc) {
        LOGEVENTINFO(Info, "Controller exists, binding functions");
        PlayerInputComponent->BindAxis("ZoomIn", pc, &AINA_PawnController::ZoomCamera);
        PlayerInputComponent->BindAxis("Turn", pc, &AINA_PawnController::RotateCameraRight);
        PlayerInputComponent->BindAxis("LookUp", pc, &AINA_PawnController::RotateCameraUp);
        PlayerInputComponent->BindAxis("MoveRight", pc, &AINA_PawnController::MovePawnRight);
        PlayerInputComponent->BindAxis("MoveForward", pc, &AINA_PawnController::MovePawnForward);
    }
}

void InitializeINA_PawnInputBindings() {
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("ZoomIn", EKeys::MouseWheelAxis, -10.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::MouseX, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUp", EKeys::MouseY, 1.f));

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::D, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::A, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Gamepad_LeftX, 1.f));

    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::S, -1.f));
    UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Gamepad_LeftY, 1.f));
    
#if !PLATFORM_ANDROID
#else
#endif
}
