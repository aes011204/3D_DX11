#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CPiking_Manager :
    public CBase
{
private:
    CPiking_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CPiking_Manager();

public:
    HRESULT Initialize(); 
public:
    void Update();   // 여기서 무거운거 한 번만 계산 후 캐싱
    _bool Intersect_Terrain(class CVIBuffer_Terrain* pBuffer,class CTransform* pTransform, _float3* pOutPos);
   

    const Ray& GetRay() const { return m_CurrentRay; }
private:
    Ray m_CurrentRay;

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    weak_ptr<class CGameInstance> m_pGameInstance = {};
public:
    static unique_ptr<CPiking_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END