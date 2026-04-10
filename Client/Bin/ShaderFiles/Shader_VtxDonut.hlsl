#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

//float g_Dark = 0.f;
float g_Alpha= 1.f;

struct Zone
{
    // 0~1
    float start; 
    float end;
    float padding1;
    float padding2;
};
Zone g_zones[8];
float g_zoneCount = 0;

float4 g_BaseColor;
float4 g_ZoonColor;

float g_AccTime;

//float4 g_ColorFlat = { 0.f, 0.f, 0.f, 0.f };
//float4 g_ColorMix = { 0.f, 0.f, 0.f, 0.f };


sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};


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

    float4 vTextureColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    if (vTextureColor.a <= 0.1)
        discard;
    

    // 0~1 범위를 -1~1 사이로 바꾸기
    float2 centerUV = In.vTexcoord * 2.f - 1.f;

    float dist = sqrt(centerUV.x * centerUV.x + centerUV.y * centerUV.y);

   float angle = atan2(-centerUV.x, centerUV.y); // -pi ~ pi 사이 
   // float angle = atan2(centerUV.y, centerUV.x);
    //float angle01 = (angle / PI2) + 0.5f;
    float angle01 = (angle + PI) / PI2;

    bool InZoon = false;
    for (int i = 0; i < g_zoneCount;i++)
    {
        if (g_zones[i].start <= angle01 && g_zones[i].end >= angle01)
        {
            InZoon = true;
            break;

        }

    }
    if (length(centerUV) < 0.02)
    {
        Out.vColor = float4(1, 0, 0, 1);
        return Out;
    }


    Out.vColor = lerp(g_BaseColor, g_ZoonColor, InZoon);
    Out.vColor.a = 1.f;

    ////Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //float4 vTextureColor = g_Texture.Sample(DefaultSampler,In.vTexcoord);
    //float4 vFinalRGBA = vTextureColor.rgba;
    ////vFinalRGBA.a = g_Alpha;
    //vFinalRGBA.a = vTextureColor.a * g_Alpha;
    //
    //vFinalRGBA = lerp(vFinalRGBA, g_ColorFlat.rgba, g_ColorFlat.a);
    //float3 vFinalRGB = vFinalRGBA.rgb;
    //vFinalRGB *= lerp(1.0, 0.f, g_Dark);
    //vFinalRGB = lerp(vFinalRGB, g_ColorMix.rgb, g_ColorMix.a);
    //Out.vColor = float4(vFinalRGB, vFinalRGBA.a);


    return Out;

}





// 이 코드는 가능하면 젤 밑으로
technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
   // pass ProgressDount
   // {
   //     SetRasterizerState(RS_Default);
   //     SetDepthStencilState(DSS_Default, 0);
   //     SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
   //
   //     VertexShader = compile vs_5_0 VS_MAIN();
   //     PixelShader = compile ps_5_0 PS_MAIN();
   // }
}