#include "ShellComponent.h"
#include "ProceduralMeshComponent.h"
#include "LaserManager.h"
#include "Laser.h"
#include "INA_PawnController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/CollisionProfile.h"
#include "UObject/UObjectGlobals.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({ {2,2},{5,6} });/*comment these two lines to disable debug lines*/
//debugEvent(1, VeryVerbose, "ticking");
//debugInfo(2, IndraInit, "MeshComponent was constructed (%s)", *MeshComponent->GetName());
//debugVerbose(3, IndraGeometry, "mesh component can render: %d", MeshComponent->CanEverRender());
//debugVerbose(3, IndraGeometry, "mesh component should render: %d", MeshComponent->ShouldRender());
//debugVerbose(3, IndraGeometry, "mesh component's render state exists: %d", MeshComponent->IsRenderStateCreated());
//debugVerbose(4, IndraGeometry, "mesh component's reported vertices: %d", MeshComponent->GetProcMeshSection(0)->ProcVertexBuffer.Num());
//debugVerbose(4, IndraGeometry, "mesh component's reported indices: %d", MeshComponent->GetProcMeshSection(0)->ProcIndexBuffer.Num());
//debugVerbose(3, IndraGeometry, "mesh component can render: %d", MeshComponent->CanEverRender());
//debugVerbose(3, IndraGeometry, "mesh component should render: %d", MeshComponent->ShouldRender());
//debugVerbose(3, IndraGeometry, "mesh component's render state exists: %d", MeshComponent->IsRenderStateCreated());
//debugVerbose(4, IndraGeometry, "mesh component's reported vertices: %d", MeshComponent->GetProcMeshSection(0)->ProcVertexBuffer.Num());
//debugVerbose(4, IndraGeometry, "mesh component's reported indices: %d", MeshComponent->GetProcMeshSection(0)->ProcIndexBuffer.Num());
//debugInfo(5, IndraGeometry, "new radius = %f", value);
//debugInfo(6, IndraGeometry, "power = %f", power);
//debugInfo(7, IndraGeometry, "outter shell pos = [%s]", *this->GetComponentLocation().ToString());
//debugInfo(7, IndraGeometry, "inner shell pos = [%s]", *p->GetComponentLocation().ToString());
//EndDebugBlock

#define USE_SETUPATTACH
//#define USE_RELATIVE

#include "icosphere.h"
icosphere unitsphere[10];
float epsilon = 0.000015f;

int UShellComponent::shell_count = 0;

//Initialization
UShellComponent::UShellComponent() {
    LOGINIT();
    depth = 0;
    PrimaryComponentTick.bCanEverTick = true;
    SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    InitSphereRadius(1.1f);
    SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
    CanCharacterStepUpOn = ECB_No;
    SetShouldUpdatePhysicsVolume(true);
    //FName mesh(*Debug::sprintf(L"ShellMesh_%d", shell_count));
}

void UShellComponent::BeginPlay() {
    LOGINIT();
    Super::BeginPlay();
    //CreateRuntimeComponents();
}

void UShellComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction) {
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (Material) {
        debugEvent(1, VeryVerbose, "ticking");
        Opacity = FMath::Lerp(Opacity, TargetOpacity, DeltaTime * LerpRate);
        Material->SetScalarParameterValue(TEXT("Opacity"), Opacity);
    }
}

//Mesh
void UShellComponent::CreateRuntimeComponents() {
    LOGINIT();
    if (!MeshComponent) {
        FName ComponentName = MakeUniqueObjectName(this, UProceduralMeshComponent::StaticClass(), TEXT("ShellMesh"));
        MeshComponent = NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), ComponentName);
        if (MeshComponent) {
            debugInfo(2, IndraInit, "MeshComponent was constructed (%s)", *MeshComponent->GetName());
#ifdef USE_SETUPATTACH
            MeshComponent->SetupAttachment(this);
            MeshComponent->RegisterComponent();
            MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
#else
            MeshComponent->SetWorldLocation(GetComponentLocation());
            MeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
#endif
        } else {
            logError(IndraCriticalErrors, "MeshComponent failed to construct it seems");
        }
    } else {
        logWarning(IndraInit, "MeshComponent has already been constructed.");
    }
}

void UShellComponent::CreateMeshSection() {
    LOGEVENT(Info);
    if (MeshComponent) {
        if (MeshComponent->GetNumSections() == 0) {
            static TArray<FVector2D> dummy_uv;
            static TArray<FColor> dummy_color;
            static TArray<FProcMeshTangent> dummy_tangents;

            if (!unitsphere[Subdivisions].ready()) {
                unitsphere[Subdivisions].make_icosphere(Subdivisions);
            }
            unitsphere[Subdivisions].normalize();
            auto normals = FOccluderVertexArray(unitsphere[Subdivisions].get_vertices());
            unitsphere[Subdivisions].multiply(Radius);
            unitsphere[Subdivisions].create_noise(Noise);
            icosphere src(unitsphere[Subdivisions]);
            src.print();
            src.print_vertices(1);
            MeshComponent->CreateMeshSection(0, src.get_vertices(), src.get_indices(), normals, src.get_uvmapping(), dummy_color, dummy_tangents, true);
            debugVerbose(3, IndraGeometry, "mesh component can render: %d", MeshComponent->CanEverRender());
            debugVerbose(3, IndraGeometry, "mesh component should render: %d", MeshComponent->ShouldRender());
            debugVerbose(3, IndraGeometry, "mesh component's render state exists: %d", MeshComponent->IsRenderStateCreated());
            debugVerbose(4, IndraGeometry, "mesh component's reported vertices: %d", MeshComponent->GetProcMeshSection(0)->ProcVertexBuffer.Num());
            debugVerbose(4, IndraGeometry, "mesh component's reported indices: %d", MeshComponent->GetProcMeshSection(0)->ProcIndexBuffer.Num());
        } else {
            logError(IndraGeometry, "A mesh section already exists.");
        }
    } else {
        logWarning(IndraGeometry, "MeshComponent is null. Something may be wrong. Either way we can't create a mesh section.");
    }
}

