#pragma once
#include "Component.h"

/* 모델 정점들을 객체 기준으로 월드로 변환시켜주기위한 월드변환행렬을 보관하낟.  */
/* 이 행렬의 상태변환(월드에서의 상태변환)에 해당하는 인터페이스(기능)을 담당한다. */

// 컴포넌트중에 트렌스 폼만 특이하게 클론을 안쓸거임 , 그리고 모든 오드젝트가 무조건 적으로 가지고 있게 할것임

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final:
    public CComponent
{
public:
    struct TRANSFOM_DESC
    {
        _float					fSpeedPerSec = {};
        _float					fRotationPerSec = {};
    };
private:
    CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CTransform(const CTransform& rhs);
    virtual ~CTransform() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
private:
    _float4x4				m_WorldMatrix = {};
    _float					m_fSpeedPerSec = {};
    _float					m_fRotationPerSec = {};
public:
    static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CTransform* Clone(void* pArg) override;
    void Free();
};

NS_END