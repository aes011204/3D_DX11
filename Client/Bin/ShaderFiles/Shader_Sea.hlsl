#include "Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

vector g_vCamPosition;

float g_Scale; // 64,128 등의 사이즈

// 나중에  cBuffer(b0) 등으로 묶으면 더 빠름 한번에 보내는거기 떄문

struct Wave_Desc
{
    float2 g_Dir;
    float g_WaveHeight;
    float g_WaveLength;
    float g_Speed;

    float3 Padding; // 4의 배수로 
};

Wave_Desc g_Waves[10];
int g_WaveCount;
float g_Time;
//float g_depthMask01;
float3 g_TerrainPos;
float g_TerrainSize;

texture2D g_TerrainHeight;

float3 g_deepColor;
float3 g_shallowColor;

// 재질 정보
//texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(0.5f, 0.5f, 0.5f, 1); // 주변광 반응 정도
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
    AddressU = Clamp;
    AddressV = Clamp;
};

struct VS_IN
{
    float3 vPosition : POSITION;
   
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION; // SV이거빼고는 바꿔도됨
    float4 vNormal : NORMAL;
    //float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float height01 : TEXCOORD2;
};

struct PS_IN
{

    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    //float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float height01 : TEXCOORD2;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

float3 Calculate_GerstnerWave(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time);
float3 Calculate_GerstnerWave_Overlap(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time/*, inout float3 finalNormal*/);


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    // 매쉬 배수
    float3 scaledPos = In.vPosition;
    scaledPos.x *= g_Scale;
    scaledPos.z *= g_Scale;
    
    //모핑
    float nextScale = g_Scale * 2.0f;
    float2 connectXZ;
    
    connectXZ.x = floor((scaledPos.x / nextScale) + 0.5f) * nextScale;
    connectXZ.y = floor((scaledPos.z / nextScale) + 0.5f) * nextScale;
    // nextScale나눠서 1 단위로 만들고 0.5+후 소숫점 무시 후 다시 곱해서 원래 사이즈로 복구
    
    float maxPoint = max(abs(In.vPosition.x), abs(In.vPosition.z));
    float alpha = saturate((maxPoint - 48.0f) / 8.f); //saturate : 0-1 사이의 값만 남김
    // 이 수치는 바다 크기에 따라 다르게 줘야함0.75
    float2 finalXZ = lerp(scaledPos.xz, connectXZ, alpha);
    
    float3 pos = float3(finalXZ.x, scaledPos.y, finalXZ.y); // pos  로컬 좌표
    ///스냅핑
    float4x4 snappedWorld = g_WorldMatrix;
    float cellsize = 1.f;
    snappedWorld._41 = floor(snappedWorld._41 / cellsize) * cellsize;
    snappedWorld._43 = floor(snappedWorld._43 / cellsize) * cellsize;
    
    float4 worldPos = mul(float4(pos, 1.f), snappedWorld);
    
    Out.vWorldPos = worldPos;
    
// UV 하이트 매ㅐㅂ
        // Terrain
   // g_TerrainPos;
    float relativePosX = worldPos.x - g_TerrainPos.x;
    float relativePosZ = worldPos.z - g_TerrainPos.z;
    
    //float2 UV;
    //UV.x = (relativePosX / g_TerrainSize) + 0.5;
    //UV.y = (-relativePosZ / g_TerrainSize) + 0.5;
    //Out.height01 = g_TerrainHeight.SampleLevel(DefaultSampler, UV, 0).r;
    float2 UV;
    UV.x = (worldPos.x - g_TerrainPos.x) / g_TerrainSize;
    UV.y = 1.0f -((worldPos.z - g_TerrainPos.z) / g_TerrainSize);
    UV = saturate(UV);
    Out.height01 = min(1.f - g_TerrainHeight.SampleLevel(DefaultSampler, UV, 0).r, 0.2f);
    
    float3 offset = float3(0, 0, 0);
    float3 pZ = float3(0, 0, 0);
    float3 pX = float3(0, 0, 0);
    
    for (int i = 0; i < g_WaveCount; i++)
    {
        offset += Calculate_GerstnerWave_Overlap
    	(worldPos, g_Waves[i].g_Dir, g_Waves[i].g_WaveHeight * Out.height01, g_Waves[i].g_WaveLength, g_Waves[i].g_Speed, g_Time);
    
        pZ += Calculate_GerstnerWave_Overlap
    	(worldPos + float4(0.f, 0.f, 0.1f, 0.f), g_Waves[i].g_Dir, g_Waves[i].g_WaveHeight * Out.height01, g_Waves[i].g_WaveLength, g_Waves[i].g_Speed, g_Time);
        pX += Calculate_GerstnerWave_Overlap
    	(worldPos + float4(0.1f, 0.f, 0.f, 0.f), g_Waves[i].g_Dir, g_Waves[i].g_WaveHeight * Out.height01, g_Waves[i].g_WaveLength, g_Waves[i].g_Speed, g_Time);
	    
    }
    
    
    float3 wavefinal = worldPos + offset;
    
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
   // matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(float4(wavefinal, 1.f), matVP);
         // Out.vTexcoord = In.vTexcoord;
         // Out.vNormal = normalize(mul(float4(In.vNormal, 0.f),g_WorldMatrix)); //받아온 노말은 지역이라 월드좌표로 차원맞춰줘야함. 노말라이즈는 픽셀 쉐이더 에서 하는것보다 여기서 하는게 성능상 이점
         // Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // 나중 계산을 위해 z 나누기, 뷰,투영 없는 거 저장 
    