void UShellComponent::UpdateMeshSection() {
    LOGEVENT(Info);
    if (MeshComponent) {
        if (MeshComponent->GetNumSections() != 0) {
            static TArray<FVector2D> dummy_uv;
            static TArray<FColor> dummy_color;
            static TArray<FProcMeshTangent> dummy_tangents;

            if (!unitsphere[Subdivisions].ready()) {
            }
            unitsphere[Subdivisions].make_icosphere(Subdivisions);
            unitsphere[Subdivisions].normalize();
            auto normals = FOccluderVertexArray(unitsphere[Subdivisions].get_vertices());
            unitsphere[Subdivisions].multiply(Radius);
            unitsphere[Subdivisions].create_noise(Noise);
            icosphere src(unitsphere[Subdivisions]);
            src.print();
            src.print_vertices(1);
            debugVerbose(3, IndraGeometry, "mesh component can render: %d", MeshComponent->CanEverRender());
            debugVerbose(3, IndraGeometry, "mesh component should render: %d", MeshComponent->ShouldRender());
            debugVerbose(3, IndraGeometry, "mesh component's render state exists: %d", MeshComponent->IsRenderStateCreated());
            debugVerbose(4, IndraGeometry, "mesh component's reported vertices: %d", MeshComponent->GetProcMeshSection(0)->ProcVertexBuffer.Num());
            debugVerbose(4, IndraGeometry, "mesh component's reported indices: %d", MeshComponent->GetProcMeshSection(0)->ProcIndexBuffer.Num());
            if (MeshComponent->GetProcMeshSection(0)->ProcVertexBuffer.Num() == normals.Num()) {
                MeshComponent->UpdateMeshSection(0, src.get_vertices(), normals, src.get_uvmapping(), dummy_color, dummy_tangents);
            } else {
                LOGEVENTINFO(Info, "Topology changed, recreating mesh section.");
                MeshComponent->ClearMeshSection(0);
                MeshComponent->CreateMeshSection(0, src.get_vertices(), src.get_indices(), normals, src.get_uvmapping(), dummy_color, dummy_tangents, true);
            }
        } else {
            logError(IndraGeometry, "MeshComponent has no mesh sections to update..");
        }
    } else {
        logError(IndraGeometry, "MeshComponent is null.");
    }
}

void UShellComponent::SetMeshMaterial(UMaterialInstanceDynamic* material_instance) {
    LOGEVENT(Info);
    if (!MeshComponent) {
        logWarning(IndraGeometry, "MeshComponent is null. Something may be wrong.");
        return;
    }
    Material = material_instance;
    MeshComponent->SetMaterial(0, material_instance);
}


#pragma region "Property Interface"

void UShellComponent::SetRadius(float value) {
    value = value >= 0.1f ? value : 0.1f;
    debugInfo(5, IndraGeometry, "new radius = %f", value);
    auto parent = GetAttachParent();
    auto parent_shell = Cast<UShellComponent>(parent);
    if (parent_shell) {
        float max = 0.9f * parent_shell->GetRadius();
        Radius = value <= max ? value : max;
    } else {
        //must be the root shell
        Radius = value;
    }
    for (auto shell : InnerShells) {
        shell->SetRadius(shell->GetRadius());
    }
    InitSphereRadius(Radius);
    if (MeshComponent && (MeshComponent->GetNumSections() != 0)) {
        UpdateMeshSection();
    }
    //SetWorldScale3D(FVector(Radius,Radius,Radius));
}

void UShellComponent::SetOpacity(float value) {
    TargetOpacity = value;
}

void UShellComponent::SetVertexNoise(float power) {
    debugInfo(6, IndraGeometry, "power = %f", power);
    Noise = power;
    if (MeshComponent && (MeshComponent->GetNumSections() != 0)) {
        UpdateMeshSection();
    }
}

void UShellComponent::SetRefractionIndex(float value) {
    RefractionIndex = value;
}

#pragma endregion


UShellComponent* UShellComponent::AddShell() {
    LOGEVENT(Info);
    FName ComponentName = MakeUniqueObjectName(this, UShellComponent::StaticClass(), TEXT("InnerShell"));
    UShellComponent* p = NewObject<UShellComponent>(this, UShellComponent::StaticClass(), ComponentName);
#ifdef USE_SETUPATTACH
    p->SetupAttachment(this);
    p->RegisterComponent();
    p->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
#else
    p->SetWorldLocation(GetComponentLocation());
    p->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
#endif
    p->SetRadius(0.9f);
    debugInfo(7, IndraGeometry, "outter shell pos = [%s]", *this->GetComponentLocation().ToString());
    debugInfo(7, IndraGeometry, "inner shell pos = [%s]", *p->GetComponentLocation().ToString());
    //logInfo(Indra, "InnerShells size: %d", InnerShells.Num());
    InnerShells.Add(p);
    //logInfo(Indra, "InnerShells size: %d", InnerShells.Num());
    return p;
}

bool UShellComponent::RemoveShell(uint8 index) {
    LOGEVENT(Info);
    if (InnerShells.IsValidIndex(index)) {
        InnerShells[index]->DestroyComponent();
        InnerShells.RemoveAt(index);
        return true;
    }
    return false;
}

bool UShellComponent::RemoveShell(UShellComponent * inner) {
    LOGEVENT(Info);
    inner->DestroyComponent();
    return InnerShells.Remove(inner) > 0;
}
