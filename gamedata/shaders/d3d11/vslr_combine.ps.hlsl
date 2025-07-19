#include "common.hlsli"
#include "reflections.hlsli"

TextureCube s_cube;
TextureCube s_cube_depth;

struct PSInput
{
    float4 hpos : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

uniform float3x4 m_View;

float4 main(PSInput I, out float envDepth : SV_DEPTH) : SV_Target0
{
    // float3 View = float3(I.texcoord.xy * 2.0f - 1.0f, 1.0f);
	// View.xy *= float2(1.0f, -1.0f);
	
	float3 View = -NormalDecode(I.texcoord);
	
	float3 World = mul((float3x3)m_invV, View);
	// View = mul((float3x3)m_View, World);
	
	float2 rotation = 0.0f;
	sincos(L_sky_color.w, rotation.x, rotation.y);
	World.xz = float2(World.x * rotation.y - World.z * rotation.x, World.x * rotation.x + World.z * rotation.y);
	
#ifndef USE_FULL_SKY_SPHERE
	RemapVector(World);
#endif
	
	float3 s0 = sky_s0.SampleLevel(smp_rtlinear, World, 0).xyz;
	float3 s1 = sky_s1.SampleLevel(smp_rtlinear, World, 0).xyz;
	float3 Env = PushGamma(L_sky_color.xyz * lerp(s0, s1, L_ambient.w));
	
	float3 Color = s_cube.SampleLevel(smp_rtlinear, View, 0.0f).xyz;
	envDepth = s_cube_depth.SampleLevel(smp_rtlinear, View, 0.0f).x;
	View = cubemap_depth_to_vector(View, envDepth);
	
	float ViewDist = length(View);
	
    float Fog = PushGamma(saturate(ViewDist * fog_params.w + fog_params.x));
	
	Color = lerp(Color, Env, Fog);
	Color *= rcp(1.0f + Color);
	
	// envDepth = saturate(depth_unpack.x * rcp(ViewDist) + depth_unpack.y);
	
	return float4(Color, 1.0f);
}

