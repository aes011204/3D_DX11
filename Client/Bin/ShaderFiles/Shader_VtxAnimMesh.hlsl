
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
//texture2D g_Texture;

vector g_vCamPosition;

// 재질 정보
texture2D g_DiffuseTexture;
vector g_vMtrlAmbient = vector(0.3f, 0.3f, 0.3f, 1); // 주변광 반응 정도
vector g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f); //하이라이트 강도

// 빛정보 (빛색, 세기 등)
vector g_vLightDir;

vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

// 본
matrix g_BoneMatrices[512];


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

    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
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
    vector vColor : SV_Target0;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out;
    
    float4x4 matWV, matWVP;

    float fWeightW = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);

    matrix BoneMatrix = g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
    g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
    g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
    g_BoneMatrices[In.vBlendIndex.w] * fWeightW;

    vector vPosition = mul(float4(In.vPosition, 1.f), BoneMatrix);




    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f),g_WorldMatrix)); //받아온 노말은 지역이라 월드좌표로 차원맞춰줘야함. 노말라이즈는 픽셀 쉐이더 에서 하는것보다 여기서 하는게 성능상 이점
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix); // 나중 계산을 위해 z 나누기, 뷰,투영 없는 거 저장 
    
    
    return Out;
}

/* w나누기연산을 수행한다.-> 이 연산으로 이어질수 있는 이유 -> VS_OUT구조체의 위치 -> SV_ */
/* 뷰포트(윈도우좌표)로 변환한다. */
/* 래스터라이즈 -> 정점 세개로 감싸진 영역의 픽셀 정보를 생성한다 */

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexcoord);
    //빛의 크기
    vector vShader = saturate(max(dot(normalize(g_vLightDir) * -1, In.vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    //dot(nomalize한 빛의 방향의 반대방향, 노멀라이즈 한 노멀) = 세타각 을 알수 있음 // 사인그래프를 그린다 왜냐 그게 더 자연스럽거든
    // -1*빛의 방향과 노멀의 각의 차이가 +-90이상 이면 0으로 : 왜냐 안보이니까 빛은 0 (없다, 안보인다),0~+-90 까지는 0 젤 밝음 ~점점 어두워짐 
    // 근데 빛을 직접적으로 안받는다고 0 이면 검은색 이 어색함 -> + g_vLightAmbient(엠비언트 강도) * g_vMtrlAmbient(엠비언트 색) 을 통해 보정
    // 더하면  1~0 보다 초과 될수 있으니saturate() 로 범위 1~0로 제한
    
    // 스팩큘러 (물체에 반사된 빛이 카메라에 들어오는 정도) 빛의 반사된 방향백터,카메라의 look
    vector vLook = In.vWorldPos - g_vCamPosition;
    vector vRelfect = reflect(normalize(g_vLightDir), In.vNormal);
    //dot(normalize(g_vLightDir), normalize(In.vNormal))*normalize(In.vNormal)???? 모르겠음 낼 질문
    float vSpecular = pow(max(dot(normalize(vLook) * -1, normalize(vRelfect)), 0.f), 100.f);
    
    vector vSpecularColor = g_vLightSpecular * g_vMtrlSpecular * vSpecular;
    
    Out.vColor = g_vLightDiffuse * vMtrlDiffuse * vShader + vSpecularColor;
    // 빛의 색 * 텍스쳐의 색 * 빛의 크기 계산한것 + 하이라이트??
    
    return Out;
}

technique11 DefaultTechnique
{
    pass DefaultTechnique
    {
        SetDepthStencilState(Depth_Enable, 0);

        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}