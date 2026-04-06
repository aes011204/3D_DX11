#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CPicking_Manager :
    public CBase
{
private:
    CPicking_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
    virtual ~CPicking_Manager();

public:
    HRESULT Initialize(); 
public:
    void Update();   // 여기서 무거운거 한 번만 계산 후 캐싱
    _bool Culaulate_Terrain(class CVIBuffer_Terrain* pBuffer,class CTransform* pTransform, _float3* pOutPos);
    _bool Picking_Terrain(_wstring layerTag, _uint TerrainIndex, _float3* Out);
    _float Calculate_HeightOnTerrain(_fvector pPos, _fvector vPointA, _fvector vPointB, _fvector vPointC);
    _bool Compute_HeightOnTerrain(_wstring layerTag, _uint TerrainIndex, _fvector pCurPos, _float* Out);
   

    const Ray& GetRay() const { return m_CurrentRay; }
private:
    Ray m_CurrentRay;

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    weak_ptr<class CGameInstance> m_pGameInstance = {};
public:
    static unique_ptr<CPicking_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    void Free() override;
};

NS_END