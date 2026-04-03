#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

//cbuffer CB_UI9Slice : register(b1)
//{
float2 g_TexOriginalSize;
float2 g_UISize;
    
float4 g_PxSliceLRTB;
float2 g_TexCustomSize;
//}
float g_Dark = 0.f;
float g_Alpha= 1.f;


float4 g_ColorFlat = { 0.f, 0.f, 0.f, 0.f };
float4 g_ColorMix = { 0.f, 0.f, 0.f, 0.f };


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
    
    //Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    float4 vTextureColor = g_Texture.Sample(DefaultSampler,In.vTexcoord);
    float4 vFinalRGBA = vTextureColor.rgba;
    //vFinalRGBA.a = g_Alpha;
    vFinalRGBA.a = vTextureColor.a * g_Alpha;

    vFinalRGBA = lerp(vFinalRGBA, g_ColorFlat.rgba, g_ColorFlat.a);
    float3 vFinalRGB = vFinalRGBA.rgb;
    vFinalRGB *= lerp(1.0, 0.f, g_Dark);
    vFinalRGB = lerp(vFinalRGB, g_ColorMix.rgb, g_ColorMix.a);
    Out.vColor = float4(vFinalRGB, vFinalRGBA.a);


    return Out;
    //Out.vColor.a = 0.2;

    //if (vColor.a >= 1.0f)
    //{
    //    Out.vColor = float4(0.f, 0.f, 1.f, 1.f);
    //}
    //else
    //{
    //    // 3. 알파가 조금이라도 있으면(투명하면) '빨간색'을 출력합니다.
    //    Out.vColor = float4(1.f, 0.f, 0.f, 1.f);
    //}
    return Out;
}

float2 UV9Slice(float2 uv);

PS_OUT PS_NINESLICE(PS_IN In)
{
    PS_OUT Out;
    
    float2 uv = UV9Slice(In.vTexcoord);

    float4 vTextureColor = g_Texture.Sample(DefaultSampler, uv);
    float3 vFinalRGB = vTextureColor.rgb;
    vTextureColor.a = vTextureColor.a * g_Alpha;


    vFinalRGB = lerp(vFinalRGB, g_ColorFlat.rgb, g_ColorFlat.a);
    vFinalRGB *= lerp(1.0, 0.f, g_Dark);
    vFinalRGB = lerp(vFinalRGB, g_ColorMix.rgb, g_ColorMix.a);
    Out.vColor = float4(vFinalRGB, vTextureColor.a);


    return Out;
}

float2 UV9Slice(float2 uv)
{
    float2 Out;

    float2 radio = g_TexCustomSize / g_TexOriginalSize;

    float2 TexScaled = g_TexOriginalSize * radio;

    float Border_left = g_PxSliceLRTB.x * radio.x;
    float Border_right = g_PxSliceLRTB.y * radio.x;
    float Border_top = g_PxSliceLRTB.z * radio.y;
    float Border_bottom = g_PxSliceLRTB.w * radio.y;
    
    //지금은  원본 tex기준
    float2 PXUV = uv * g_UISize;
    
    if (PXUV.x <= Border_left)
    {
        Out.x = PXUV.x;
    }
    else if (PXUV.x >= g_UISize.x - Border_right)
    {
        Out.x = TexScaled.x - (g_UISize.x - PXUV.x);
        //전체 길이에서 현재 위치를 빼서 끝점 기준의 거리를 구한 뒤, 그걸 원본 이미지의 끝점에서 다시 빼주
    }
    else
    {
        //float centerWidth = g_TexOriginalSize.x - Border_left - Border_right;
        //Out.x = Border_left + fmod(PXUV.x - Border_left, max(centerWidth, 0.0001f));

        float centerWidth = (TexScaled.x - Border_left - Border_right);
        Out.x = Border_left + fmod((PXUV.x - Border_left) , max(centerWidth, 0.0001f));
    }   
    //--------
    if (PXUV.y <= Border_top)
    {
        Out.y = PXUV.y;
    }
    else if (PXUV.y >= g_UISize.y - Border_bottom)
    {
        Out.y = TexScaled.y - (g_UISize.y - PXUV.y);
        //전체 길이에서 현재 위치를 빼서 끝점 기준의 거리를 구한 뒤, 그걸 원본 이미지의 끝점에서 다시 빼주
    }
    else
    {
        //float centerHight = g_TexOriginalSize.y - Border_top - Border_bottom;
        //Out.y = Border_top + fmod(PXUV.y - Border_top, max(centerHight, 0.0001f));

        float centerHeight = (TexScaled.y - Border_top - Border_bottom);
        Out.y = Border_top + fmod((PXUV.y - Border_top) , max(centerHeight, 0.0001f));
    }

    return Out / TexScaled;

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
        PixelShader = compile ps_5_0 PS_MAIN();
    }
    pass NineSlice
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_NINESLICE();
    }
}