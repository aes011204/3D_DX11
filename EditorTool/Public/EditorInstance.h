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
	HRESULT Initialize_Editor(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext);
	void Update_Editor(float fTimeDelta);
	void Render_Editor();
    bool Get_ImGuiGpuMs(double& milliseconds) const;
    const EditorCpuTimings& Get_EditorCpuTimings() const { return m_editorCpuTimings; }
    void Reset_EditorCpuPeak() { m_editorCpuTimings.peakTotalMs = 0.0; }

	ESelectType GetType() const;
	shared_ptr<CEntity> GetEntity() const;

	ImGuiContext* GetContext();
	class CGameView* GetGameView() const { return m_pGameView.get(); }

	shared_ptr<CEntity> GetCurSelect() const;

	//Engine::CEventBus* GetEventBus() { return &m_EditorBus; }

private:
    EditorCpuTimings m_editorCpuTimings;
    double m_editorUpdateMs = 0.0;

	unique_ptr<class CImguiManager> m_pImgui_Manager = { nullptr };

	unique_ptr < class CWin_Mananger> m_pWin_Manager = {};
	unique_ptr<class CGameView> m_pGameView = {};

	shared_ptr<class CSelection> m_pSelection;

	//Engine::CEventBus m_EditorBus;
public:
	virtual void Free();

};

NS_END
