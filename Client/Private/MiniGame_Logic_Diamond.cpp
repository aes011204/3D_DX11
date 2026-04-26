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
	m_MiniGameType = MINIGAME::DIAMOND;

	MINIGAEMELOGIC_DIAMOND_DESC* pDesc = static_cast<MINIGAEMELOGIC_DIAMOND_DESC*>(pArg);

	m_Speed = 1.f;
	__super::Initialize(pDesc);

	return S_OK;

}



void CMiniGame_Logic_Diamond::Update(const _float& timeDelta)
{


	m_prograssBar01 = clamp(m_prograssBar01, 0.f, 1.f);




	if (m_bStart == true)
	{
		m_prograssBar01 += m_RodSpeed * 0.01f * timeDelta;


		////////////////////////////////////
		m_AccTime += timeDelta;
		float t = m_Speed * m_AccTime;

		float loop = fmod(t, 1.f);
		m_Size = t;
		m_Angle = loop * 90.f;
		if (m_bInputRequested)
		{

			/*if (m_chose == true)
			{*/
				bool isSuccess = false;

				if (m_Size > 1.f && m_Size < 1.2f)
				{
					isSuccess = true;
				}

				if (isSuccess == true)
				{
					// 효과 초록 원 이팩트 밖으로 커짐
				//m_bStart = false;

					m_prograssBar01 += 0.2;
					m_pGameInstance.lock()->Play_Once(L"Fishing_Success");

					m_LastInputResult = INPUT_RESULT::SUCCESS;
				}
				else
				{
					// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

					m_prograssBar01 -= 0.2;

					m_LastInputResult = INPUT_RESULT::FAIL;
					m_pGameInstance.lock()->Play_Once(L"Fishing_Failure");

					m_changeColor = true;
					colortime = .5f;
				}
			//}


			m_bInputRequested = false;

		}

		if (m_Size > 2)
		{
			m_Size = 0.f;
			m_AccTime = 0.f;
			m_LastInputResult = INPUT_RESULT::START;
		}

		if (m_prograssBar01 >= 1.f && !m_bFin)
		{
			//진짜 성공 반환
			m_bStart = false;

			m_bFin = true;


			Evt_GetFish  e = {};
			e.DefID = m_DefID;
			e.fishInst.size = static_cast<int>(m_pGameInstance.lock()->Random(20.f, 35.f));
			e.fishInst.mutation_ID = 2;
			m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_GetFish>(e);


			m_pGameInstance.lock()->Play_Once(L"Fish_New");
			m_pGameInstance.lock()->Stop(L"fishing_loop");

			m_FishCount--;



		}
	}
}



void CMiniGame_Logic_Diamond::OnInput()
{

	if (m_bFin && m_FishCount > 0)
	{
		m_bFin = false;
	}
	bool isDragging = m_InvenCtrl.lock()->Is_Dragging();
	if (isDragging)
	{
		m_bStart = false;
		m_pGameInstance.lock()->Stop(L"fishing_loop");

		return;
	}
	if (m_bFin && m_FishCount == 0)
	{
		m_bStart = false;
		m_pGameInstance.lock()->Play_Once(L"fishing_end");

		return;
	}

	//시작
	if (!m_bStart)
	{
		m_pGameInstance.lock()->Play_Loop(L"fishing_loop");

		m_LastInputResult = INPUT_RESULT::START;
		m_bStart = true;
		m_AccTime = 0.f;
		m_Size = 0.f;
		m_prograssBar01 = 0.f;
		m_bFin = false;
		return;
	}


	m_bInputRequested = true;
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
