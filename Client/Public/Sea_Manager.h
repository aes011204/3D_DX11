#pragma once
#include "Client_Define.h"
#include "Base.h"

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
    void Update(float TimeDelta);
    float Calculate_GerstnerWave_Overlap(_float3 Pos);

    virtual void OnGui() override;

    float Get_AccTime() { return m_AccTime; };
    int Get_WaveCount() { return m_WaveCount; }
    float Get_DepthMask01(){ return m_depthMask01; }
    const Wave_Desc* Get_WaveDescArray() const { return m_waveDesc; }
    const Wave_Desc* Get_Pos() const {}
    float Get_GlobalY() { return m_GlobalY; }
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };

private:

    float m_GlobalY = {};

    float m_AccTime = {};
    int m_WaveCount = {};
    float m_depthMask01 = { 1 };
    Wave_Desc m_waveDesc[10] = {};
public:
 /*   static shared_ptr<CSea_Manager> Create();*/
    virtual void Free() override;

};

NS_END