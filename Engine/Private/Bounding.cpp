#include "Bounding.h"

CBounding::CBounding(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device(device), m_context(context)
{
}

HRESULT CBounding::Initialize(BoundingDesc* desc)
{
	auto boundingDesc = static_cast<BoundingDesc*>(desc);

	return S_OK;
}


void CBounding::Free()
{
	CBase::Free();
}
