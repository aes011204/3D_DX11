#include "Level.h"

#include "GameInstance.h"

CLevel::CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_device(pDevice)
	, m_context(pContext)
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

void CLevel::Update(_float fTimeDelta)
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
