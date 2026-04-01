#include "Bounding.h"

CBounding::CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
}

HRESULT CBounding::Initialize(BOUNDING_DESC* pDesc)
{
	return S_OK;
}


void CBounding::Free()
{
	CBase::Free();
}
