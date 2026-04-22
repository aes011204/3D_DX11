#pragma once
#include "UIPanel.h"
#include "Client_Define.h"

namespace Engine
{
	class CUIButton;
	class CUIImage;
	class CUIText;
}

NS_BEGIN(Client)

class CMiniGame_Logic;

	class CUI_MiniGame :
	public CUIPanel
{
public:
	
	struct MINIGAEMEPANEL_DESC : public CUIPanel::UIPANEL_DESC
	{

	};
protected:
	CUI_MiniGame(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUI_MiniGame(const CUIPanel& prototype);
public:
	~CUI_MiniGame() = default;

public:
	void SetMiniGameLogic(shared_ptr<CMiniGame> logic) { m_Logic = logic; }

	void UI_PanelActive();

	HRESULT OnInit(void* pArg) override;
	void Is_OnZoon(_bool IsOnZoon);
	void OnActive()override;
	void OnInActive()override;
	void OnDisabled()override;
	void OnUpdate(const _float& timeDelta)override;
	void OnLateUpdate()override;
	HRESULT OnRender()override;
	void OnClear()override;




public:
	Zone m_zones[8];
	float m_zoneCount;
	//float m_AccTime = { 0 };
	//float m_Angle = { 0 };
	//float m_Speed = {};
	//_bool m_bStart = { false };

	///
	bool m_changeColor = { false };
	float m_AccTime2 = { 0 };
	float colortime = { 0 };

	///
	float m_prograssBar01 = { 0 };
	float m_RodSpeed = {};


	// °øÅë
	_uint m_DefID = { ID_Absence };
	_bool m_bFin = { false };

	//
	_uint m_FishCount = {};

	MINIGAME m_MiniGameState = {};
private:
	shared_ptr<CUIText> m_nameTex = { nullptr };//
	shared_ptr<CUIText> m_amountTex = { nullptr };//
	shared_ptr<CUIText> m_locationTex = { nullptr };//
	shared_ptr<CUIImage> m_FishIcon = { nullptr }; //

	shared_ptr<CUIImage> m_pCircleEff = { nullptr };//
	shared_ptr<CUIImage> m_PrograssIcon = { nullptr };//


	shared_ptr<CUIButton> m_Button = { nullptr };
	shared_ptr<CUIImage> m_Base = { nullptr };
	shared_ptr<CUIImage> m_Depth = { nullptr };

	shared_ptr<CUIImage> m_pCircle = { nullptr };
	shared_ptr<CUIImage> m_Spinner = { nullptr };

	//Diamond

	shared_ptr<CUIImage> m_pSizeCircleIn = { nullptr };//
	shared_ptr<CUIImage> m_pSizeCircle = { nullptr };//

    //Ball
	shared_ptr<CUIImage> m_pBallCircle = { nullptr };//
	shared_ptr<CUIImage> m_pBall = { nullptr };//

	private:
	shared_ptr<CShader> m_CircleShader = { nullptr };

	shared_ptr<class CMiniGame> m_Logic = { nullptr };
	//shared_ptr<class CMiniGame_Logic> m_LogicCashingCircle = { nullptr };
	//shared_ptr<class CMiniGame_Logic_Ball> m_LogicCashingBall = { nullptr };
	//shared_ptr<class CMiniGame_Logic_Diamond> m_LogicCashingDiamond = {nullptr};
public:
	static shared_ptr<CUI_MiniGame> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	void Free() override;
};

NS_END