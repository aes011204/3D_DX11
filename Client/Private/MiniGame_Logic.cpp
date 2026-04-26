#include "MiniGame_Logic.h"


#include <UIButton.h>
#include <UIText.h>

#include "CScaleModifier.h"
#include "ItemDB.h"
#include "UIImage.h"
#include "DInput_Manager.h"
#include "FadeModifier.h"
#include "Inventory_Controller.h"
#include "EventBus.h"

CMiniGame_Logic::CMiniGame_Logic()
	

{
}

CMiniGame_Logic::CMiniGame_Logic(const CMiniGame_Logic& prototype)
	
{
}

HRESULT CMiniGame_Logic::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Logic::Render(_uint iMeshIndex)
{
	return S_OK;

}

HRESULT CMiniGame_Logic::Initialize(void* pArg)
{
	m_MiniGameType = MINIGAME::BASIC_CIRCLE;

	MINIGAEMELOGIC_DESC* pDesc = static_cast<MINIGAEMELOGIC_DESC*>(pArg);


	m_zoneCount = pDesc->zoneCount;
	m_InitZoonSize = pDesc->zoneSize;
	m_zoneSize = m_pGameInstance.lock()->Random(pDesc->zoneSize.x, pDesc->zoneSize.y);
                                                                                                                                                                                        


	//m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_FishingData>(
	//	[this](const Evt_FishingData& e) {
	//		m_DefID = e.Fish_ID;
	//	}
	//);



	m_Speed = 100.f;





	////////////////////TEST//////////////////////////////

	float divide = 1.f / m_zoneCount;

	for (int i = 0; i < m_zoneCount; i++)
	{
		float divideStart = i * divide;
		float divideEnd = divideStart + divide;

		//float size = m_pGameInstance.lock()->Random(m_zoneSize.x, m_zoneSize.y);

		// 구간 안에서만 생성되게 제한
		float start = m_pGameInstance.lock()->Random(divideStart, divideEnd - m_zoneSize);
		float end = start + m_zoneSize;

		m_zones[i].start = start;
		m_zones[i].end = end;
	}


	__super::Initialize(pDesc);
	return S_OK;

}



void CMiniGame_Logic::Update(const _float& timeDelta)
{
	//bool m_chose = false;
	//if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyDown(DIK_F))
	//{
	//	if (/*m_bFin == true &&*/ m_InvenCtrl.lock()->Is_Dragging() == true)
	//	{
	//		m_bStart = false;
	//	}
	//	else if (m_bFin == true && m_FishCount == 0)
	//	{
	//		m_bStart = false;
	//	}
	//	else if (!m_bStart && m_InvenCtrl.lock()->Is_Dragging() != true)
	//	{
	//		m_bStart = true; m_AccTime = 0.f; m_prograssBar01 = 0.f; m_bFin = false;
	//	}
	//	else { m_chose = true; }


	//}

	m_prograssBar01 = clamp(m_prograssBar01, 0.f, 1.f);



	
	if (m_bStart == true)
	{
		m_prograssBar01 += m_RodSpeed * 0.01f * timeDelta;


		////////////////////////////////////
		m_AccTime += timeDelta;
		m_Angle = m_Speed * m_AccTime;


		if(m_bInputRequested)
		{

			float angle01 = {};
			float currentAngle = fmod(m_Angle, 360.f);
			if (currentAngle < 0) currentAngle += 360.f;

			angle01 = (currentAngle / 360.f);

			//if (m_chose == true)
			{
				bool isSuccess = false;
				for (int i = 0; i < m_zoneCount; i++)
				{
					if (m_zones[i].start + 0.01f <= angle01 && m_zones[i].end + 0.01f >= angle01)
					{
						isSuccess = true;
						//m_pGameInstance.lock()->Play_Once(L"Fishing_Success");

						break;
					}
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
					m_pGameInstance.lock()->Play_Once(L"Fishing_Failure");

					m_LastInputResult = INPUT_RESULT::FAIL;

					m_changeColor = true;
					colortime = .5f;
				}
			}





			m_bInputRequested = false;

		}


		/*float currentAngle = fmod(m_Angle, 360.f);
		if (currentAngle < 0) currentAngle += 360.f;

		angle01 = (currentAngle / 360.f);*/
		//LOG_F(LOG_LEVEL::INFO, "angle %f", angle01);

		//if (m_chose == true)
		//{
		//	bool isSuccess = false;
		//	for (int i = 0; i < m_zoneCount; i++)
		//	{
		//		if (m_zones[i].start <= angle01 && m_zones[i].end >= angle01)
		//		{
		//			isSuccess = true;
		//			break;
		//		}
		//	}

		//	if (isSuccess == true)
		//	{
		//		// 효과 초록 원 이팩트 밖으로 커짐
		//	//m_bStart = false;

		//		m_prograssBar01 += 0.2;
		//		Evt_MiniGame e = {};
		//		e.IsOnZoon = true;
		//		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_MiniGame>(e);
		//	}
		//	else
		//	{
		//		// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

		//		m_prograssBar01 -= 0.2;

		//		Evt_MiniGame e = {};
		//		e.IsOnZoon = false;
		//		m_pGameInstance.lock()->Get_EventBus()->Publish<Evt_MiniGame>(e);
		//		
		//		m_changeColor = true;
		//		colortime = .5f;
		//	}
		//}
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

		ResetZoon();

	}



	//if (m_changeColor == true)
	//{
	//	m_AccTime2 += timeDelta;
	//	if (m_AccTime2 >= colortime)
	//	{
	//		m_AccTime2 = 0;
	//		m_changeColor = false;
	//	
	//	}
	//}

	
}

void CMiniGame_Logic::ResetZoon()
{
	m_zoneSize = m_pGameInstance.lock()->Random(m_InitZoonSize.x, m_InitZoonSize.y);

	float divide = 1.f / m_zoneCount;

	for (int i = 0; i < m_zoneCount; i++)
	{
		float divideStart = i * divide;
		float divideEnd = divideStart + divide;

		//float size = m_pGameInstance.lock()->Random(m_zoneSize.x, m_zoneSize.y);

		// 구간 안에서만 생성되게 제한
		float start = m_pGameInstance.lock()->Random(divideStart, divideEnd - m_zoneSize);
		float end = start + m_zoneSize;

		m_zones[i].start = start;
		m_zones[i].end = end;
	}
}

void CMiniGame_Logic::OnInput()
{
	//bool m_chose = false;

	if (m_bFin && m_FishCount > 0)
	{
		

		m_bFin = false;
	}
	bool isDragging = m_InvenCtrl.lock()->Is_Dragging();
	if (isDragging)
	{
		m_pGameInstance.lock()->Stop(L"fishing_loop");
		m_bStart = false;
		return;
	}
	if (m_bFin && m_FishCount == 0)
	{
		m_pGameInstance.lock()->Play_Once(L"fishing_end");

		m_bStart = false;
		return;
	}

	// 시작
	if (!m_bStart)
	{
		m_pGameInstance.lock()->Play_Loop(L"fishing_loop");
		m_bStart = true;
		m_AccTime = 0.f;
		m_prograssBar01 = 0.f;
		m_bFin = false;
		return;
	}
		
		
	m_bInputRequested = true;
		//else { m_chose = true; }

		

}


shared_ptr<CMiniGame_Logic> CMiniGame_Logic::Create(void* pArg)
{
	shared_ptr<CMiniGame_Logic> pInstance(new CMiniGame_Logic(), [](CMiniGame_Logic* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return pInstance;
}

void CMiniGame_Logic::Free()
{
	__super::Free();
}
