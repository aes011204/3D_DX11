#include "../Public/GameInstance.h"

#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Prototype_Manager.h"
#include "Renderer.h"


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
	// 프로토 타입매니져를 생성해 둔다
	m_pProto_Manager = CPrototype_Manager::Create(EngineDesc.iMaxLevelNum);
	if (nullptr == m_pProto_Manager)
		return E_FAIL;
	// 오브젝트 매니져를 생성해 둔다
	m_pObject_Manager = CObject_Manager::Create(EngineDesc.iMaxLevelNum);
	if (nullptr == m_pProto_Manager)
		return E_FAIL;
	// 렌더러를 생성해 둔다
	m_Renderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_Renderer)
		return E_FAIL;

	//Imgui- 젤 마지막에
	{
		m_pImgui_Manager = CImguiManager::Create();
		m_pImgui_Manager->Initialize(EngineDesc.hWnd,*ppDevice, *ppContext);

	}

	return S_OK;
}

ImGuiContext* CGameInstance::GetContext()
{
	return m_pImgui_Manager->GetContext();
}
void CGameInstance::Update_Engine(float fTimeDelta)
{


	m_pObject_Manager->Priority_Update(fTimeDelta);

	m_pObject_Manager->Update(fTimeDelta);

	m_pObject_Manager->Late_Update(fTimeDelta);


	m_pLevel_Manager->Update(fTimeDelta);

	//Imgui
	{
		m_pImgui_Manager->Begin();
	}
	m_pImgui_Manager->Example();
	float fps = 1.f / fTimeDelta;
	ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);

	m_pObject_Manager->Update_Gui();
}

void CGameInstance::Draw()
{
	m_pLevel_Manager->Render();





	//Imgui
	{
	    m_pImgui_Manager->Render();
	}
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	if (FAILED(m_pObject_Manager->Clear_Layers(iLevelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}
	if(FAILED(m_pProto_Manager->Clear_Prototype(iLevelIndex)))
	{
		MSG_BOX("failed to Clear Resourse");
		return;
	}
	return;
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

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	return m_pProto_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag,
	void* pArg)
{
	return  m_pProto_Manager->Clone_Prototype(ePrototy, iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_GameObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
	_uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	return m_pObject_Manager->Add_GameObject(iPrototypeLevelIndex, strPrototypeTag, iLayerLevelIndex, strLayerTag, pArg);
}

void CGameInstance::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	m_Renderer->Add_RenderGroup(eRenderGroup, pGameObject);
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


void CGameInstance::Release_Engine()
{
	if (m_pImgui_Manager)
	{
		m_pImgui_Manager->Free();

	}
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pProto_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_Renderer);


	DestroyInstance();
}

void CGameInstance::Free()
{
	__super::Free();
}
