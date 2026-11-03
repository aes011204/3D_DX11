#pragma once
#include "Editor_Define.h"
#include "EventBus.h"

struct ImGuiContext;

NS_BEGIN(Engine)
class CEntity;
//class CEventBus;
NS_END


NS_BEGIN(Editor)

struct EditorCpuTimings
{
    double updateMs = 0.0;
    double renderMs = 0.0;
    double captureMs = 0.0;
    double totalMs = 0.0;
    double averageTotalMs = 0.0;
    double peakTotalMs = 0.0;
    bool ready = false;
};

class  CEditorInstance
{
	DECLARE_SINGLETON(CEditorInstance)

private:
	CEditorInstance();
public:
	virtual ~CEditorInstance() =default;

public:
	HRESULT Initialize_Editor(const EngineDesc& engineDesc, ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext);
	void Update_Editor(float fTimeDelta);
	void Render_Editor();
    bool Get_ImGuiGpuMs(double& milliseconds) const;
    const EditorCpuTimings& Get_EditorCpuTimings() const { return m_editorCpuTimings; }
    void Reset_EditorCpuPeak() { m_editorCpuTimings.peakTotalMs = 0.0; }

	ESelectType GetType() const;
	shared_ptr<CEntity> GetEntity() const;

	ImGuiContext* GetContext();
	class CGameView* GetGameView() const { return m_gameView.get(); }

	shared_ptr<CEntity> GetCurSelect() const;

	//Engine::CEventBus* GetEventBus() { return &m_EditorBus; }

private:
    EditorCpuTimings m_editorCpuTimings;
    double m_editorUpdateMs = 0.0;

	unique_ptr<class CImguiManager> m_imguiManager = { nullptr };

	unique_ptr < class CWin_Mananger> m_winManager = {};
	unique_ptr<class CGameView> m_gameView = {};

	shared_ptr<class CSelection> m_selection;

	//Engine::CEventBus m_EditorBus;
public:
	virtual void Free();

};

NS_END
