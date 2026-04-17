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

	struct BOX_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CUI_Box(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_Box(const CUIPanel& prototype);
public:
	~CUI_Box() = default;

public:
	void UI_PanelActive(_bool IsStorage, _bool IsBox = false, _uint BoxNum = 0);
	virtual void UI_Active() override; //  UI 활성활시 호출되는 함수// 이 패널은 다 켜지면 안되기떄문에 오버라이딩으로 호출뻇고 여기서 처리
	HRESULT OnInit(void* pArg) override;
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;

	void SetActiveTab(TAB tab);
public:


private:
	//TAB m_Active = TAB::NONE;

	//shared_ptr<CUIImage> m_Line = {};
	//shared_ptr<CUIImage> m_TextIMG = {};

	shared_ptr<CUIText> m_Name_Text = {};

	/*shared_ptr<CUIButton> m_ButtonContents[32] = {};*/
	//shared_ptr<CUIPanel> m_TabContents[32] = {};
	shared_ptr<CUIPanel> m_Storage = {};


	Vector2 m_vecAni = {};
	_bool m_bStart = false;
	float m_TimeAcc = {};
	float m_fDuration = {};


	//weak_ptr<CPlayerBoat> m_PlayerBoat = {  };

public:
	static shared_ptr<CUI_Box> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	void Free() override;
};

NS_END