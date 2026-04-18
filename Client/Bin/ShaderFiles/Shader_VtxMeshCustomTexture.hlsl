#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

float g_ColorLamp1;
float g_ColorLamp2;

vector g_SandColor;
vector g_RockColor;
vector g_GrassColor;

// 재질 정보
texture2D g_DiffuseTexture;
//vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1); // 주변광 반응 정도
//vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f); //하이라이트 강도
//
//// 빛정보 (빛색, 세기 등)
//vector g_vLightDir;
//
//vector g_vLightDiffuse;
//vector g_vLightAmbient;
//vector g_vLightSpecular;

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
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;



    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f),g_WorldMatrix)); //받아온 노말은 지역이라 월드좌표로 차원맞춰줘야함. 노말라이즈는 픽셀 쉐이더 에서 하는것보다 여기서 하는게 성능상 이점
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // 나중 계산을 위해 z 나누기, 뷰,투영 없는 거 저장 
    
    
    return Out;
}

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    

    
    float SandGlass = step(g_ColorLamp1, In.vWorldPos.y);
    float4 SandGlassColor = lerp(g_SandColor, g_GrassColor, SandGlass);
    In.vNormal = normalize(In.vNormal);


    // vNormal.y가 크면 하늘을 바라보는면
    float rockUp = step(g_ColorLamp2, In.vNormal.y);
    float4 finalColor = lerp(g_RockColor, SandGlassColor, rockUp);

    In.vTexcoord = finalColor;
    vector vMtrlDiffuse = finalColor;
    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    //빛의 크기
    //vector vShader = saturate(max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    //vector vLook = In.vWorldPos - g_vCamPosition;
    //vector vRelfect = reflect(normalize(g_vLightDir), In.vNormal);
    //float vSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vRelfect)), 0.f), 100.f);
    //vector vSpecularColor = g_vLightSpecular * g_vMtrlSpecular * vSpecular;
    //Out.vColor = g_vLightDiffuse * vMtrlDiffuse * vShader + vSpecularColor;
    Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 1.f);

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
}