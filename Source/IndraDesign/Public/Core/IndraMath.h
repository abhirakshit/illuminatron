#pragma once
#include "CoreMinimal.h"

class IndraMath {
public:
    static float RemapRange(float value, float r1_low, float r1_high, float r2_low, float r2_high);
    static FVector Refract(const FVector& v_inc, const FVector& norm_inc, const float& n1, const float& n2);
    static float MaterialBeamWidth(float alpha, float softness); //Calculates width via the two intersections of a beam material with the y and z intersects (ie. softness, alpha)
};