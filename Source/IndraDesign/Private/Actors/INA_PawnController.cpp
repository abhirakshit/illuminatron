// Fill out your copyright notice in the Description page of Project Settings.

#include "INA_PawnController.h"
#include "LaserManager.h"
#include "icore.h"

void AINA_PawnController::BeginPlay(){
    LOGINIT();
    Super::BeginPlay();
}

#pragma region "Control Bindings"

void AINA_PawnController::ZoomCamera(float forward) {
    if (!FMath::IsNearlyZero(forward)) {
        if (GetINAPawn() && Mode == ControllerMode::Movement || Mode == ControllerMode::HUDCursor) {
            GetINAPawn()->GetCameraZoomArm()->TargetArmLength += forward;
        }
    }
}

void AINA_PawnController::RotateCameraRight(float right) {
    if (!FMath::IsNearlyZero(right)) {
        if (GetINAPawn() && Mode != ControllerMode::HUDCursor) {
            FRotator rot = GetINAPawn()->GetCameraRotatorArm()->GetTargetRotation() + FRotator(0.f, right, 0.f);
            GetINAPawn()->GetCameraRotatorArm()->SetWorldRotation(rot);
        }
    }
}

void AINA_PawnController::RotateCameraUp(float up) {
    if (!FMath::IsNearlyZero(up)) {
        if (GetINAPawn() && Mode != ControllerMode::HUDCursor) {
            FQuat q(FRotator(up, 0.f, 0.f));
            FQuat b = GetINAPawn()->GetCameraRotatorArm()->GetTargetRotation().Quaternion();
            b = b * q; //rotate b by q
            GetINAPawn()->GetCameraRotatorArm()->SetWorldRotation(b);
        }
    }
}

void AINA_PawnController::MovePawnRight(float right) {
    if (!FMath::IsNearlyZero(right)) {
        if (GetINAPawn() && Mode == ControllerMode::Movement) {
            GetINAPawn()->Move(GetINAPawn()->GetCameraRotatorArm()->GetRightVector(), right);
        }
    }
}

void AINA_PawnController::MovePawnForward(float forward) {
    if (!FMath::IsNearlyZero(forward)) {
        if (GetINAPawn() && Mode == ControllerMode::Movement) {
            GetINAPawn()->Move(GetINAPawn()->GetCameraRotatorArm()->GetForwardVector(), forward);
        }
    }
}

void AINA_PawnController::MoveShellRight(float right) {
    if (Mode == ControllerMode::Placement) {
        selectedShell;
    }
}

void AINA_PawnController::MoveShellForward(float forward) {
    if (Mode == ControllerMode::Placement) {
        selectedShell;
    }
}

void AINA_PawnController::MoveShellUp(float up) {
    if (Mode == ControllerMode::Placement) {
        selectedShell;
    }
}

#pragma endregion


#pragma region "Property Interface"
#define RI_MIN 0.1f
#define RI_MAX 2.0f
#define RAD_MIN 1.0f
#define RAD_MAX 250.0f
void AINA_PawnController::SetSelectedShell(UShellComponent* shell) {
    if (shell && shell->IsValidLowLevel()) {
        selectedShell = shell;
    }
}

void AINA_PawnController::SetShellVertexNoise(float power) {
    if (selectedShell) {
        selectedShell->SetVertexNoise(power);
    }
}

void AINA_PawnController::SetShellRadius(float value) {
    if (selectedShell) {
        value = ((RAD_MAX - RAD_MIN)*value) + RAD_MIN;
        value = FMath::Clamp(value, RAD_MIN, RAD_MAX);
        if (GetINAPawn() && GetINAPawn()->GetShellComponent() == selectedShell) {
            GetINAPawn()->GetCameraRotatorArm()->TargetArmLength = value + 15.f;
        }
        selectedShell->SetRadius(value);
    }
}

void AINA_PawnController::SetShellOpacity(float value) {
    if (selectedShell) {
        selectedShell->SetOpacity(value);
    }
}

void AINA_PawnController::SetShellRefractionIndex(float value) {
    if (selectedShell) {
        value = ((RI_MAX - RI_MIN)*value) + RI_MIN;
        value = FMath::Clamp(value, RI_MIN, RI_MAX);
        selectedShell->SetRefractionIndex(value);
    }
}

void AINA_PawnController::SetBeamLength(float value) {
    if (GetINAPawn()) {
        auto props = LaserManager::get().GetLaserProperties(GetINAPawn());
        props.Length = value;
        LaserManager::get().SetLaserProperties(GetINAPawn(), props);
    }
}

void AINA_PawnController::SetBeamWidth(float value) {
    if (GetINAPawn()) {
        auto props = LaserManager::get().GetLaserProperties(GetINAPawn());
        props.Width = IndraMath::RemapRange(value, 0.f, 1.f, 0.3f, 2.5f);
        LaserManager::get().SetLaserProperties(GetINAPawn(), props);
    }
}

void AINA_PawnController::SetBeamAngle(float value) {
    if (GetINAPawn()) {
        auto props = LaserManager::get().GetLaserProperties(GetINAPawn());
        props.Angle = value;
        LaserManager::get().SetLaserProperties(GetINAPawn(), props);
    }
}

void AINA_PawnController::SetBeamSoftness(float value) {
    if (GetINAPawn()) {
        auto props = LaserManager::get().GetLaserProperties(GetINAPawn());
        float real_width = props.WidthScalar * props.Width * IndraMath::MaterialBeamWidth(0.9f, props.Softness);
        props.Softness = value;
        props.WidthScalar = real_width / (props.Width * IndraMath::MaterialBeamWidth(0.9f, props.Softness));
        LaserManager::get().SetLaserProperties(GetINAPawn(), props);
    }
}

void AINA_PawnController::SetBeamIntensity(float value) {
    if (GetINAPawn()) {
        auto props = LaserManager::get().GetLaserProperties(GetINAPawn());
        props.Intensity = value;
        LaserManager::get().SetLaserProperties(GetINAPawn(), props);
    }
}

float AINA_PawnController::GetBeamLength() const {
    if (GetINAPawn()) {
        return LaserManager::get().GetLaserProperties(GetINAPawn()).Length;
    }
    return -1.f;
}

float AINA_PawnController::GetBeamWidth() const {
    if (GetINAPawn()) {
        return LaserManager::get().GetLaserProperties(GetINAPawn()).Width;
    }
    return -1.f;
}

float AINA_PawnController::GetBeamAngle() const {
    if (GetINAPawn()) {
        return LaserManager::get().GetLaserProperties(GetINAPawn()).Angle;
    }
    return -1.f;
}

float AINA_PawnController::GetBeamSoftness() const {
    if (GetINAPawn()) {
        return LaserManager::get().GetLaserProperties(GetINAPawn()).Softness;
    }
    return -1.f;
}

float AINA_PawnController::GetBeamIntensity() const {
    if (GetINAPawn()) {
        return LaserManager::get().GetLaserProperties(GetINAPawn()).Intensity;
    }
    return -1.f;
}

#pragma endregion