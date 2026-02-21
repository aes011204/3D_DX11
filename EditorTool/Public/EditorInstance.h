#pragma once
#include "Editor_Define.h"
#include "EventBus.h"

struct ImGuiContext;

NS_BEGIN(Engine)
class CEntity;
//class CEventBus;
NS_END


NS_BEGIN(Editor)

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

	ESelectType GetType() const;
	shared_ptr<CEntity> GetEntity() const;

	ImGuiContext* GetContext();
	class CGameView* GetGameView() const { return m_pGameView.get(); }

	shared_ptr<CEntity> GetCurSelect() const;

	//Engine::CEventBus* GetEventBus() { return &m_EditorBus; }

private:
	unique_ptr<class CImguiManager> m_pImgui_Manager = { nullptr };

	unique_ptr < class CWin_Mananger> m_pWin_Manager = {};
	unique_ptr<class CGameView> m_pGameView = {};

	shared_ptr<class CSelection> m_pSelection;

	//Engine::CEventBus m_EditorBus;
public:
	virtual void Free();

};

NS_END