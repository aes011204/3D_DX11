#pragma once

// 뷰 투영행렬을 보관함
// 반복적인 갱신을 통해서 역행렬도 보관한다
// 카메라 위치까지 저장한다

#include "Base.h"

NS_BEGIN(Engine)
class CPipeLine :
    public CBase
{
private:
    CPipeLine();
public:
    virtual ~CPipeLine();

public:
    const _float4x4* Get_Transfrom(ED3DTS eTransformState) const {
        return &m_transformStateMatrices[ETOI(eTransformState)];
    }

    const _float4* Get_CamPosition() const {
        return reinterpret_cast<const _float4*>(m_transformInverseStateMatrices[ETOI(ED3DTS::VIEW)].m[3]);
    }

    const _float4x4* Get_InverseTransfrom(ED3DTS eTransformState) const {
        return &m_transformInverseStateMatrices[ETOI(eTransformState)];
    }
public:
    void Set_Transform(ED3DTS eTransformState, _fmatrix TransformStateMatrix) {
        XMStoreFloat4x4(&m_transformStateMatrices[ETOI(eTransformState)], TransformStateMatrix);
    }
public:
    HRESULT Bind_CamPosition(shared_ptr<class CShader> pShader, const _char* pConstantName);
    HRESULT Bind_TransformMatrix(ED3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName);
    HRESULT Bind_TransformMatrix_Inverse(ED3DTS eTransformState, shared_ptr<class CShader> pShader, const _char* pConstantName);

    void Update();

    void OnGui() override;
private:
    _float4x4 m_transformStateMatrices[ETOI(ED3DTS::END)];
    _float4x4 m_transformInverseStateMatrices[ETOI(ED3DTS::END)];
public:
    static unique_ptr<CPipeLine> Create();
    virtual void Free() override;

};

NS_END