#include "IndraMath.h"
#include "icore.h"


float IndraMath::RemapRange(float value, float r1_low, float r1_high, float r2_low, float r2_high) {
    return r2_low + (value - r1_low) * (r2_high - r2_low) / (r1_high - r1_low);
}

//https://stackoverflow.com/a/29758766/1327399 I invert V_i instead of N_i, might be the same result
FVector IndraMath::Refract(const FVector& v_inc, const FVector& norm_inc, const float& n1, const float& n2) {
    float ratio = n1 / n2;
    float cos = FVector::DotProduct(norm_inc, (v_inc * -1));
    /*float cos_angle_i = FVector::DotProduct(norm_inc, (v_inc * -1));
    float angle_i = FMath::Acos(cos_angle_i);/**/
    FVector v_refr = (ratio * v_inc) + (ratio* cos - FMath::Sqrt(1-FMath::Square(ratio)*(1-FMath::Square(cos))))*norm_inc;
    /*float angle_r = FMath::Asin(ratio * FMath::Sin(angle_i));
    float angle_r_mine = FMath::Acos(FVector::DotProduct((norm_inc * -1), v_refr));
    bool snells = FMath::IsNearlyEqual(n1 * FMath::Sin(angle_i), n2 * FMath::Sin(angle_r),0.00001f);
    IN_STATIC_LOG(Indra, Info, false, "%f * %f == %f * %f = %d", n1, FMath::Sin(angle_i), n2, FMath::Sin(angle_r),snells);
    IN_STATIC_LOG(Indra, Info, false, "%f == %f",FMath::RadiansToDegrees(angle_r), FMath::RadiansToDegrees(angle_r_mine));/**/
    return v_refr;
}

float IndraMath::MaterialBeamWidth(float z_plane, float softness) {
    float y = 3.f - (2.f*softness); // [2(1 - softness) + 1]
    y = y * y;
    y = 1 / y; // 1 / (2h + 1)^2
    float sine = FMath::Pow(FMath::Sin(y),2.f/7.f);
    float x_intersect = FMath::FastAsin(sine) / PI;
    float width = FMath::Abs(x_intersect - (PI - x_intersect));
    return width;
}