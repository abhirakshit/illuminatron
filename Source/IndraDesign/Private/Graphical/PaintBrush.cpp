// Fill out your copyright notice in the Description page of Project Settings.

#include "PaintBrush.h"
#include "icore.h"


UPaintBrush::UPaintBrush()
{
    static const uint8 Diameter = RADIUS * 2;
    m_BrushPixels.Reserve( Diameter * Diameter );
    m_BrushTexture = UTexture2D::CreateTransient( Diameter, Diameter );
    m_BrushTexture->UpdateResource();
}

UPaintBrush::~UPaintBrush()
{
    m_BrushPixels.Empty();
    //m_BrushTexture->BeginDestroy();
    //todo: perform any necessary cleanup of UPaintBrush
    //m_BrushTexture->FinishDestroy();
}

void UPaintBrush::UpdateBrush( UTexture2D* &texture )
{
    texture = m_BrushTexture;
    m_BrushPixels.Empty();
    static const uint8 w = RADIUS * 2, h = w;
    static FUpdateTextureRegion2D region = FUpdateTextureRegion2D( 0, 0, 0, 0, w, h );
    auto cleanupFunc = []( uint8 *SrcData, const FUpdateTextureRegion2D *Regions ) {};

    //Create circular brush
    for( uint8 x = 0; x < w; ++x )
    {
        for( uint8 y = 0; y < h; ++y )
        {
            int8 rx = x - RADIUS;
            int8 ry = y - RADIUS;
            double hyp = FMath::Sqrt( rx*rx + ry*ry );
            double coord_radius = hyp / RADIUS;
            FColor pixel;
            if( coord_radius <= 1.0 )
            {
                pixel = FColor( m_BrushColour );
                //todo calculate brush softness correctly
                pixel.A *= FMath::Lerp<float>(1,1-m_BrushSoftness,coord_radius);
            }
            else
            {
                pixel = FColor( 0, 0, 0, 0 );
            }
            m_BrushPixels.Add( pixel );
        }
    }
    m_BrushTexture->UpdateTextureRegions( 0, 1, &region, 4 * w, 4, (uint8*)m_BrushPixels.GetData(), cleanupFunc );
}

void UPaintBrush::UpdateBrushProperties( const FColor colour, const float softness, UTexture2D* &texture )
{
    SetBrushColour( colour );
    SetBrushSoftness( softness );
    UpdateBrush( texture );
}

void UPaintBrush::SetBrushColour( const FColor &colour )
{
    m_BrushColour = colour;
}

void UPaintBrush::SetBrushSoftness( const float &softness )
{
    m_BrushSoftness = FMath::Clamp<float>( softness, 0.25, 1 );
}

void UPaintBrush::GetBrushTexture( UTexture2D* &texture )
{
    texture = m_BrushTexture;
}
