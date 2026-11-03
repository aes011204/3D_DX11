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
	m_miniGameType = EMiniGameType::BASIC_CIRCLE;

	MiniGameLogicDesc* pDesc = static_cast<MiniGameLogicDesc*>(pArg);


	m_initZoneCount = pDesc->zoneCount;
	m_zoneCount = m_gameInstance.lock()->RandomInt(pDesc->zoneCount.x, pDesc->zoneCount.y);
	
	//m_zoneSize = m_pGameInstance.lock()->Random(pDesc->zoneSize.x, pDesc->zoneSize.y);
                                                                                                                                                                                        
	float baseSegment = 1.f / m_zoneCount;
	float minRatio = 0.1f;
	float maxRatio = 0.3f;
	float minAbsolute = 0.04f;

	float minSize = baseSegment * minRatio;
	if (minSize < minAbsolute)
		minSize = minAbsolute;

	float maxSize = baseSegment * maxRatio;
	if (maxSize < minSize)
		maxSize = minSize;

	m_initZoneSize = _float2(
		minSize,
		maxSize);

	m_zoneSize= m_gameInstance.lock()->Random(m_initZoneSize.x, m_initZoneSize.y);

	//m_pGameInstance.lock()->Get_EventBus()->Subscribe<Evt_FishingData>(
	//	[this](const Evt_FishingData& e) {
	//		m_DefID = e.Fish_ID;
	//	}
	//);



	m_speed = 100.f;





	////////////////////TEST//////////////////////////////

	float divide = 1.f / m_zoneCount;

	for (int i = 0; i < m_zoneCount; i++)
	{
		float divideStart = i * divide;
		float divideEnd = divideStart + divide;

		float size = m_gameInstance.lock()->Random(m_initZoneSize.x, m_initZoneSize.y);
		if (size > divide)
			size = divide;

		// 구간 안에서만 생성되게 제한
		float start = m_gameInstance.lock()->Random(divideStart, divideEnd - size);
		float end = start + size;

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



	
	if (m_isStart == true)
	{
		m_prograssBar01 += m_rodSpeed * 0.005f * timeDelta;


		////////////////////////////////////
		m_accTime += timeDelta;
		m_angle = m_speed * m_accTime;


		if(m_isInputRequested)
		{

			float angle01 = {};
			float currentAngle = fmod(m_angle, 360.f);
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
					m_gameInstance.lock()->Play_Once(L"Fishing_Success");

					m_lastInputResult = EInputResult::SUCCESS;
				}
				else
				{
					// 초록 영역 붉어지고 빨간 원 이팩트 밖으로 커짐 아주 짧게

					m_prograssBar01 -= 0.2;
					m_gameInstance.lock()->Play_Once(L"Fishing_Failure");

					m_lastInputResult = EInputResult::FAIL;

					m_changeColor = true;
					colortime = .5f;
				}
			}





			m_isInputRequested = false;

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
	m_zoneCount = m_gameInstance.lock()->RandomInt(m_initZoneCount.x, m_initZoneCount.y);

	float baseSegment = 1.f / m_zoneCount;
	float minRatio = 0.05f;
	float maxRatio = 0.3f;

	m_initZoneSize = _float2(
		baseSegment * minRatio,
		baseSegment * maxRatio);

	m_zoneSize = m_gameInstance.lock()->Random(m_initZoneSize.x, m_initZoneSize.y);

	float divide = 1.f / m_zoneCount;

	for (int i = 0; i < m_zoneCount; i++)
	{
		float divideStart = i * divide;
		float divideEnd = divideStart + divide;

		//float size = m_pGameInstance.lock()->Random(m_zoneSize.x, m_zoneSize.y);

		// 구간 안에서만 생성되게 제한
		float start = m_gameInstance.lock()->Random(divideStart, divideEnd - m_zoneSize);
		float end = start + m_zoneSize;

		m_zones[i].start = start;
		m_zones[i].end = end;
	}
}

void CMiniGame_Logic::OnInput()
{
	bool isDragging = m_invenCtrl.lock()->Is_Dragging();
	if (isDragging)
	{
		m_gameInstance.lock()->Stop(L"fishing_loop");
		m_isStart = false;
		return;
	}
	if (m_isFinished && m_fishCount == 0)
	{
		m_gameInstance.lock()->Play_Once(L"fishing_end");
		m_isStart = false;
		return;
	}
	if (m_isFinished && m_fishCount > 0)
	{
		m_isFinished = false;
	}

	// 시작
	if (!m_isStart)
	{
		m_gameInstance.lock()->Play_Loop(L"fishing_loop");
		m_isStart = true;
		m_accTime = 0.f;
		m_prograssBar01 = 0.f;
		m_isFinished = false;
		return;
	}
		
		
	m_isInputRequested = true;
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
