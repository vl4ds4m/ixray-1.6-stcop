#include "stdafx.h"

#include "base_lighting.h"
#include "xrHardwareLight.h"

void	base_lighting::select(xr_vector<R_Light>& dest, xr_vector<R_Light>& src, Fvector& P, float R) {
    Fsphere		Sphere;
    Sphere.set(P, R);
    dest.clear();
    R_Light* L = &*src.begin();
    for (; L != &*src.end(); L++)
    {
        if (L->type == LT_POINT) {
            float dist = Sphere.P.distance_to(L->position);
            if (dist > (Sphere.R + L->range))	continue;
        }
        dest.push_back(*L);
    }
}
void	base_lighting::select(base_lighting& from, Fvector& P, float R) {
    if (xrHardwareLight::IsEnabled())
    {
        select(rgbIndexes, from.rgb, P, R);
        select(hemiIndexes, from.rgb, P, R);
        select(sunIndexes, from.rgb, P, R);
    }
    else
    {
        select(rgb, from.rgb, P, R);
        select(hemi, from.hemi, P, R);
        select(sun, from.sun, P, R);
    }
}

void base_lighting::select(xr_vector<DWORD>& dest, xr_vector<R_Light>& src, Fvector& P, float R) {
    Fsphere		Sphere;
    Sphere.set(P, R);
    dest.clear();

    for (int i = 0; i < src.size(); ++i)
    {
        R_Light& LightObj = src[i];
        if (LightObj.type == LT_POINT) {
            float dist = Sphere.P.distance_to(LightObj.position);
            if (dist > (Sphere.R + LightObj.range))	continue;
        }

        dest.push_back(i);
    }
}