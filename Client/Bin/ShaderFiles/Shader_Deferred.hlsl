#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4 g_ViewMatrixInverse, g_ProjMatrixInverse;

Texture2D g_Texture;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ShadeTexture;
Texture2D g_EmissiveTexture;
//texture2D g_SpecularTexture;
Texture2D g_DepthTexture;

vector g_vLightDir;
vector g_vLightPos;
float g_fLightRange;
vector g_vLightDiffuse;
vector g_vLightAmbient;
//vector g_vLightSpecular;

vector g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
//vector g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

vector g_vCamPosition;

float g_Far;
float3 g_SkyColor;


sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
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

struct PS_OUT_BACKBUFFER
{
    vector vColor : SV_TARGET0;
};


struct PS_OUT_LIGHT
{
    vector vShade : SV_TARGET0;
    vector vSpecular : SV_TARGET1;
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

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT_BACKBUFFER PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
   
  
    return Out;
}



PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out;

    
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    //vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    //float fViewZ = vDepthDesc.y * g_Far;
    //
    //float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
    //
    //float4 vWorldPos;
    //
    ////투영공간상의 위치를 구한다. 
    ////월드위치 * 뷰행렬 * 투영행렬 / w 
    //vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    //vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    //vWorldPos.z = vDepthDesc.x;
    //vWorldPos.w = 1.f;
    //
    //// 뷰스페이스 상의 위치를 구한다 
    //// 월드위치 * 뷰행렬 * 투영행렬  
    //vWorldPos *= fViewZ;
    //
    //// 월드위치 * 뷰행렬 
    //vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    //vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
    //float4 vLook = vWorldPos - g_vCamPosition;
    //
    //float fSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vReflect)), 0.f), 50.f);

    Out.vShade = g_vLightDiffuse * (max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));


    return Out;

}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out;
    vector vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_Far;
    
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);
    
    float4 vWorldPos;
    
    //투영공간상의 위치를 구한다. 
    //월드위치 * 뷰행렬 * 투영행렬 / w 
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    // 뷰스페이스 상의 위치를 구한다 
    // 월드위치 * 뷰행렬 * 투영행렬  
    vWorldPos *= fViewZ;
    
    // 월드위치 * 뷰행렬 
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

    vector vLightDir = g_vLightPos - vWorldPos;
    float fDistance = length(vLightDir);

    vLightDir = normalize(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade =
    (g_vLightDiffuse *
     (max(dot(vLightDir, normalize(vNormal)), 0.f)
     + (g_vLightAmbient * g_vMtrlAmbient)))
    * fAtt;
    return Out;
}


PS_OUT_BACKBUFFER PS_MAIN_COMBINED(PS_IN In)
{
    PS_OUT_BACKBUFFER Out;
    vector vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;

    vector vShade = g_ShadeTexture.Sample(DefaultSampler, In.vTexcoord);

    vector vEmissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord);

    vector finalColor= vDiffuse * vShade + vEmissive;
    /////역산 //////
    vector vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * g_Far;
    
    float4 vWorldPos;
  
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos *= fViewZ;
 
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

    vector vDir = g_vCamPosition - vWorldPos;
    float fDistance = length(vDir);

    //////안개///////////
    float fogDst = smoothstep(50.f, 300.f, fDistance);

    float heightFog = saturate((vWorldPos.y + 2.0) /40.0);
    heightFog = 1.0 - heightFog;
    float fogFactor = fogDst * heightFog;

    fogFactor = pow(fogFactor, 1.5);
    float3 fogColor = lerp(g_SkyColor, float3(0.7, 0.7, 0.7), 0.3);
  
    Out.vColor = float4(lerp(finalColor, fogColor, fogFactor), 1.f);
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Combined
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN_COMBINED();
    }
}