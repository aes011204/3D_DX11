#include "../Public/GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"

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

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;


	return S_OK;
}


void CGameInstance::Update_Engine(float fTimeDelta)
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
