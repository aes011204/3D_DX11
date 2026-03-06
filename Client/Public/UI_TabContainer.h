#pragma once
#include "UIPanel.h"
#include "Client_Define.h"
//오른쪽에서 나오는 효과있음
// 엔진 루프용 m_Children과는 별개로, 내가 직접 '조종'할 녀석들만 포인터로 보관
// 버튼은 상태를 바꿀 일이 거의 없으니 굳이 안 들고 있어도 되지만, 
// 패널(내용물)은 껏다 켰다 해야 하니 리스트로 관리합니다.

NS_BEGIN(Engine)

class CUIButton;
NS_END
NS_BEGIN(Client)

class CUI_TabContainer :
	public CUIPanel
{
public:

	struct TABCONTAINER_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CUI_TabContainer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_TabContainer(const CUIPanel& prototype);
public:
	~CUI_TabContainer() = default;

public:
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	void UI_PanelActive(_uint iTabfig, TAB Active);
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

public:


private:
	TAB m_Active = TAB::NONE;

	shared_ptr < CUIButton >m_ButtonContents[32] = {};
	shared_ptr < CUIPanel >m_TabContents[32] = {};
public:
	static shared_ptr<CUI_TabContainer> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END