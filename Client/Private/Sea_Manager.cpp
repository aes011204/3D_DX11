#include "Sea_Manager.h"
#include <numbers>
#include "VIBuffer_Terrain.h"
IMPLEMENT_SINGLETON(CSea_Manager)



CSea_Manager::CSea_Manager()
{
}

CSea_Manager::~CSea_Manager()
{
}

HRESULT CSea_Manager::Initialize(/*ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext*/)
{


	m_WaveCount = 3;

	m_waveDesc[0].dir = _float2(.67f, .98f);
	m_waveDesc[0].waveHeight = .43f;
	m_waveDesc[0].waveLength = 17.f;
	m_waveDesc[0].speed = 2.5f;

	m_waveDesc[1].dir = _float2(0.f, -0.46f);
	m_waveDesc[1].waveHeight = 0.21f;
	m_waveDesc[1].waveLength = 7.4f;
	m_waveDesc[1].speed = 1.8f;

	m_waveDesc[2].dir = _float2(-.88f, .3f);
	m_waveDesc[2].waveHeight = 0.08f;
	m_waveDesc[2].waveLength = 2.7f;
	m_waveDesc[2].speed = 1.f;


	return S_OK;

}

void CSea_Manager::Update(float TimeDelta)
{
	m_AccTime += TimeDelta;

}
float CSea_Manager::Calculate_GerstnerWave_Overlap(_float3 Pos)
{
  

    float relativePosX = Pos.x - TerrainPos.x;
    float relativePosZ = Pos.z - TerrainPos.z;

    _float2 UV;
    UV.x = (relativePosX / TerrainSize) + 0.5;
    UV.y = (-relativePosZ / TerrainSize) + 0.5;
    UV.x = max(0.0f, min(1.0f, UV.x));
    UV.y = max(0.0f, min(1.0f, UV.y));
    float fX = UV.x * (terrain_buffer.lock()->Get_NumVerticeX() - 1);
    float fZ = UV.y * (terrain_buffer.lock()->Get_NumVerticeZ() - 1);
    
    int iX = (int)fX;
    int iZ = (int)fZ;

    
    int iIndex = (iZ * terrain_buffer.lock()->Get_NumVerticeX()) + iX;
    const vector<float>& vec01 = terrain_buffer.lock()->Get_HeightData();
    float height01 = vec01[iIndex];
   


	_float offsetY = {};
	for (_uint i = 0; i < m_WaveCount; i++)
	{
		_vector waveDir = XMLoadFloat2(&m_waveDesc[i].dir);
		waveDir = XMVector2Normalize(waveDir);

		float RadianWaveLength = (numbers::pi * 2) / m_waveDesc[i].waveLength;
        _float2 normalizedDir = m_waveDesc[i].dir;
		_float dotXZ = (normalizedDir.x * Pos.x) + (normalizedDir.y * Pos.z);
		float angle = dotXZ * RadianWaveLength - (m_waveDesc[i].speed * m_AccTime);
		float amplitude = m_waveDesc[i].waveHeight* height01;


		offsetY += amplitude * sin(angle);
		/*offset.x = waveDir.x * (amplitude * cos(angle));
		offset.z = waveDir.y * (amplitude * cos(angle));*/

	}

	return offsetY;

}

void CSea_Manager::OnGui()
{
    ImGui::DragFloat("Global Sea Pos(Y)", &m_GlobalY, 0.1f);

    //ImGui::SliderFloat("Depth Mask (0~1)", &m_depthMask01, 0.0f, 1.0f);

    ImGui::DragFloat("AccTime", &m_AccTime, 0.01f);

	ImGui::ColorEdit3("Deep Sea Color", (float*)&deepColor);
	ImGui::ColorEdit3("Shallow Sea Color", (float*)&shallowColor);


    ImGui::Separator();

  
    ImGui::SliderInt("Wave Count", &m_WaveCount, 0, 10);

    ImGui::Separator();

    ImGui::Text("[ Wave Details ]");


    for (int i = 0; i < m_WaveCount; ++i)
    {

        ImGui::PushID(i);
        
        if (ImGui::TreeNode((void*)(intptr_t)i, "Wave %d", i))
        {
           
            if (ImGui::SliderFloat2("Direction (X,Z)", &m_waveDesc[i].dir.x, -1.0f, 1.0f))
            {
             
                float length = sqrt((m_waveDesc[i].dir.x * m_waveDesc[i].dir.x) + (m_waveDesc[i].dir.y * m_waveDesc[i].dir.y));
                if (length > 0.0001f) {
                    m_waveDesc[i].dir.x /= length;
                    m_waveDesc[i].dir.y /= length;
                }
            }

           
            ImGui::DragFloat("Wave Height", &m_waveDesc[i].waveHeight, 0.01f, 0.0f, 50.0f);

            ImGui::DragFloat("Wave Length", &m_waveDesc[i].waveLength, 0.1f, 0.1f, 1000.0f);

            ImGui::DragFloat("Speed", &m_waveDesc[i].speed, 0.01f, 0.0f, 20.0f);

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

	CBase::OnGui();
}


void CSea_Manager::Free()
{
	CBase::Free();
}
