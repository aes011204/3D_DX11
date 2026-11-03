#include "ItemInfo.h"

#include <UIImage.h>

#include "ItemDB.h"
#include "UIText.h"
#include "EventBus.h"

CItemInfo::CItemInfo(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CItemInfo::CItemInfo(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CItemInfo::UI_PanelActive(_bool isHold, ItemInst itemInst, EPlayerState playerState, _bool isPlayer)
{
	wstring nameInfo = L"";
	wstring leftInfo = L"";
	wstring rightInfo = L"";
	wstring descInfo = L"";
	//wstring KeyInfo = L"";



	if (itemInst.itemInstID == kIDAbsence)
	{

		UI_InActive();
		return;
	}

	float cost = {};

	float currentY = -m_namePadding;

	ItemDef itemDef = CItemDB::GetInstance()->GetItemByID(itemInst.itemDefID);

	switch (itemInst.itemType)
	{
	case EItemType::FISH:

		if (FishInst* fishInst = get_if<FishInst>(&itemInst.typeDef))
		{
			FishDef* fishDef = get_if<FishDef>(&itemDef.typeDef);

			if (fishInst->mutationID == kIDAbsence)
			{
				nameInfo = S2W(itemDef.itemName);
				descInfo = S2W(itemDef.itemDesc);
				cost = fishDef->cost;
			}
			else
			{
				const int mutationIndex = static_cast<int>(fishInst->mutationID) - 1;

				if (mutationIndex >= 0 && mutationIndex < static_cast<int>(fishDef->vecMutation.size()))
				{
					nameInfo = S2W(fishDef->vecMutation[mutationIndex].mutName);
					descInfo = S2W(fishDef->vecMutation[mutationIndex].mutDesc);
					cost = fishDef->vecMutation[mutationIndex].mutCost;
				}
				else
				{
					nameInfo = S2W(itemDef.itemName);
					descInfo = S2W(itemDef.itemDesc);
					cost = fishDef->cost;
				}
			}

			leftInfo = L"크기:\n상태:\n유형:";
			rightInfo = format(L"{:.2f}cm\n{}\n{}", fishInst->size, freshToWstr(fishInst->freshness), OcceanToWstr(fishDef->seaTypeMask));
		}
		if (isHold == true)
			break;
		break;
	case EItemType::EQUIP:
	{

		EquipInst* equipInst = get_if<EquipInst>(&itemInst.typeDef);
		EquipDef* equipDef = get_if<EquipDef>(&itemDef.typeDef);

		nameInfo = S2W(itemDef.itemName);

		descInfo = S2W(itemDef.itemDesc);
		cost = equipDef->cost;

		if (isHold == true)
			break;

		if (EquipEngine* equipEngine = get_if<EquipEngine>(&equipDef->equipTypeEffect))
		{
			leftInfo = L"설치 시간:\n상태:\n속도:";
			rightInfo = format(L"{}h\n{}\n+{}kn", equipDef->installTime, GetBroken(equipInst->isBroken), equipEngine->speedKn);
		}
		else if (EquipLight* equipLight = get_if<EquipLight>(&equipDef->equipTypeEffect))
		{
			leftInfo = L"설치 시간:\n상태:\n루멘:\n범위";
			rightInfo = format(L"{}h\n{}\n+{}lm\n{}m", equipDef->installTime, GetBroken(equipInst->isBroken), equipLight->lumen, equipLight->range);
		}
		else if (EquipRod* equipRod = get_if<EquipRod>(&equipDef->equipTypeEffect))
		{
			leftInfo = L"설치 시간:\n상태:\n낚시 속도:\n어종";
			rightInfo = format(L"{}h\n{}\n+{}%\n{}", equipDef->installTime, GetBroken(equipInst->isBroken), equipRod->speedPer, OcceanToWstr(equipRod->seaTypeMask));
		}
		else
		{
			//넷트랑 게통발은 패스 그거 까지 할시간 없을듯 후에 시간 남으면 추가
		}

		break;
	}
	case EItemType::MATERIAL:
	{
		MaterialDef* matDef = get_if<MaterialDef>(&itemDef.typeDef);
		nameInfo = S2W(itemDef.itemName);
		descInfo = S2W(itemDef.itemDesc);
		cost = matDef->cost;
		break;

	}
	}
	m_nameText->Set_Text(nameInfo); //위치조정 필요 없음
	if (m_nameText->Get_UIState() != EUIState::ACTIVE)
		m_nameText->UI_Active();
	m_nameText->GetUITransform()->SetAnchoredPos({ m_paddingX, currentY });
	currentY -= (m_nameText->Get_TextSize().y + m_namePadding);

	if (m_lineImg->Get_UIState() != EUIState::ACTIVE)
		m_lineImg->UI_Active();
	m_lineImg->GetUITransform()->SetAnchoredPos({ 0, currentY });
	currentY -= (m_lineImg->GetUITransform()->Get_SizeDelta().y + m_paddingY);

	m_leftText->Set_Text(leftInfo);
	m_leftText->GetUITransform()->SetAnchoredPos({ m_paddingX, currentY });
	if (m_leftText->Get_UIState() != EUIState::ACTIVE)
		m_leftText->UI_Active();

	m_rightText->Set_Text(rightInfo);
	m_rightText->GetUITransform()->SetAnchoredPos({ -m_paddingX, currentY });
	if (m_rightText->Get_UIState() != EUIState::ACTIVE)
		m_rightText->UI_Active();
	currentY -= (m_leftText->Get_TextSize().y + m_paddingY);


	m_detailText->Set_Text(descInfo);
	m_detailText->GetUITransform()->SetAnchoredPos({ -m_paddingX, currentY });
	if (m_detailText->Get_UIState() != EUIState::ACTIVE)
		m_detailText->UI_Active();
	currentY -= (m_detailText->Get_TextSize().y + m_paddingY);

	for (auto& infoBtn : m_vecIcon)
	{
		if (infoBtn->Get_UIState() == EUIState::ACTIVE)
			infoBtn->UI_InActive();
	}


	float sizeY = {};
	if (isHold == true)
	{

		switch (playerState)
		{
		case EPlayerState::FISHING:
		case EPlayerState::SEA:
			// 놓기 회전 버리기
			sizeY = Active_ButtonInfo(EButtonInfo::RELEASE, { 40.f ,currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::ROTATION, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::THROWUP, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);

			break;
		case EPlayerState::REPAIR_SHOP:
		{
			//설치 회전 창고로보내기 환불


			sizeY = Active_ButtonInfo(EButtonInfo::RELEASE, { 40.f,currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::ROTATION, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::STORAGE, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);
			if (itemInst.itemType == EItemType::EQUIP)
			{
				sizeY = Active_ButtonInfo(EButtonInfo::SELL, { 40.f, currentY }, cost*.5f);
				currentY -= (sizeY + m_paddingY);
			}
			break;
		}
		case EPlayerState::FISH_SHOP:

		{


			//설치 회전 창고로보내기 환불

			sizeY = Active_ButtonInfo(EButtonInfo::RELEASE, { 40.f,currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::ROTATION, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);


			sizeY = Active_ButtonInfo(EButtonInfo::STORAGE, { 40.f, currentY });
			currentY -= (sizeY + m_paddingY);
			if (itemInst.itemType == EItemType::FISH)
			{
				sizeY = Active_ButtonInfo(EButtonInfo::SELL, { 40.f, currentY }, cost);
				currentY -= (sizeY + m_paddingY);

			}


			break;
		}
		}
	}
	else
	{

		switch (playerState)
		{
		case EPlayerState::FISHING:
		case EPlayerState::SEA:
			//집기 버리기
			sizeY = Active_ButtonInfo(EButtonInfo::PICK, { 50.f ,currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::THROWUP, { 50.f, currentY });
			currentY -= (sizeY + m_paddingY);
			break;
			//case E_PLAYERSTATE::REPAIR_SHOP: // 이거 구매 측
			//	//구매
			//	sizeY = Active_ButtonInfo(BUTTONINFO::BUY, { 50.f ,fCurrentY }, cost);
			//	fCurrentY -= (sizeY + m_PaddingY);

			//break;
		case EPlayerState::REPAIR_SHOP:
			if (isPlayer)
			{
				sizeY = Active_ButtonInfo(EButtonInfo::PICK, { 50.f ,currentY });
				currentY -= (sizeY + m_paddingY);

				sizeY = Active_ButtonInfo(EButtonInfo::THROWUP, { 50.f, currentY });
				currentY -= (sizeY + m_paddingY);
				sizeY = Active_ButtonInfo(EButtonInfo::STORAGE, { 50.f ,currentY });
				currentY -= (sizeY + m_paddingY);
				if (itemInst.itemType == EItemType::EQUIP)
				{
					sizeY = Active_ButtonInfo(EButtonInfo::SELL, { 50.f, currentY }, cost * 0.5f);
					currentY -= (sizeY + m_paddingY);

				}
			}
			else
			{


				sizeY = Active_ButtonInfo(EButtonInfo::BUY, { 50.f, currentY }, cost);
				currentY -= (sizeY + m_paddingY);
			}
			//집기 버리기 창고로보내기 판매

			break;


		case EPlayerState::FISH_SHOP:
			//집기 버리기 창고로보내기 판매
			sizeY = Active_ButtonInfo(EButtonInfo::PICK, { 50.f ,currentY });
			currentY -= (sizeY + m_paddingY);

			sizeY = Active_ButtonInfo(EButtonInfo::THROWUP, { 50.f, currentY });
			currentY -= (sizeY + m_paddingY);
			sizeY = Active_ButtonInfo(EButtonInfo::STORAGE, { 50.f ,currentY });
			currentY -= (sizeY + m_paddingY);
			if (itemInst.itemType == EItemType::FISH)
			{
				sizeY = Active_ButtonInfo(EButtonInfo::SELL, { 50.f, currentY }, cost);
				currentY -= (sizeY + m_paddingY);
			}

			break;
		}
	}
	//_float Ysize = 0;
	/*for (size_t i = 0; i < m_vecIcon.size(); i++)
	{
		if (m_vecIcon[i]->Get_UIState() == UI_STATE::ACTIVE)
		{
			Ysize = max(Ysize, m_vecIcon[i]->GetUITransform()->Get_WorldRect().Bottom());

		}
	}*/
	m_targetSize.y = abs(currentY) + m_paddingY;
	//m_Targetsize.y = rc.Bottom() - GetUITransform()->Get_WorldRect().Top() ;


	const bool wasActive = Get_UIState() == EUIState::ACTIVE;
	Set_ActiveForCustom();
	if (!wasActive)
		m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();


}

void CItemInfo::UI_Active()
{
	CUIPanel::UI_Active();
}

HRESULT CItemInfo::OnInit(void* arg)
{


	HRESULT hr = E_FAIL;
	ItemInfoDesc* INFODesc = static_cast<ItemInfoDesc*>(arg);
	INFODesc->useNineSlice = true;
	INFODesc->textureComLevel = ETOI(ELevel::STATIC);
	INFODesc->textureProtoName = L"Prototype_Component_Texture_PopupBackground";
	INFODesc->pivot = (_float2{ 0.f,0.f });
	hr = CUIPanel::OnInit(INFODesc);


	//이름 - 고정
	CUIText::TextDesc itemNameDesc = {};
	itemNameDesc.fontAlign = CUIText::EFontAlign::LEFT;
	itemNameDesc.fontTag = L"Noto_Sans_CJK_SC_32";
	itemNameDesc.text = L"이름";
	itemNameDesc.pivot = (_float2{ 0.f,1.f });
	itemNameDesc.anchorPoint = (_float2{ 0.f,1.f });

	shared_ptr<CUIText> itemNameText = CUIText::Create(m_device, m_context);
	itemNameText->Initialize(&itemNameDesc);
	Add_Child(itemNameText, L"ItemNameText", false);
	m_nameText = itemNameText;

	// 선 - 고정
	CUIImage::UIImageDesc lineDesc = {};
	lineDesc.textureComLevel = ETOI(ELevel::STATIC);
	lineDesc.textureProtoName = L"Prototype_Component_Texture_TabDivider";
	lineDesc.pivot = (_float2{ 0.5f,0.5f });
	lineDesc.anchorPoint = (_float2{ 0.5f,1.f });

	shared_ptr<CUIImage> Line = CUIImage::Create(m_device, m_context);
	Line->Initialize(&lineDesc);
	Add_Child(Line, L"Line", false);
	m_lineImg = Line;


	//오른쪽 왼쪽 - 변동
	CUIText::TextDesc itemLeftDesc = {};
	itemLeftDesc.fontAlign = CUIText::EFontAlign::LEFT;
	itemLeftDesc.fontTag = L"Noto_Sans_CJK_SC";
	itemLeftDesc.text = L"왼쪽";
	itemLeftDesc.pivot = (_float2{ 0.f,1.f });
	itemLeftDesc.anchorPoint = _float2{ 0.f,1.f };
	shared_ptr<CUIText> itemLeftText = CUIText::Create(m_device, m_context);
	itemLeftText->Initialize(&itemLeftDesc);
	Add_Child(itemLeftText, L"ItemLeftText", false);
	m_leftText = itemLeftText;

	CUIText::TextDesc itemRightDesc = {};
	itemRightDesc.fontAlign = CUIText::EFontAlign::RIGHT;
	itemRightDesc.fontTag = L"Noto_Sans_CJK_SC";
	itemRightDesc.text = L"오른쪽";
	itemRightDesc.pivot = (_float2{ 1.f,1.f });
	itemRightDesc.anchorPoint = _float2{ 1.f,1.f };
	shared_ptr<CUIText> itemRightText = CUIText::Create(m_device, m_context);
	itemRightText->Initialize(&itemRightDesc);
	Add_Child(itemRightText, L"ItemRightText", false);
	m_rightText = itemRightText;

	// 상세 설명 - 변동 

	CUIText::TextDesc itemDetailDesc = {};
	itemDetailDesc.fontAlign = CUIText::EFontAlign::CENTER;
	itemDetailDesc.fontTag = L"Noto_Sans_CJK_SC_24";
	itemDetailDesc.text = L"상세 설명";
	itemDetailDesc.textColor = _float4{ 0.6f,0.6f,0.6f,1.f };
	itemDetailDesc.pivot = (_float2{ 0.5f,1.f });
	itemDetailDesc.anchorPoint = (_float2{ 0.5f,1.f });

	shared_ptr<CUIText> itemDetailText = CUIText::Create(m_device, m_context);
	itemDetailText->Initialize(&itemDetailDesc);
	Add_Child(itemDetailText, L"ItemDetailText", false);
	m_detailText = itemDetailText;




	{


		m_vecIcon.resize(EButtonInfo::END);
		for (size_t i = 0; i < EButtonInfo::END; i++)
		{
			EButtonInfo currentBtn = static_cast<EButtonInfo>(i);
			string viewName = string(magic_enum::enum_name(currentBtn));
			wstring wName = S2W(viewName);

			//버튼인포 아이콘 - 변동
			CUIImage::UIImageDesc buttonIconDesc = {};
			buttonIconDesc.textureComLevel = ETOI(ELevel::STATIC);
			buttonIconDesc.textureProtoName = L"Prototype_Component_Texture_KEY";
			buttonIconDesc.textureIndex = 119;
			buttonIconDesc.pivot = (_float2{ 0.5f,1.f });
			buttonIconDesc.anchorPoint = (_float2{ 0.5f,1.f });
			buttonIconDesc.scale = (_float2{ 0.6f,0.6f });
			buttonIconDesc.anchoredPos = _float2{ 10.f,0.f };

			shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
			icon->Initialize(&buttonIconDesc);
			Add_Child(icon, L"icon" + wName, false);
			icon->UI_InActive();

			// 버튼 인포
			CUIText::TextDesc buttonInfoDesc = {};
			buttonInfoDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			buttonInfoDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			buttonInfoDesc.text = L"버튼인포";
			buttonInfoDesc.pivot = _float2{ 1.f,0.5f };
			buttonInfoDesc.anchorPoint = _float2{ 1.f,0.5f };
			buttonInfoDesc.anchoredPos = _float2{ -(icon->GetUITransform()->Get_FinalSize().x + 5.f),0.f };

			shared_ptr<CUIText> buttonInfoText = CUIText::Create(m_device, m_context);
			buttonInfoText->Initialize(&buttonInfoDesc);



			icon->Add_Child(buttonInfoText, wName, false);
			buttonInfoText->UI_InActive();

			m_vecIcon[i] = icon;
		}

	}
	return hr;
}
void CItemInfo::GetButtonInfo(_uint buttonInfo, _uint& texIndex, wstring& str, _float extraInfo)
{
	// 119 - 집기 / 213- z / 123 - 창고 / 53 - 판매 /119 놓기/ 121 회전
	switch (buttonInfo)
	{
	case ETOI(CItemInfo::EButtonInfo::STORAGE):
		texIndex = 123;
		str = L"창고로 보내기";
		return;
	case ETOI(CItemInfo::EButtonInfo::BUY):
		texIndex = 123;
		str = format(L"구매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::EButtonInfo::PICK):
		texIndex = 119;
		str = L"집기";
		return;
	case ETOI(CItemInfo::EButtonInfo::RELEASE):
		texIndex = 119;
		str = L"놓기";
		return;
	case ETOI(CItemInfo::EButtonInfo::ROTATION):
		texIndex = 121;
		str = L"회전";
		return;
	case ETOI(CItemInfo::EButtonInfo::SELL):
		texIndex = 53;
		str = format(L"판매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::EButtonInfo::THROWUP):
		texIndex = 213;
		str = L"버리기";
		return;

	}
}
void CItemInfo::OnActive()
{
	CUIPanel::OnActive();

}

void CItemInfo::OnInActive()
{
	CUIPanel::OnInActive();

}

void CItemInfo::OnDisabled()
{
	CUIPanel::OnDisabled();
}

void CItemInfo::OnUpdate(const _float& timeDelta)
{

	GetUITransform()->SetLocalScale({ 1.4f, m_targetSize.y / GetUITransform()->Get_SizeDelta().y });
	m_lineImg->GetUITransform()->SetLocalScale({ GetUITransform()->Get_FinalSize().x / m_lineImg->GetUITransform()->Get_SizeDelta().x, 0.5f });


	CUIPanel::OnUpdate(timeDelta);
}

void CItemInfo::OnPrepareRender()
{
    CItemInfo::OnUpdate(0.f);
    CItemInfo::OnLateUpdate();
    __super::OnPrepareRender();
}

void CItemInfo::OnLateUpdate()
{
	GetUITransform()->SetAnchoredPos({ 0.f,m_gameInstance.lock()->Get_MousePos().y - (m_gameInstance.lock()->Get_WinSize().Bottom() / 2) });

	CUIPanel::OnLateUpdate();
}

HRESULT CItemInfo::OnRender()
{
	return CUIPanel::OnRender();
}

void CItemInfo::OnClear()
{
	CUIPanel::OnClear();
}

_float CItemInfo::Active_ButtonInfo(EButtonInfo btnInfo, _float2 anchoredPos, _float cost, _float2 pivot)
{
	wstring str = L"";
	_uint texIndex = {};

	anchoredPos.x = 100.f;

	GetButtonInfo(btnInfo, texIndex, str, cost);
	m_vecIcon[ETOI(btnInfo)]->Set_TextureIndex(texIndex);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot(pivot);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetAnchoredPos(anchoredPos);
	auto tex = dynamic_pointer_cast<CUIText>(m_vecIcon[ETOI(btnInfo)]->GetChildren()[0]);
	tex->Set_Text(str);
	m_vecIcon[ETOI(btnInfo)]->UI_Active();
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot({ 0.5f,1.f });

	//m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Set






	return m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Get_FinalSize().y;// 사이즈
}

shared_ptr<CItemInfo> CItemInfo::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CItemInfo> instance(new CItemInfo(device, context), [](CItemInfo* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemInfo");

	}
	return instance;
}

void CItemInfo::Free()
{
	CUIPanel::Free();
}
