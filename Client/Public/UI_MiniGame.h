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
	
	struct MiniGamePanelDesc : public CUIPanel::UIPanelDesc
	{

	};
protected:
	CUI_MiniGame(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CUI_MiniGame(const CUIPanel& prototype);
public:
	~CUI_MiniGame() = default;

public:
	void SetMiniGameLogic(shared_ptr<CMiniGame> logic) { m_logic = logic; }

	void UI_PanelActive();

	HRESULT OnInit(void* arg) override;
	void Is_OnZoon(_bool isOnZone);
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
	float m_accTime2 = { 0 };
	float m_colorTime = { 0 };

	///
	float m_prograssBar01 = { 0 };
	float m_rodSpeed = {};


	// 공통
	_uint m_defID = { kIDAbsence };
	_bool m_isFinished = { false };

	//
	_uint m_fishCount = {};

	EMiniGameType m_miniGameState = {};
private:
	shared_ptr<CUIText> m_nameTex = { nullptr };//
	shared_ptr<CUIText> m_amountTex = { nullptr };//
	shared_ptr<CUIText> m_locationTex = { nullptr };//
	shared_ptr<CUIImage> m_fishIcon = { nullptr }; //

	shared_ptr<CUIImage> m_circleEff = { nullptr };//
	shared_ptr<CUIImage> m_progressIcon = { nullptr };//


	shared_ptr<CUIButton> m_button = { nullptr };
	shared_ptr<CUIImage> m_base = { nullptr };
	shared_ptr<CUIImage> m_depth = { nullptr };

	shared_ptr<CUIImage> m_circle = { nullptr };
	shared_ptr<CUIImage> m_spinner = { nullptr };

	//Diamond

	shared_ptr<CUIImage> m_sizeCircleIn = { nullptr };//
	shared_ptr<CUIImage> m_sizeCircle = { nullptr };//

    //Ball
	shared_ptr<CUIImage> m_ballCircle = { nullptr };//
	shared_ptr<CUIImage> m_ball = { nullptr };//

	private:
	shared_ptr<CShader> m_circleShader = { nullptr };

	shared_ptr<class CMiniGame> m_logic = { nullptr };
	//shared_ptr<class CMiniGame_Logic> m_LogicCashingCircle = { nullptr };
	//shared_ptr<class CMiniGame_Logic_Ball> m_LogicCashingBall = { nullptr };
	//shared_ptr<class CMiniGame_Logic_Diamond> m_LogicCashingDiamond = {nullptr};
public:
	static shared_ptr<CUI_MiniGame> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

	void Free() override;
};

NS_END