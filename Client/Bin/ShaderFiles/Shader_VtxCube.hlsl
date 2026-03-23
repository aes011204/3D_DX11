#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;


//float g_Dark = 0.f;



sampler DefaultSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
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
    
    //Out.vColor = float4(In.vTexcoord.y, In.vTexcoord.y, In.vTexcoord.y, 1.f);
    Out.vColor = g_Texture.Sample(DefaultSampler, In.vTexcoord);
    //Out.vColor.gb = Out.vColor.r;

   // Out.vColor.rgb *= lerp(1.0, 0.f, g_Dark);

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
        PixelShader = compile ps_5_0 PS_MAIN();
    }

}