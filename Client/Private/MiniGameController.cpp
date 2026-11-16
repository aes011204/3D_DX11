#include "MiniGameController.h"
#include "GameInstance.h"

#include "Fish.h"
#include "MiniGame_Logic_Ball.h"
#include "MiniGame_Logic_Diamond.h"
#include "UI_MiniGame.h"
#include "EventBus.h"

Client::CMiniGameController::CMiniGameController()
	: m_gameInstance(CGameInstance::GetInstance())
{
}

Client::CMiniGameController::CMiniGameController(const CMiniGameController& prototype)
	: m_gameInstance(CGameInstance::GetInstance())
{
}

HRESULT Client::CMiniGameController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Client::CMiniGameController::Initialize(void* arg)
{
	return S_OK;

}

HRESULT Client::CMiniGameController::Render(_uint meshIndex)
{
	return S_OK;

}

void Client::CMiniGameController::Update(const _float& timeDelta)
{
}

void Client::CMiniGameController::Start_Fishing(weak_ptr<CFish> m_fish, weak_ptr<class CInventory_Controller> invenCtrl, float rodSpeed)
{
	shared_ptr<CMiniGame>  instance = {};
	auto minigame  = m_fish.lock()->GetMiniGameType();
	switch (minigame)
	{
	case EMiniGameType::BASIC_CIRCLE:
	{
		CMiniGame_Logic::MiniGameLogicDesc logicDesc = { };
		logicDesc.invenCtrl = invenCtrl.lock();

		logicDesc.fishCount = m_fish.lock()->GetInitFishCount();
		logicDesc.defID = m_fish.lock()->Get_FishDefID();
		logicDesc.rodSpeed = rodSpeed;
		logicDesc.zoneCount =_float2(2.f, 4.f);
		/*float baseSegment = 1.f / pLogicDesc.zoneCount;
		float minRatio = 0.05f;
		float maxRatio = 0.3f;

		pLogicDesc.zoneSize = _float2(
			baseSegment * minRatio,
			baseSegment * maxRatio);*/
		logicDesc.miniGameType = EMiniGameType::BASIC_CIRCLE;


		instance = CMiniGame_Logic::Create(&logicDesc);
		m_fish.lock()->SetMiniGameLogic(instance);

		break;
	}

	case EMiniGameType::DIAMOND:
	{
		CMiniGame_Logic_Diamond::MiniGameLogicDiamondDesc logicDesc = { };
		logicDesc.invenCtrl = invenCtrl.lock();

		logicDesc.fishCount = m_fish.lock()->GetInitFishCount();
		logicDesc.defID = m_fish.lock()->Get_FishDefID();
		logicDesc.rodSpeed = rodSpeed;
		logicDesc.miniGameType = EMiniGameType::DIAMOND;
		
		instance = CMiniGame_Logic_Diamond::Create(&logicDesc);
		m_fish.lock()->SetMiniGameLogic(instance);

		break;
	}
	case EMiniGameType::BALL:
	{
		/*CMiniGame_Logic_Ball::MINIGAEMELOGIC_BALL_DESC pLogicDesc = { };
		desc.level = 1;

		logic = CHackingLogic::Create(&desc);*/
		break;
	}
	}
	
	

	Evt_BindMiniGameLogic e;
	e.logic = instance;

	m_gameInstance.lock()->Get_EventBus()->Publish(e);

	auto miniGameUI = m_gameInstance.lock()->Find_UI_InCurLevel(EUILayer::WINDOW, L"MiniGame");
	auto UI = dynamic_pointer_cast<CUI_MiniGame>(miniGameUI);
	
	
}

shared_ptr<Client::CMiniGameController> Client::CMiniGameController::Create()
{
	shared_ptr<CMiniGameController> instance(new CMiniGameController(), [](CMiniGameController* p) {p->Free(); delete(p); });

	/*if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
	}*/
	return instance;

}

void Client::CMiniGameController::Free()
{
	CBase::Free();
}
