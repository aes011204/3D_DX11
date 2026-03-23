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

	m_pGameView = CGameView::Create(EngineDesc, pDevice, pContext);
	if (!m_pGameView)
		return E_FAIL;

	m_pWin_Manager = CWin_Mananger::Create();
	if (!m_pWin_Manager)
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

	// 3? ImGui 시작
	m_pImgui_Manager->Begin();

	// 4? GameView 출력 (SRV 보여주기)
	if (m_pGameView)
		m_pGameView->Render();

	// 5 다른 에디터 UI
	if (m_pWin_Manager)
		m_pWin_Manager->Render();

	// 6? ImGui 렌더 + Present는 밖에서
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

shared_ptr<CEntity> CEditorInstance::GetCurSelect() const
{
	return m_pSelection->GetEntity();
}

void CEditorInstance::BeginRender()
{

	m_pGameView->BeginRender();
}

void CEditorInstance::Free()
{
	m_pSelection.reset();
	m_pWin_Manager.reset();
	m_pGameView.reset();
	m_pImgui_Manager.reset();
}
