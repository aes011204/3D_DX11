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


HRESULT Editor::CEditorInstance::Initialize_Editor(const EngineDesc& engineDesc, ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	m_imguiManager = CImguiManager::Create();
	m_imguiManager->Initialize(engineDesc.hWnd,pDevice, pContext);

	m_winManager = CWin_Mananger::Create();
	if (!m_winManager)
		return E_FAIL;

	m_gameView = CGameView::Create(engineDesc, pDevice, pContext);
	if (!m_gameView)
		return E_FAIL;

	m_selection = CSelection::Create();
	if (!m_selection)
		return E_FAIL;

	GetContext();


	return S_OK;
}

void Editor::CEditorInstance::Update_Editor(float fTimeDelta)
{
    const auto updateStart = EditorPerfClock::now();
	if (m_winManager)
		m_winManager->Update(fTimeDelta);

	if (m_gameView)
		m_gameView->Update(fTimeDelta);

	
	//m_pImgui_Manager->Example();
	//float fps = 1.f / fTimeDelta;
	//ImGui::Text("TimDelta = %.5f | FPS = %.1f ", fTimeDelta, fps);
	
	//m_pObject_Manager->Update_Gui();

    m_editorUpdateMs = EditorElapsedMs(updateStart);


}

void Editor::CEditorInstance::Render_Editor()
{
    const auto renderStart = EditorPerfClock::now();
	m_imguiManager->Begin();

	if (m_gameView)
		m_gameView->Render();

	auto* gd = CGameInstance::GetInstance()->Get_GraphicDevice();
    double captureMs = 0.0;
	if (gd && m_gameView)
	{
        const auto captureStart = EditorPerfClock::now();
		m_gameView->CaptureFromBackBuffer(gd->GetBackBufferRTV());
        captureMs = EditorElapsedMs(captureStart);
    }

	if (m_winManager)
		m_winManager->Render();




	m_imguiManager->Render();

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
    return m_imguiManager && m_imguiManager->Get_GpuRenderMs(milliseconds);
}

ESelectType CEditorInstance::GetType() const
{
	if (m_selection->HasSelection())
		return m_selection->GetType();
	else
		return ESelectType::NONE;

}

shared_ptr<CEntity> CEditorInstance::GetEntity() const
{
	if (m_selection->HasSelection())
		return m_selection->GetEntity();
	else
		return nullptr;
}

ImGuiContext* CEditorInstance::GetContext()
{
	return m_imguiManager->GetContext();
}

shared_ptr<CEntity> CEditorInstance::GetCurSelect() const
{
	return m_selection->GetEntity();
}

void CEditorInstance::Free()
{
	m_selection.reset();
	m_winManager.reset();
	m_gameView.reset();
	m_imguiManager.reset();
}
