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


	m_waveCount = 3;

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

void CSea_Manager::Update(float timeDelta)
{
	m_accTime += timeDelta;

}
_float3 CSea_Manager::Calculate_GerstnerWavePosition(_float3 pos)
{
  

    //float relativePosX = Pos.x - TerrainPos.x;
    //float relativePosZ = Pos.z - TerrainPos.z;

    _float2 UV;
    UV.x = (pos.x - m_terrainPos.x) / m_terrainSize;
    UV.y = 1.0f - ((pos.z - m_terrainPos.z) / m_terrainSize);
    UV.x = max(0.0f, min(1.0f, UV.x));
    UV.y = max(0.0f, min(1.0f, UV.y));
    float gridX = UV.x * (m_terrainBuffer.lock()->Get_NumVerticeX() - 1);
    float gridZ = UV.y * (m_terrainBuffer.lock()->Get_NumVerticeZ() - 1);
    
    int cellX = (int)gridX;
    int cellZ = (int)gridZ;

    
    int heightIndex = (cellZ * m_terrainBuffer.lock()->Get_NumVerticeX()) + cellX;
    const vector<float>& vec01 = m_terrainBuffer.lock()->Get_HeightData();
    float height01 = max(1.f- vec01[heightIndex],0.1f);
   


	_float3 wavePosition = pos;
	for (_uint i = 0; i < m_waveCount; i++)
	{
		_float2 waveDir = m_waveDesc[i].dir;
		const float dirLength = sqrtf(waveDir.x * waveDir.x + waveDir.y * waveDir.y);
		if (dirLength <= 0.f)
			continue;

		waveDir.x /= dirLength;
		waveDir.y /= dirLength;

		float radianWaveLength = (numbers::pi * 2) / m_waveDesc[i].waveLength;
		_float dotXZ = (waveDir.x * pos.x) + (waveDir.y * pos.z);
		float angle = dotXZ * radianWaveLength - (m_waveDesc[i].speed * m_accTime);
		float amplitude = m_waveDesc[i].waveHeight* height01;


		wavePosition.y += amplitude * sinf(angle);
		wavePosition.x += waveDir.x * (amplitude * cosf(angle));
		wavePosition.z += waveDir.y * (amplitude * cosf(angle));

	}

	return wavePosition;

}

void CSea_Manager::OnGui()
{
    ImGui::DragFloat("Global Sea Pos(Y)", &m_globalY, 0.1f);

    //ImGui::SliderFloat("Depth Mask (0~1)", &m_depthMask01, 0.0f, 1.0f);

    ImGui::DragFloat("AccTime", &m_accTime, 0.01f);

	ImGui::ColorEdit3("Deep Sea Color", (float*)&m_deepColor);
	ImGui::ColorEdit3("Shallow Sea Color", (float*)&m_shallowColor);


    ImGui::Separator();

  
    ImGui::SliderInt("Wave Count", &m_waveCount, 0, 10);

    ImGui::Separator();

    ImGui::Text("[ Wave Details ]");


    for (int i = 0; i < m_waveCount; ++i)
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

