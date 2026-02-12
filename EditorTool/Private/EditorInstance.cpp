//#include "pch.h"
#include "ImguiManager.h"
#include "EditorInstance.h"
#include "Win_Mananger.h"
#include "GameView.h"
#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Selection.h"
#include "Event_Struct.h"
#include "EventBus.h"


IMPLEMENT_SINGLETON(CEditorInstance)

Editor::CEditorInstance::CEditorInstance()
{

}


HRESULT Editor::CEditorInstance::Initialize_Editor(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	m_pImgui_Manager = CImguiManager::Create();
	m_pImgui_Manager->Initialize(EngineDesc.hWnd,pDevice, pContext);

	m_pWin_Manager = CWin_Mananger::Create();
	if (!m_pWin_Manager)
		return E_FAIL;

	m_pGameView = CGameView::Create(EngineDesc, pDevice, pContext);
	if (!m_pGameView)
		return E_FAIL;

	m_pSelection = CSelection::Create();
	if (!m_pSelection)
		return E_FAIL;

	GetContext();


	return S_OK;
}

void Editor::CEditorInstance::Update_Editor(float fTimeDelta)
{
	if (m_pWin_Manager)
		m_pWin_Manager->Update(fTimeDelta);

	if (m_pGameView)
		m_pGameView->Update(fTimeDelta);

	
	//m_pImgui_Manager->Example();
	//float fps = 1.f / fTimeDelta;
	//ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);
	
	//m_pObject_Manager->Update_Gui();


}

void Editor::CEditorInstance::Render_Editor()
{
	m_pImgui_Manager->Begin();

	if (m_pGameView)
		m_pGameView->Render();

	auto* gd = CGameInstance::GetInstance()->Get_GraphicDevice();
	if (gd && m_pGameView)

		m_pGameView->CaptureFromBackBuffer(gd->GetBackBufferRTV());

	if (m_pWin_Manager)
		m_pWin_Manager->Render();




	m_pImgui_Manager->Render();
}

ESelectType CEditorInstance::GetType() const
{
	if (m_pSelection->HasSelection())
		return m_pSelection->GetType();
	else
		return ESelectType::NONE;

}

shared_ptr<CEntity> CEditorInstance::GetEntity() const
{
	if (m_pSelection->HasSelection())
		return m_pSelection->GetEntity();
	else
		return nullptr;
}

ImGuiContext* CEditorInstance::GetContext()
{
	return m_pImgui_Manager->GetContext();
}

void CEditorInstance::Free()
{
}
