// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "INA_PawnBase.h"
#include "ShellComponent.h"
#include "INA_PawnController.generated.h"

class UShellComponent;

UENUM(BlueprintType)
enum struct ControllerMode : uint8 {
    Movement UMETA(DisplayName="Movement"),
    Placement UMETA(DisplayName="Placement"),
    HUDCursor UMETA(DisplayName="HUDCursor")
};

UCLASS()
class INDRADESIGN_API AINA_PawnController : public APlayerController
{
    GENERATED_BODY()

private:
    AINA_PawnBase* ina_pawn = nullptr;

protected:
    UPROPERTY(Category = ".INA|PawnController", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    ControllerMode Mode = ControllerMode::Movement;
    UPROPERTY() //we want it to be null when/if the component dies
    UShellComponent* selectedShell;

public:
    virtual void BeginPlay() override;
    AINA_PawnBase* GetINAPawn() {
        if (!ina_pawn) {
            ina_pawn = Cast<AINA_PawnBase>(GetPawn());
        }
        return ina_pawn;
    }
    AINA_PawnBase* GetINAPawn() const { return Cast<AINA_PawnBase>(GetPawn()); }

#pragma region "Control Bindings"
    void ZoomCamera(float forward);
    void RotateCameraRight(float right);
    void RotateCameraUp(float up);
    void MovePawnRight(float right);
    void MovePawnForward(float forward);
    void MoveShellRight(float right);
    void MoveShellForward(float forward);
    void MoveShellUp(float up);
#pragma endregion

#pragma region "Property Interface"
    //Interface for use by UMG blueprints, and anything else that will need quick easy access to Beam/Shell properties
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetSelectedShell(UShellComponent* shell);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetShellVertexNoise(float power);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetShellRadius(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetShellOpacity(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetShellRefractionIndex(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetBeamLength(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetBeamWidth(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetBeamAngle(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetBeamSoftness(float value);
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) void SetBeamIntensity(float value);

    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) UShellComponent* GetSelectedShell() const { return selectedShell; }
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetShellVertexNoise() const { return selectedShell ? selectedShell->GetVertexNoise() : -1.f; }
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetShellRadius() const { return selectedShell ? selectedShell->GetRadius() : -1.f; }
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetShellOpacity() const { return selectedShell ? selectedShell->GetOpacity() : -1.f; }
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetShellRefractionIndex() const { return selectedShell ? selectedShell->GetRefractionIndex() : -1.0f; }
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetBeamLength()const;
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetBeamWidth()const;
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetBeamAngle()const;
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetBeamSoftness()const;
    UFUNCTION(Category = ".INA|Controller|Properties", BlueprintCallable) float GetBeamIntensity()const;
#pragma endregion
};
