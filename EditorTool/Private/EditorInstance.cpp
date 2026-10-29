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
#include <chrono>

namespace
{
    using EditorPerfClock = std::chrono::steady_clock;

    double EditorElapsedMs(EditorPerfClock::time_point start)
    {
        return std::chrono::duration<double, std::milli>(EditorPerfClock::now() - start).count();
    }
}


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
    const auto updateStart = EditorPerfClock::now();
	if (m_pWin_Manager)
		m_pWin_Manager->Update(fTimeDelta);

	if (m_pGameView)
		m_pGameView->Update(fTimeDelta);

	
	//m_pImgui_Manager->Example();
	//float fps = 1.f / fTimeDelta;
	//ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);
	
	//m_pObject_Manager->Update_Gui();

    m_editorUpdateMs = EditorElapsedMs(updateStart);


}

void Editor::CEditorInstance::Render_Editor()
{
    const auto renderStart = EditorPerfClock::now();
	m_pImgui_Manager->Begin();

	if (m_pGameView)
		m_pGameView->Render();

	auto* gd = CGameInstance::GetInstance()->Get_GraphicDevice();
    double captureMs = 0.0;
	if (gd && m_pGameView)
	{
        const auto captureStart = EditorPerfClock::now();
		m_pGameView->CaptureFromBackBuffer(gd->GetBackBufferRTV());
        captureMs = EditorElapsedMs(captureStart);
    }

	if (m_pWin_Manager)
		m_pWin_Manager->Render();




	m_pImgui_Manager->Render();

    auto& timings = m_editorCpuTimings;
    timings.updateMs = m_editorUpdateMs;
    timings.renderMs = EditorElapsedMs(renderStart);
    timings.captureMs = captureMs;
    timings.totalMs = timings.updateMs + timings.renderMs;
    timings.averageTotalMs = timings.ready
        ? timings.averageTotalMs * 0.9 + timings.totalMs * 0.1
        : timings.totalMs;
    if (timings.totalMs > timings.peakTotalMs)
        timings.peakTotalMs = timings.totalMs;
    timings.ready = true;
}

bool CEditorInstance::Get_ImGuiGpuMs(double& milliseconds) const
{
    milliseconds = 0.0;
    return m_pImgui_Manager && m_pImgui_Manager->Get_GpuRenderMs(milliseconds);
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

void CEditorInstance::Free()
{
	m_pSelection.reset();
	m_pWin_Manager.reset();
	m_pGameView.reset();
	m_pImgui_Manager.reset();
}
