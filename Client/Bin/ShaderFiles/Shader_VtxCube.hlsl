#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_NoiseTexture;
texture2D g_DistortaionTexture;


float g_Acc = 0.f;
float3 g_vSkyBottomColor = float3(0.65f, 0.78f, 0.85f);
float3 g_vSkyTopColor = float3(0.18f, 0.52f, 0.82f);


float g_fTOD01;

float3 g_SkyColor;
float3 g_SkyColorOrigin;
//
//float2 g_NightT;
//float2 g_MidNightT;
//float2 g_DayT;
//float2 g_SunsetT;
//float2 g_AfterSunsetT;
//float2 g_NightT2;
//
//// 색상 데이터 (Time Colors)
//float3 g_vNightColor;
//float3 g_vMidNightColor;
//float3 g_vDayColor;
//float3 g_vSunsetColor;
//float3 g_vAfterSunsetColor;



sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float3 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

float hash(float2 p)
{
    p = frac(p * 0.3183099f + float2(0.1f, 0.1f));
    p *= 17.0f;
    return frac(p.x * p.y * (p.x + p.y));
}

float2 hash2
    (
    float2 p)
{
    return float2(hash(p), hash(p + 13.37f));
}
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    

  
    float3 vDir = normalize(In.vTexcoord);

// Sky
    float2 vXZ = normalize(vDir.xz);
    float fDist = acos(saturate(vDir.y)) / 1.570796f;
    float2 vSkyUV = vXZ * fDist * 0.5f + 0.5f;
    float4 skyTex = g_Texture.Sample(DefaultSampler, vSkyUV);

// Planar
    //float fPlanarDiv = max(vDir.y, 0.2f);
    //float2 baseUV = vDir.xz / fPlanarDiv;
    float fPlanarDiv = max(vDir.y, 0.1f); // 0.2 → 0.5
    float2 baseUV = vDir.xz / fPlanarDiv;
// 방향5

    float2 dir = normalize(float2(1.0f, 0.2f));
    float2 move = dir * g_Acc * 0.0025f;


// 도메인 워핑
    float2 warp1 = g_NoiseTexture.Sample(DefaultSampler, baseUV * 0.1f).rg - 0.5f;
    float2 warp2 = g_NoiseTexture.Sample(DefaultSampler, baseUV * 0.2f).rg - 0.5f;

    float2 warpedUV = baseUV + warp1 * 0.1f + warp2 * 0.05f;
    warpedUV += move * 0.05f;


// Noise 

    float2 rand1 = hash2(float2(12.3f, 45.6f)) * 200.0f;
    float2 rand2 = hash2(float2(78.9f, 11.2f)) * 200.0f;
    float2 rand3 = hash2(float2(33.3f, 99.9f)) * 200.0f;

    float n1 = g_NoiseTexture.Sample(DefaultSampler, warpedUV * 0.03f + move + rand1).r;
    float n2 = g_NoiseTexture.Sample(DefaultSampler, warpedUV * 0.04f + move + rand2).r;
    float n3 = g_NoiseTexture.Sample(DefaultSampler, warpedUV * 0.02f + move + rand3).r;

// 경계

    float c1 = smoothstep(0.58f, 0.64f, n1) * n1;
    float c2 = smoothstep(0.6f, 0.66f, n2) * n2;
    float c3 = smoothstep(0.57f, 0.63f, n3) * n3;

// contrast
    c1 = pow(c1, 1.3f);
    c2 = pow(c2, 1.3f);
    c3 = pow(c3, 1.3f);


// Combine

    float cloud = max(c1, max(c2, c3)) * 0.5f + (c1 + c2 + c3) * 0.5f;
    cloud = saturate(cloud);

// 투명 유지
    cloud *= 0.75f;

// 지평선
    float horizonMask = saturate(1.0f - vDir.y * 1.f);
    cloud *= lerp(0.7f, 1.f, horizonMask);


    float t = g_fTOD01 ;

    float stepped = floor(skyTex.g * 4.0f) / 4.0f;
    float3 skyColor = lerp(g_vSkyBottomColor, g_vSkyTopColor, stepped);
    skyColor = lerp(skyColor, float3(0.7f, 0.75f, 0.8f), 0.2f);

// Cloud Color
    //float3 cloudColor = float3(0.85f, 0.87f, 0.9f);
    
//// Final
//    float3 finalSky = lerp(skyColor,g_SkyColor, 0.7f);
//    float3 finalColor = lerp(finalSky, cloudColor, cloud);
//
//    Out.vColor = float4(finalColor, 1.0f);
//



// 낮 비율
    float dayFactor = smoothstep(0.25f, 0.6f, t);


    float cycle = 1.0f - abs(t * 2.0f - 1.0f); 
   // float3 finalSky = lerp(skyColor, g_SkyColor, dayFactor);
    float3 finalSky = lerp(g_SkyColor, skyColor, cycle);


    float brightness = lerp(0.2f, 1.0f, cycle);
    finalSky *= brightness;
    float3 cloudColor = lerp(float3(0.2f, 0.2f, 0.25f), float3(0.85f, 0.87f, 0.9f), brightness);

// 구름
    float3 finalColor = lerp(finalSky, cloudColor, cloud);




    float heightMask = saturate(1.0f - vDir.y);
 
// 범위 줄이기
    float fogMask = smoothstep(0.6f, 1.0f, heightMask);
 
//  기존 finalColor 유지하면서 덮기
    float3 fogColor = lerp(g_SkyColorOrigin, float3(0.7, 0.7, 0.7), 0.3);
    finalColor = lerp(finalColor, fogColor, fogMask);
 
    Out.vColor = float4(finalColor, 1.f);
   
	return Out;
}



// 이 코드는 가능하면 젤 밑으로
technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetRasterizerState(RS_Cull_CW);
        SetDepthStencilState(DSS_None,0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);


        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}