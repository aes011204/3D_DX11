#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//texture2D g_Texture;

vector g_vCamPosition;

// 재질 정보
Texture2D g_DiffuseTexture;
//vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1); // 주변광 반응 정도
//vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f); //하이라이트 강도
//
//// 빛정보 (빛색, 세기 등)
//vector g_vLightDir;
//
//vector g_vLightDiffuse;
//vector g_vLightAmbient;
//vector g_vLightSpecular;
//bool g_EmissivePower;
float g_Far;
Texture2D g_EmissiveTexture;
float g_EmissiveStrength = 1.f;


float g_Alpha=1.f;

DepthStencilState Depth_Enable
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
     vector vDepth : SV_TARGET2;
    vector vEmissive : SV_TARGET3;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    //float3 normal = mul(In.vNormal, (float3x3) g_WorldMatrix);
    //Out.vNormal = float4(normalize(normal), 1.f);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f),g_WorldMatrix)); //받아온 노말은 지역이라 월드좌표로 차원맞춰줘야함. 노말라이즈는 픽셀 쉐이더 에서 하는것보다 여기서 하는게 성능상 이점
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // 나중 계산을 위해 z 나누기, 뷰,투영 없는 거 저장 
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);
  //  Out.vNormal = float4(1, 0, 0, 1);
  //  Out.vNormal = In.vNormal;
    Out.vNormal = float4(normalize(In.vNormal.xyz) * 0.5f + 0.5f, 1.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
    float3 emissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vEmissive = float4(emissive * g_EmissiveStrength, 1.0f);
	return Out;
}
PS_OUT PS_MAINAlpha(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);
    //Out.vNormal = float4(1, 0, 0, 1);
    Out.vNormal = float4(normalize(In.vNormal.xyz) * 0.5f + 0.5f, 1.f);
    Out.vDiffuse.a = vMtrlDiffuse.a * g_Alpha;
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / g_Far, 0.f, 1.f);
    float3 emissive = g_EmissiveTexture.Sample(DefaultSampler, In.vTexcoord).rgb;
    Out.vEmissive = float4(emissive * g_EmissiveStrength, 1.0f);
	return Out;
}

technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DefaultTechniqueAlpha
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAINAlpha();
    }
}