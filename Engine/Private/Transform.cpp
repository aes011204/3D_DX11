#include "Transform.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
{
}

HRESULT CTransform::Initialize_Prototype()
{
	return S_OK;
	//프로토 타입을 만들지 않음 모든 트렌스폼이 원본일 예정
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFOM_DESC* pDesc = static_cast<TRANSFOM_DESC*>(pArg);
	m_fRotationPerSec = pDesc->fRotationPerSec;
	m_fSpeedPerSec = pDesc->fSpeedPerSec;

	return S_OK;
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CTransform* CTransform::Clone(void* pArg)
{
	return nullptr;
	// 클론도 하지 않을것임
}

void CTransform::Free()
{
	__super::Free();
}
