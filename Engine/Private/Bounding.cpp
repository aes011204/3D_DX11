#include "Bounding.h"

CBounding::CBounding(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_device(pDevice), m_context(pContext)
{
}

HRESULT CBounding::Initialize(BoundingDesc* pDesc)
{
	auto pBoundingDesc = static_cast<BoundingDesc*>(pDesc);

	return S_OK;
}


void CBounding::Free()
{
	CBase::Free();
}
