#pragma once
#include "Client_Define.h"
#include "Base.h"
NS_BEGIN(Engine)
class CVIBuffer_Terrain;
NS_END

NS_BEGIN(Client)
class CSea_Manager :
    public CBase
{
    DECLARE_SINGLETON(CSea_Manager)
public:
    CSea_Manager();

public:
    virtual ~CSea_Manager();

public:
    HRESULT Initialize(/*ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext*/);
    void Update(float timeDelta);
    _float3 Calculate_GerstnerWavePosition(_float3 pos);

    virtual void OnGui() override;

    float Get_AccTime() { return m_accTime; };
    int Get_WaveCount() { return m_waveCount; }
    //float Get_DepthMask01(){ return m_depthMask01; }
    const WaveDesc* Get_WaveDescArray() const { return m_waveDesc; }
    const WaveDesc* Get_Pos() const {}
    float Get_GlobalY() { return m_globalY; }
    void Set_TerrainBuffer(shared_ptr<class CVIBuffer_Terrain> buffer)
    {
        m_terrainBuffer = buffer;
    }
   _float3 m_terrainPos={-750.f,0.f,-750.f};
   _float m_terrainSize={1500.f};
    _float3 m_deepColor = _float3(0.094f, 0.137f, 0.302f); // Deep wave color
    _float3 m_shallowColor = _float3(0.114f, 0.176f, 0.365f); // Shallow wave color

private:
    ComPtr<ID3D11Device> m_device = { nullptr };
    ComPtr<ID3D11DeviceContext> m_context = { nullptr };
    weak_ptr<class CGameInstance> m_gameInstance = { };

private:



    float m_globalY = {};

    float m_accTime = {};
    int m_waveCount = {};
   
    WaveDesc m_waveDesc[10] = {};

   weak_ptr<CVIBuffer_Terrain> m_terrainBuffer = {  };
    ///
public:
 /*   static shared_ptr<CSea_Manager> Create();*/
    virtual void Free() override;

};

NS_END
