#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//texture2D g_Texture;

vector g_vCamPosition;

float g_Scale; // 64,128 등의 사이즈

float g_Time;



// 재질 정보
texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1); // 주변광 반응 정도
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f); //하이라이트 강도

// 빛정보 (빛색, 세기 등)
vector g_vLightDir;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

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
   
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    //float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
};

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    //float4 vNormal : NORMAL;
    //float2 vTexcoord : TEXCOORD0;
    //float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

float3 Calculate_GerstnerWave(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time);
float3 Calculate_GerstnerWave_Overlap(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time);


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;

    float4x4 matWV, matWVP;

    float3 scaledPos = In.vPosition;
    scaledPos.x *= g_Scale;
    scaledPos.z *= g_Scale;

    float nextScale = g_Scale * 2.0f;
    float2 connectXZ;

    connectXZ.x = floor((scaledPos.x / nextScale) + 0.5f) * nextScale; 
    connectXZ.y = floor((scaledPos.z / nextScale) + 0.5f) * nextScale;
    // nextScale나눠서 1 단위로 만들고 0.5+후 소숫점 무시 후 다시 곱해서 원래 사이즈로 복구

    float maxPoint = max(abs(In.vPosition.x), abs(In.vPosition.z));
    float alpha = saturate((maxPoint - 24.f) / 8.f); //saturate : 0-1 사이의 값만 남김

    float2 finalXZ = lerp(scaledPos.xz, connectXZ, alpha);

    float3 pos = float3(finalXZ.x, scaledPos.y, finalXZ.y);

    float2 dir1 = float2(1.f, 1.f);
    float2 dir2 = float2(0.f,-0.46f);
    float2 dir3 = float2(-1.f, .3f);

    float3 offset = float3(0, 0, 0);
    offset += Calculate_GerstnerWave_Overlap(pos, dir1, .43f, 17.f, 2.5f, g_Time);
    offset += Calculate_GerstnerWave_Overlap(pos, dir2, 0.21f, 7.4f, 1.8f, g_Time);
    offset += Calculate_GerstnerWave_Overlap(pos, dir3, 0.08f, 2.7f, 1.f, g_Time);

    float3 wavefinal = pos + offset;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(float4(wavefinal, 1.f), matWVP);
   // Out.vTexcoord = In.vTexcoord;
   // Out.vNormal = normalize(mul(float4(In.vNormal, 0.f),g_WorldMatrix)); //받아온 노말은 지역이라 월드좌표로 차원맞춰줘야함. 노말라이즈는 픽셀 쉐이더 에서 하는것보다 여기서 하는게 성능상 이점
   // Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // 나중 계산을 위해 z 나누기, 뷰,투영 없는 거 저장 
    
    
    return Out;
}

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
   // vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
   // //빛의 크기
   // vector vShader = saturate(max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
   // //dot(nomalize한 빛의 방향의 반대방향, 노멀라이즈 한 노멀) = 세타각 을 알수 있음 // 사인그래프를 그린다 왜냐 그게 더 자연스럽거든
   // // -1*빛의 방향과 노멀의 각의 차이가 +-90이상 이면 0으로 : 왜냐 안보이니까 빛은 0 (없다, 안보인다),0~+-90 까지는 0 젤 밝음 ~점점 어두워짐 
   // // 근데 빛을 직접적으로 안받는다고 0 이면 검은색 이 어색함 -> + g_vLightAmbient(엠비언트 강도) * g_vMtrlAmbient(엠비언트 색) 을 통해 보정
   // // 더하면  1~0 보다 초과 될수 있으니saturate() 로 범위 1~0로 제한
   // 
   // // 스팩큘러 (물체에 반사된 빛이 카메라에 들어오는 정도) 빛의 반사된 방향백터,카메라의 look
   // vector vLook = In.vWorldPos - g_vCamPosition;
   // vector vRelfect = reflect(normalize(g_vLightDir), In.vNormal);
   // //dot(normalize(g_vLightDir), normalize(In.vNormal))*normalize(In.vNormal)???? 모르겠음 낼 질문
   // float vSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vRelfect)), 0.f), 100.f);
   // 
   // vector vSpecularColor = g_vLightSpecular * g_vMtrlSpecular * vSpecular;
   // 
   // Out.vColor = g_vLightDiffuse * vMtrlDiffuse * vShader + vSpecularColor;
    // 빛의 색 * 텍스쳐의 색 * 빛의 크기 계산한것 + 하이라이트??
    Out.vColor = float4(0.f, 0.f, 1.f, 1.f);
    return Out;
}

float3 Calculate_GerstnerWave(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed , float time)
{
    waveDir = normalize(waveDir);
    
    float angle = dot(waveDir, Pos.xz) * (1.0f / waveLength) - (speed * time);
    float amplitude = waveHeight;
    float3 finalPos;

    finalPos.y = amplitude * sin(angle);

   
    finalPos.x = waveDir.x * (amplitude * cos(angle)) + Pos.x;
    finalPos.z = waveDir.y * (amplitude * cos(angle)) + Pos.z;

    return finalPos;

}
float3 Calculate_GerstnerWave_Overlap(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time)
{
    waveDir = normalize(waveDir);

    float RadianWaveLength = PI2 / waveLength;


    float angle = dot(waveDir, Pos.xz) * RadianWaveLength - (speed * time);
    float amplitude = waveHeight;


	float3 offset;
    offset.y = amplitude * sin(angle);
    offset.x = waveDir.x * (amplitude * cos(angle));
    offset.z = waveDir.y * (amplitude * cos(angle));

    return offset;

}

float3 Calculate_GerstnerWave_Overlap_Phy(float3 Pos, float2 waveDir, float waveHeight01, float waveLength, float speed, float time)
{
    waveDir = normalize(waveDir);

    float RadianWaveLength = PI2 / waveLength;
    float PhaseSpeed = sqrt(9.8f / RadianWaveLength);
    float angle = RadianWaveLength * (dot(waveDir, Pos.xz) - PhaseSpeed * speed * time);
    float a = waveHeight01 / RadianWaveLength;

    //float angle = dot(waveDir, Pos.xz) * RadianWaveLength - (speed * time);

    float3 offset;
    offset.y = a * sin(angle);
    offset.x = waveDir.x * (a * cos(angle));
    offset.z = waveDir.y * (a * cos(angle));

    return offset;

}


technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetRasterizerState(RS_WireFrame);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}