    float3 look = (worldPos + float3(0.0f, 0.0f, 0.1f) + pZ) - wavefinal;
    
    float3 right = (worldPos + float3(0.1f, 0.0f, 0.f) + pX) - wavefinal;
    
    Out.vNormal = float4(normalize(cross(look, right)), 1.f);

   
    ////////////////////////////////////////////////////////////////////////////////
   // // 1. 스케일
   // float3 scaledPos = In.vPosition;
   // scaledPos.x *= g_Scale;
   // scaledPos.z *= g_Scale;
   // 
   // // 2. 모핑
   // float nextScale = g_Scale * 2.0f;
   // float2 connectXZ = floor((scaledPos.xz / nextScale) + 0.5f) * nextScale;
   // float maxPoint = max(abs(In.vPosition.x), abs(In.vPosition.z));
   // float alpha = saturate((maxPoint - 48.0f) / 8.f);
   // float2 finalXZ = lerp(scaledPos.xz, connectXZ, alpha);
   // 
   // // 3. 월드 좌표
   // float4x4 snappedWorld = g_WorldMatrix;
   // snappedWorld._41 = floor(g_WorldMatrix._41);
   // snappedWorld._43 = floor(g_WorldMatrix._43);
   // 
   // float4 worldPos = mul(float4(finalXZ.x, scaledPos.y, finalXZ.y, 1.0f), snappedWorld);
   // 
   // //  4. UV 계산 (수정됨)
   // float2 UV;
   // UV.x = (worldPos.x - g_TerrainPos.x) / g_TerrainSize;
   // UV.y = 1.0f - ((worldPos.z - g_TerrainPos.z) / g_TerrainSize);
   // float h01 = g_TerrainHeight.SampleLevel(DefaultSampler, UV, 0).r;
   // 
   // 
   // // 6. 높이 적용
   // float3 wavefinal = worldPos.xyz;
   // wavefinal.y += h01 * 100.0f - 100.0f;
   // 
   // // 7. 출력
   // float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
   // Out.vPosition = mul(float4(wavefinal, 1.0f), matVP);
   // 
   // //  중요 수정
   // Out.vWorldPos = float4(wavefinal, 1.0f);
   // 
   // Out.height01 = h01;
   // 
   // // 일단 노말 고정 (디버그용)
   // Out.vNormal = float4(0.0f, 1.0f, 0.0f, 0.0f);


    return Out;
}

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
	//float3 waterColorsh = float3(0.05f, 0.2f, 0.4f);
    float3 waterColor = lerp(g_shallowColor, g_deepColor, In.
    height01);

    float Alpha = lerp(0.5f, 1.f, In.height01);



    //vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    //빛의 크기
    vector vShader = saturate(max(dot(normalize(g_vLightDir) * -1, normalize(In.vNormal)), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    
    // 스팩큘러 (물체에 반사된 빛이 카메라에 들어오는 정도) 빛의 반사된 방향백터,카메라의 look
    vector vLook = In.vWorldPos - g_vCamPosition;
    vector vRelfect = reflect(normalize(g_vLightDir), In.vNormal);
    //dot(normalize(g_vLightDir), normalize(In.vNormal))*normalize(In.vNormal)???? 모르겠음 낼 질문
    float vSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vRelfect)), 0.f), 100.f);
    
    vector vSpecularColor = g_vLightSpecular * g_vMtrlSpecular * vSpecular;
    
    float3 finalRGB = g_vLightDiffuse.xyz * waterColor * vShader.xyz + vSpecularColor.xyz;
    // 빛의 색 * 텍스쳐의 색 * 빛의 크기 계산한것 + 하이라이트??
    //Out.vColor = float4(waterColor,1.f);

    // 프레넬
    float3 viewDir = normalize(g_vCamPosition.xyz - In.vWorldPos.xyz);
    float fresnel = pow(1.0f - saturate(dot(viewDir, In.vNormal.xyz)), 5.0f);

    float finalAlpha = saturate(Alpha + vSpecularColor.x + fresnel);
    Out.vColor = float4(finalRGB, finalAlpha);
    //Out.vColor = float4(In.height01.xxx, 1);
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
float3 Calculate_GerstnerWave_Overlap(float3 Pos, float2 waveDir, float waveHeight, float waveLength, float speed, float time/*, inout float3 finalNormal*/)
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
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}