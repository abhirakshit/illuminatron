// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "PaintBrush.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class INDRADESIGN_API UPaintBrush : public UObject
{
    GENERATED_BODY()

private:
    #define RADIUS 64
    //uint8*       m_BrushPixelsRGBA = nullptr;
    //todo: cast to (uint8*); todo-r2: is this todo necessary?
    TArray<FColor> m_BrushPixels;
    UTexture2D*    m_BrushTexture;
    FColor         m_BrushColour;
    float          m_BrushSoftness;

public:
    UPaintBrush();
    ~UPaintBrush();

    UFUNCTION( BlueprintCallable, Category = "Brush Texture" )
        void UpdateBrush( UTexture2D* &texture );
    UFUNCTION( BlueprintCallable, Category = "Brush Texture" )
        void UpdateBrushProperties( const FColor colour, const float softness, UTexture2D* &texture );
    UFUNCTION( BlueprintCallable, Category = "Brush Texture" )
        void SetBrushColour( const FColor &colour );
    UFUNCTION( BlueprintCallable, Category = "Brush Texture" )
        void SetBrushSoftness( const float &softness );
    UFUNCTION( BlueprintCallable, Category = "Brush Texture" )
        void GetBrushTexture( UTexture2D* &texture );
};
