// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "INA_PawnBase.generated.h"

class UShellComponent;
class UPawnMovementComponent;
class USpringArmComponent;
class ALaser;

UCLASS(BlueprintType, Blueprintable)
class INDRADESIGN_API AINA_PawnBase : public APawn
{
    GENERATED_BODY()
private:
    FRandomStream LaserRNG;
    UPROPERTY()
    ALaser* ThoughtStream = nullptr;

    UPROPERTY(Category = ".INA|Pawn|Components",VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
    UShellComponent* ShellComponent;
    UPROPERTY(Category = ".INA|Pawn|Components",VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraRotatorArm;
    UPROPERTY(Category = ".INA|Pawn|Components",VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* CameraZoomArm;

    UPROPERTY(Category = ".INA|Pawn|Properties",EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
    float MoveSpeed = 15.f;

protected:
    virtual void BeginDestroy() override;
    virtual void BeginPlay() override;
    void DisableProjectileCollisions();
    void EnableProjectileCollisions();

    UFUNCTION(Category = ".INA|Pawn|Shell",BlueprintCallable,meta = (BlueprintProtected = "true")/**/)
    void SetShellProperties(UShellComponent* shell, uint8 subdivisions, float radius, float noise_power);

public:
    AINA_PawnBase();
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(Category = ".INA|Pawn|Properties",BlueprintCallable,meta = (BlueprintProtected = "true"))
    void SetLaserProjectileType(UClass* projType);

    UShellComponent* GetShellComponent() const { return ShellComponent; }
    UFUNCTION(Category = ".INA|Pawn|Components",BlueprintCallable,meta = (BlueprintProtected = "true"))
    USpringArmComponent* GetCameraRotatorArm() const { return CameraRotatorArm; }
    UFUNCTION(Category = ".INA|Pawn|Components",BlueprintCallable,meta = (BlueprintProtected = "true"))
    USpringArmComponent* GetCameraZoomArm() const { return CameraZoomArm; }

    UFUNCTION(Category = ".INA|Pawn|Methods",BlueprintCallable)
    void Move(const FVector &Direction, const float &Magnitude);

    UFUNCTION(Category = ".INA|Pawn|Methods",BlueprintCallable)
    void StartLaser();
    UFUNCTION(Category = ".INA|Pawn|Methods",BlueprintCallable)
    void StopLaser();

    UFUNCTION(Category = ".INA|Pawn|Methods",BlueprintCallable)
    void SetRadius(float radius);

};
