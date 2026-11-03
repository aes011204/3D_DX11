#include "Level.h"

#include "GameInstance.h"

CLevel::CLevel(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: m_device(device)
	, m_context(context)
	,m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

HRESULT CLevel::Post_Initialize()
{
	return S_OK;
}

void CLevel::Update(_float timeDelta)
{

}

HRESULT CLevel::Render()
{
	return S_OK;
}

void CLevel::Free()
{
	__super::Free();


}
