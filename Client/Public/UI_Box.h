#pragma once
#include <UIPanel.h>



#pragma once
#include "UIText.h"

#include "UIPanel.h"
#include "Client_Define.h"
#include "PlayerBoat.h"


NS_BEGIN(Engine)

class CUIButton;
class CUIImage;
NS_END
NS_BEGIN(Client)

class CUI_Box :
	public CUIPanel
{
public:

	struct BoxDesc : public CUIPanel::UIPanelDesc
	{

	};
protected:
	CUI_Box(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CUI_Box(const CUIPanel& prototype);
public:
	~CUI_Box() = default;

public:
	void UI_PanelActive(_bool isStorage, _bool isBox = false, _uint boxNum = 0);
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* arg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

	void SetActiveTab(ETab tab);
public:


private:
	//TAB m_Active = TAB::NONE;

	//shared_ptr<CUIImage> m_Line = {};
	shared_ptr<CUIImage> m_nameBase = {};

	shared_ptr<CUIText> m_nameText = {};

	/*shared_ptr<CUIButton> m_ButtonContents[32] = {};*/
	//shared_ptr<CUIPanel> m_TabContents[32] = {};
	shared_ptr<CUIPanel> m_storage = {};
	shared_ptr<class CUI_Storage> m_box = {};


	Vector2 m_vecAni = {};
	_bool m_isStart = false;
	float m_timeAcc = {};
	float m_duration = {};


	//weak_ptr<CPlayerBoat> m_PlayerBoat = {  };

public:
	static shared_ptr<CUI_Box> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	void Free() override;
};

NS_END