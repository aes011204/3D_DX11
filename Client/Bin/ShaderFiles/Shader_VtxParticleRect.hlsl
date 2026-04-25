#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;


vector g_vCamPosition;
sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    float2 vLifeTime : TEXCOORD5;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;

};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    float3 vWorldPos = In.vTranslation.xyz;

    float fScaleX = length(In.vRight.xyz);
    float fScaleY = length(In.vUp.xyz);


    float3 vLook = g_vCamPosition.xyz - vWorldPos;


    vLook.y = 0.f;
    vLook = normalize(vLook);


    float3 vUp = float3(0.f, 1.f, 0.f);


    float3 vRight = normalize(cross(vUp, vLook));


    vLook = normalize(cross(vRight, vUp));

    float4x4 BillboardMatrix = float4x4(
    float4(vRight * fScaleX, 0.f),
    float4(vUp * fScaleY, 0.f),
    float4(vLook, 0.f),
    float4(vWorldPos, 1.f)
);
    float4 vPosition = mul(float4(In.vPosition, 1.f), BillboardMatrix);

    float4x4 matWVP = mul(mul(g_WorldMatrix, g_ViewMatrix), g_ProjMatrix);
    Out.vPosition = mul(vPosition, matWVP);

    Out.vTexcoord = In.vTexcoord;
    Out.vLifeTime = In.vLifeTime;

    return Out;
//    VS_OUT Out;
//
//    float4x4 TransformMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
//
//    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
//
//
//
//
//    /////
//    float4x4 matWV, matWVP;
//    
//    matWV = mul(g_WorldMatrix, g_ViewMatrix);
//    matWVP = mul(matWV, g_ProjMatrix);
//    
//    Out.vPosition = mul(vPosition, matWVP);
//    Out.vTexcoord = In.vTexcoord;
//    Out.vLifeTime = In.vLifeTime;
//    return Out;

}

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    //Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.3f)
        discard;
    
   // Out.vColor.rgb = In.vLifeTime.y * 0.3f;
    Out.vColor.rgb *= 0.8f;

    float alpha = lerp(0.8f, 0.f, 1 - (In.vLifeTime.x - In.vLifeTime.y));
    Out.vColor.a = alpha;
;
    
    return Out;

}

// 이 코드는 가능하면 젤 밑으로
technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetRasterizerState(RS_Cull_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
  
}