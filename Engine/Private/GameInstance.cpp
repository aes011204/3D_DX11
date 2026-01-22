#include "../Public/GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"


IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext)
{
	// 그래픽디바이스를 초기화 한다
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc.hWnd, EngineDesc.eWinMode, EngineDesc.iViewportWidth, EngineDesc.iViewportHeight, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	// 타이머 매니져를 생성해 둔다
	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	// 레벨 매니져를 생성해 둔다
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return S_OK;
}


void CGameInstance::Update_Engine(float fTimeDelta)
{
	m_pLevel_Manager->Update(fTimeDelta);

}

void CGameInstance::Draw()
{
	m_pLevel_Manager->Render();

}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{


}

HRESULT CGameInstance::Resize(_uint g_RsizeWidth, _uint g_RsizeHeight)
{

	return m_pGraphic_Device->Resize(g_RsizeHeight, g_RsizeWidth);;
}

HRESULT CGameInstance::Add_Timer(const _wstring& strTimeTag)
{

	if (FAILED(m_pTimer_Manager->Add_Timer(strTimeTag)))
		return E_FAIL;

	return S_OK;
}

_float CGameInstance::Compute_TimeDelta(const _wstring& strTimeTag)
{
	return m_pTimer_Manager->Compute_TimeDelta(strTimeTag);
}

HRESULT CGameInstance::Change_Level(_uint iNewLevelIndex, CLevel* pNewLevel)
{

	return m_pLevel_Manager->Change_Level(iNewLevelIndex, pNewLevel);


}


HRESULT CGameInstance::Clear_Buffers(const _float4* pClearColor)
{
	if (FAILED(m_pGraphic_Device->Clear_BackBuffer_View(pClearColor)))
		return E_FAIL;
	if (FAILED(m_pGraphic_Device->Clear_DepthStencil_View()))
		return E_FAIL;

	return S_OK;
}
HRESULT CGameInstance::Bind_BackBufferRenderTarget(HWND hwnd)
{
	return m_pGraphic_Device->Bind_BackBufferRenderTarget(hwnd);
}
HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();;
}


void CGameInstance::Free()
{
	__super::Free();
}
