// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interfaces/Refractable.h"
#include "ShellComponent.generated.h"


class UProceduralMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class INDRADESIGN_API UShellComponent : public USphereComponent, public IRefractable
{
    GENERATED_BODY()
    friend class AINA_PawnBase;

public:
    UPROPERTY(Category = ".INA|Shell|Properties",EditAnywhere,BlueprintReadWrite)
    uint8 ID;
    UPROPERTY(Category = ".INA|Shell|Properties",EditAnywhere,BlueprintReadWrite)
    uint8 depth;

	static int shell_count;
    UPROPERTY(Category = ".INA|Shell|Components", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    UProceduralMeshComponent* MeshComponent;
    UPROPERTY(Category = ".INA|Shell|Components", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), VisibleInstanceOnly)
    TArray<UShellComponent*> InnerShells;

    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadOnly)
    UMaterialInstanceDynamic* Material;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ClampMax = 9), VisibleAnywhere)
    uint8 Subdivisions = 0;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), EditAnywhere)
    float Radius = 25.f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ZeroToOne), EditAnywhere)
    float Noise = 0.f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ZeroToOne), EditAnywhere)
    float LerpRate = 0.5f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ZeroToOne), EditAnywhere)
    float Opacity = 0.5f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ZeroToOne), EditAnywhere)
    float TargetOpacity = 0.5f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true", ZeroToOne), EditAnywhere)
    float Reflectivity = 1.0f;
    UPROPERTY(Category = ".INA|Shell|Properties", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), EditAnywhere)
    float RefractionIndex = 1.33f;

protected:
	virtual void BeginPlay() override;

    UFUNCTION(Category = ".INA|Shell|Methods",BlueprintCallable)
    void CreateMeshSection();
    UFUNCTION(Category = ".INA|Shell|Methods", BlueprintCallable)
    void UpdateMeshSection();
    UFUNCTION(Category = ".INA|Shell|Methods",BlueprintCallable)
    void SetMeshMaterial(UMaterialInstanceDynamic* material_instance);

public:
	UShellComponent();
    UFUNCTION(Category = ".INA|Shell|Methods", BlueprintCallable)
	void CreateRuntimeComponents();

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


#pragma region "Property Interface"
    void SetRadius(float value);
    void SetOpacity(float value);
    void SetVertexNoise(float power);
    void SetRefractionIndex(float value);

    float GetRadius() const { return Radius; }
    float GetOpacity() const { return Opacity; }
    float GetVertexNoise() const { return Noise; }
    float GetRefractionIndex() const { return RefractionIndex; }
#pragma endregion


    UFUNCTION(Category = ".INA|Shell|Methods",BlueprintCallable)
    UShellComponent* AddShell();
    bool RemoveShell(uint8 index);
    UFUNCTION(Category = ".INA|Shell|Methods",BlueprintCallable)
    bool RemoveShell(UShellComponent* shell);
};
