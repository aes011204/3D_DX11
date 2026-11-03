#include "MiniGame_Logic_Diamond.h"


#include <UIButton.h>
#include <UIText.h>

#include "CScaleModifier.h"
#include "ItemDB.h"
#include "UIImage.h"
#include "DInput_Manager.h"
#include "FadeModifier.h"
#include "Inventory_Controller.h"
#include "EventBus.h"

CMiniGame_Logic_Diamond::CMiniGame_Logic_Diamond()


{
}

CMiniGame_Logic_Diamond::CMiniGame_Logic_Diamond(const CMiniGame_Logic_Diamond& prototype)

{
}

HRESULT CMiniGame_Logic_Diamond::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMiniGame_Logic_Diamond::Render(_uint iMeshIndex)
{

	return S_OK;

}

HRESULT CMiniGame_Logic_Diamond::Initialize(void* pArg)
{
	m_miniGameType = EMiniGameType::DIAMOND;

	MiniGameLogicDiamondDesc* pDesc = static_cast<MiniGameLogicDiamondDesc*>(pArg);

	m_speed = 1.f;
	__super::Initialize(pDesc);

	return S_OK;

}



void CMiniGame_Logic_Diamond::Update(const _float& timeDelta)
{


	m_prograssBar01 = clamp(m_prograssBar01, 0.f, 1.f);




	if (m_isStart == true)
	{
		m_prograssBar01 += m_rodSpeed * 0.005f * timeDelta;


		////////////////////////////////////
		m_accTime += timeDelta;
		float t = m_speed * m_accTime;

		float loop = fmod(t, 1.f);
		m_size = t;
		m_angle = loop * 90.f;
		if (m_isInputRequested)
		{

			/*if (m_chose == true)
			{*/
				bool isSuccess = false;

				if (m_size > 1.f && m_size < 1.2f)
				{
					isSuccess = true;
				}

				if (isSuccess == true)
				{
					// 효과 초록 원 이팩트 밖으로 커짐
				//m_bStart = false;

					m_prograssBar01 += 0.2;
					m_gameInstance.lock()->Play_Once(L"Fishing_Success");

					m_lastInputResult = EInputResult::SUCCESS;
				}
				else
				{
					// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

					m_prograssBar01 -= 0.2;

					m_lastInputResult = EInputResult::FAIL;
					m_gameInstance.lock()->Play_Once(L"Fishing_Failure");

					m_changeColor = true;
					colortime = .5f;
				}
			//}


			m_isInputRequested = false;

		}

		if (m_size > 2)
		{
			m_size = 0.f;
			m_accTime = 0.f;
			m_lastInputResult = EInputResult::START;
		}

		if (m_prograssBar01 >= 1.f && !m_isFinished)
		{
			//진짜 성공 반환
			m_isStart = false;

			m_isFinished = true;


			Evt_GetFish  e = {};
			e.DefID = m_defId;
			e.fishInst.size = static_cast<int>(m_gameInstance.lock()->Random(20.f, 35.f));
			auto& item = CItemDB::GetInstance()->GetItemByID(m_defId);
			if (auto fish = std::get_if<FishDef>(&item.typeDef))
			{
				e.fishInst.mutationId = SelectMutationNoRepeat(*fish);
			}
			m_gameInstance.lock()->Get_EventBus()->Publish<Evt_GetFish>(e);


			m_gameInstance.lock()->Play_Once(L"Fish_New");
			m_gameInstance.lock()->Stop(L"fishing_loop");

			m_fishCount--;



		}
	}
}



void CMiniGame_Logic_Diamond::OnInput()
{
	bool isDragging = m_invenCtrl.lock()->Is_Dragging();
	if (isDragging)
	{
		m_isStart = false;
		m_gameInstance.lock()->Stop(L"fishing_loop");

		return;
	}
	if (m_isFinished && m_fishCount == 0)
	{
		m_isStart = false;
		m_gameInstance.lock()->Play_Once(L"fishing_end");

		return;
	}
	if (m_isFinished && m_fishCount > 0)
	{
		m_isFinished = false;
	}

	//시작
	if (!m_isStart)
	{
		m_gameInstance.lock()->Play_Loop(L"fishing_loop");

		m_lastInputResult = EInputResult::START;
		m_isStart = true;
		m_accTime = 0.f;
		m_size = 0.f;
		m_prograssBar01 = 0.f;
		m_isFinished = false;
		return;
	}


	m_isInputRequested = true;
	//else { m_chose = true; }



	//if (m_chose == true)
	//{
	//	bool isSuccess = false;

	//	if(m_Size > 0.8f&&m_Size<1.2f)
	//	{
	//		isSuccess = true;
	//	}

	//	if (isSuccess == true)
	//	{
	//		// 효과 초록 원 이팩트 밖으로 커짐
	//	//m_bStart = false;

	//		m_prograssBar01 += 0.2;
	//	
	//		m_LastInputResult = INPUT_RESULT::SUCCESS;
	//	}
	//	else
	//	{
	//		// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

	//		m_prograssBar01 -= 0.2;

	//		m_LastInputResult = INPUT_RESULT::FAIL;
	//		
	//		m_changeColor = true;
	//		colortime = .5f;
	//	}
	//}

}


shared_ptr<CMiniGame_Logic_Diamond> CMiniGame_Logic_Diamond::Create(void* pArg)
{
	shared_ptr<CMiniGame_Logic_Diamond> pInstance(new CMiniGame_Logic_Diamond(), [](CMiniGame_Logic_Diamond* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CMiniGame_Logic_Diamond::Free()
{
	__super::Free();
}
