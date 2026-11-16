#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CPicking_Manager :
    public CBase
{
private:
    CPicking_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
    virtual ~CPicking_Manager();

public:
    HRESULT Initialize(); 
public:
    void Update();   // 여기서 무거운거 한 번만 계산 후 캐싱
    _bool Culaulate_Terrain(class CVIBuffer_Terrain* buffer,class CTransform* transform, _float3* outPos);
    _bool Picking_Terrain(_wstring layerTag, _uint terrainIndex, _float3* Out);
    _float Calculate_HeightOnTerrain(_fvector pos, _fvector pointA, _fvector pointB, _fvector pointC);
    _bool Compute_HeightOnTerrain(_wstring layerTag, _uint terrainIndex, _fvector curPos, _float* Out);
   

    const Ray& GetRay() const { return m_currentRay; }
private:
    Ray m_currentRay;

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_context;
    weak_ptr<class CGameInstance> m_gameInstance = {};
public:
    static unique_ptr<CPicking_Manager> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    void Free() override;
};

NS_END