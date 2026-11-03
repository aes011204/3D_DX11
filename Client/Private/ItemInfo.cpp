#include "ItemInfo.h"

#include <UIImage.h>

#include "ItemDB.h"
#include "UIText.h"
#include "EventBus.h"

CItemInfo::CItemInfo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUIPanel(pDevice, pContext)
{
}

CItemInfo::CItemInfo(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}

void CItemInfo::UI_PanelActive(_bool isHold, ItemInst itemInst, EPlayerState PlayerState, _bool IsPlayer)
{
	wstring NameInfo = L"";
	wstring LeftInfo = L"";
	wstring RightInfo = L"";
	wstring DescInfo = L"";
	//wstring KeyInfo = L"";



	if (itemInst.itemInstId == ID_Absence)
	{

		UI_InActive();
		return;
	}

	float cost = {};

	float fCurrentY = -m_NamePadding;

	ItemDef itemDef = CItemDB::GetInstance()->GetItemByID(itemInst.itemDefId);

	switch (itemInst.itemType)
	{
	case EItemType::FISH:

		if (FishInst* fishInst = get_if<FishInst>(&itemInst.typeDef))
		{
			FishDef* fishDef = get_if<FishDef>(&itemDef.typeDef);

			if (fishInst->mutationId == ID_Absence)
			{
				NameInfo = S2W(itemDef.itemName);
				DescInfo = S2W(itemDef.itemDesc);
				cost = fishDef->cost;
			}
			else
			{
				const int mutationIndex = static_cast<int>(fishInst->mutationId) - 1;

				if (mutationIndex >= 0 && mutationIndex < static_cast<int>(fishDef->vecMutation.size()))
				{
					NameInfo = S2W(fishDef->vecMutation[mutationIndex].mutName);
					DescInfo = S2W(fishDef->vecMutation[mutationIndex].mutDesc);
					cost = fishDef->vecMutation[mutationIndex].mutCost;
				}
				else
				{
					NameInfo = S2W(itemDef.itemName);
					DescInfo = S2W(itemDef.itemDesc);
					cost = fishDef->cost;
				}
			}

			LeftInfo = L"크기:\n상태:\n유형:";
			RightInfo = format(L"{:.2f}cm\n{}\n{}", fishInst->size, freshToWstr(fishInst->freshness), OcceanToWstr(fishDef->seaTypeMask));
		}
		if (isHold == true)
			break;
		break;
	case EItemType::EQUIP:
	{

		EquipInst* equipInst = get_if<EquipInst>(&itemInst.typeDef);
		EquipDef* equipDef = get_if<EquipDef>(&itemDef.typeDef);

		NameInfo = S2W(itemDef.itemName);

		DescInfo = S2W(itemDef.itemDesc);
		cost = equipDef->cost;

		if (isHold == true)
			break;

		if (EquipEngine* equipEngine = get_if<EquipEngine>(&equipDef->equipTypeEffect))
		{
			LeftInfo = L"설치 시간:\n상태:\n속도:";
			RightInfo = format(L"{}h\n{}\n+{}kn", equipDef->installTime, GetBroken(equipInst->isBroken), equipEngine->speedKn);
		}
		else if (EquipLight* equipLight = get_if<EquipLight>(&equipDef->equipTypeEffect))
		{
			LeftInfo = L"설치 시간:\n상태:\n루멘:\n범위";
			RightInfo = format(L"{}h\n{}\n+{}lm\n{}m", equipDef->installTime, GetBroken(equipInst->isBroken), equipLight->lumen, equipLight->range);
		}
		else if (EquipRod* equipRod = get_if<EquipRod>(&equipDef->equipTypeEffect))
		{
			LeftInfo = L"설치 시간:\n상태:\n낚시 속도:\n어종";
			RightInfo = format(L"{}h\n{}\n+{}%\n{}", equipDef->installTime, GetBroken(equipInst->isBroken), equipRod->speedPer, OcceanToWstr(equipRod->seaTypeMask));
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
		NameInfo = S2W(itemDef.itemName);
		DescInfo = S2W(itemDef.itemDesc);
		cost = matDef->cost;
		break;

	}
	}
	m_NameText->Set_Text(NameInfo); //위치조정 필요 없음
	if (m_NameText->Get_UIState() != EUIState::ACTIVE)
		m_NameText->UI_Active();
	m_NameText->GetUITransform()->SetAnchoredPos({ m_PaddingX, fCurrentY });
	fCurrentY -= (m_NameText->Get_TextSize().y + m_NamePadding);

	if (m_LineImg->Get_UIState() != EUIState::ACTIVE)
		m_LineImg->UI_Active();
	m_LineImg->GetUITransform()->SetAnchoredPos({ 0, fCurrentY });
	fCurrentY -= (m_LineImg->GetUITransform()->Get_SizeDelta().y + m_PaddingY);

	m_LeftText->Set_Text(LeftInfo);
	m_LeftText->GetUITransform()->SetAnchoredPos({ m_PaddingX, fCurrentY });
	if (m_LeftText->Get_UIState() != EUIState::ACTIVE)
		m_LeftText->UI_Active();

	m_RightText->Set_Text(RightInfo);
	m_RightText->GetUITransform()->SetAnchoredPos({ -m_PaddingX, fCurrentY });
	if (m_RightText->Get_UIState() != EUIState::ACTIVE)
		m_RightText->UI_Active();
	fCurrentY -= (m_LeftText->Get_TextSize().y + m_PaddingY);


	m_DetailText->Set_Text(DescInfo);
	m_DetailText->GetUITransform()->SetAnchoredPos({ -m_PaddingX, fCurrentY });
	if (m_DetailText->Get_UIState() != EUIState::ACTIVE)
		m_DetailText->UI_Active();
	fCurrentY -= (m_DetailText->Get_TextSize().y + m_PaddingY);

	for (auto& infoBtn : m_vecIcon)
	{
		if (infoBtn->Get_UIState() == EUIState::ACTIVE)
			infoBtn->UI_InActive();
	}


	float sizeY = {};
	if (isHold == true)
	{

		switch (PlayerState)
		{
		case EPlayerState::FISHING:
		case EPlayerState::SEA:
			// 놓기 회전 버리기
			sizeY = Active_ButtonInfo(BUTTONINFO::RELEASE, { 40.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::ROTATION, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			break;
		case EPlayerState::REPAIR_SHOP:
		{
			//설치 회전 창고로보내기 환불


			sizeY = Active_ButtonInfo(BUTTONINFO::RELEASE, { 40.f,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::ROTATION, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			if (itemInst.itemType == EItemType::EQUIP)
			{
				sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 40.f, fCurrentY }, cost*.5f);
				fCurrentY -= (sizeY + m_PaddingY);
			}
			break;
		}
		case EPlayerState::FISH_SHOP:

		{


			//설치 회전 창고로보내기 환불

			sizeY = Active_ButtonInfo(BUTTONINFO::RELEASE, { 40.f,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::ROTATION, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);


			sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 40.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			if (itemInst.itemType == EItemType::FISH)
			{
				sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 40.f, fCurrentY }, cost);
				fCurrentY -= (sizeY + m_PaddingY);

			}


			break;
		}
		}
	}
	else
	{

		switch (PlayerState)
		{
		case EPlayerState::FISHING:
		case EPlayerState::SEA:
			//집기 버리기
			sizeY = Active_ButtonInfo(BUTTONINFO::PICK, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 50.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			break;
			//case E_PLAYERSTATE::REPAIR_SHOP: // 이거 구매 측
			//	//구매
			//	sizeY = Active_ButtonInfo(BUTTONINFO::BUY, { 50.f ,fCurrentY }, cost);
			//	fCurrentY -= (sizeY + m_PaddingY);

			//break;
		case EPlayerState::REPAIR_SHOP:
			if (IsPlayer)
			{
				sizeY = Active_ButtonInfo(BUTTONINFO::PICK, { 50.f ,fCurrentY });
				fCurrentY -= (sizeY + m_PaddingY);

				sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 50.f, fCurrentY });
				fCurrentY -= (sizeY + m_PaddingY);
				sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 50.f ,fCurrentY });
				fCurrentY -= (sizeY + m_PaddingY);
				if (itemInst.itemType == EItemType::EQUIP)
				{
					sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 50.f, fCurrentY }, cost * 0.5f);
					fCurrentY -= (sizeY + m_PaddingY);

				}
			}
			else
			{


				sizeY = Active_ButtonInfo(BUTTONINFO::BUY, { 50.f, fCurrentY }, cost);
				fCurrentY -= (sizeY + m_PaddingY);
			}
			//집기 버리기 창고로보내기 판매

			break;


		case EPlayerState::FISH_SHOP:
			//집기 버리기 창고로보내기 판매
			sizeY = Active_ButtonInfo(BUTTONINFO::PICK, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);

			sizeY = Active_ButtonInfo(BUTTONINFO::THROWUP, { 50.f, fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			sizeY = Active_ButtonInfo(BUTTONINFO::STORAGE, { 50.f ,fCurrentY });
			fCurrentY -= (sizeY + m_PaddingY);
			if (itemInst.itemType == EItemType::FISH)
			{
				sizeY = Active_ButtonInfo(BUTTONINFO::SELL, { 50.f, fCurrentY }, cost);
				fCurrentY -= (sizeY + m_PaddingY);
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
	m_Targetsize.y = abs(fCurrentY) + m_PaddingY;
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

HRESULT CItemInfo::OnInit(void* pArg)
{


	HRESULT hr = E_FAIL;
	ITEMINFO_DESC* INFODesc = static_cast<ITEMINFO_DESC*>(pArg);
	INFODesc->useNineSlice = true;
	INFODesc->textureComLevel = ETOI(LEVEL::STATIC);
	INFODesc->textureProtoName = L"Prototype_Component_Texture_PopupBackground";
	INFODesc->pivot = (_float2{ 0.f,0.f });
	hr = CUIPanel::OnInit(INFODesc);


	//이름 - 고정
	CUIText::TextDesc ItemNameDesc = {};
	ItemNameDesc.fontAlign = CUIText::EFontAlign::LEFT;
	ItemNameDesc.fontTag = L"Noto_Sans_CJK_SC_32";
	ItemNameDesc.text = L"이름";
	ItemNameDesc.pivot = (_float2{ 0.f,1.f });
	ItemNameDesc.anchorPoint = (_float2{ 0.f,1.f });

	shared_ptr<CUIText> ItemNameText = CUIText::Create(m_device, m_context);
	ItemNameText->Initialize(&ItemNameDesc);
	Add_Child(ItemNameText, L"ItemNameText", false);
	m_NameText = ItemNameText;

	// 선 - 고정
	CUIImage::UIImageDesc LineDesc = {};
	LineDesc.textureComLevel = ETOI(LEVEL::STATIC);
	LineDesc.textureProtoName = L"Prototype_Component_Texture_TabDivider";
	LineDesc.pivot = (_float2{ 0.5f,0.5f });
	LineDesc.anchorPoint = (_float2{ 0.5f,1.f });

	shared_ptr<CUIImage> Line = CUIImage::Create(m_device, m_context);
	Line->Initialize(&LineDesc);
	Add_Child(Line, L"Line", false);
	m_LineImg = Line;


	//오른쪽 왼쪽 - 변동
	CUIText::TextDesc ItemLeftDesc = {};
	ItemLeftDesc.fontAlign = CUIText::EFontAlign::LEFT;
	ItemLeftDesc.fontTag = L"Noto_Sans_CJK_SC";
	ItemLeftDesc.text = L"왼쪽";
	ItemLeftDesc.pivot = (_float2{ 0.f,1.f });
	ItemLeftDesc.anchorPoint = _float2{ 0.f,1.f };
	shared_ptr<CUIText> ItemLeftText = CUIText::Create(m_device, m_context);
	ItemLeftText->Initialize(&ItemLeftDesc);
	Add_Child(ItemLeftText, L"ItemLeftText", false);
	m_LeftText = ItemLeftText;

	CUIText::TextDesc ItemRightDesc = {};
	ItemRightDesc.fontAlign = CUIText::EFontAlign::RIGHT;
	ItemRightDesc.fontTag = L"Noto_Sans_CJK_SC";
	ItemRightDesc.text = L"오른쪽";
	ItemRightDesc.pivot = (_float2{ 1.f,1.f });
	ItemRightDesc.anchorPoint = _float2{ 1.f,1.f };
	shared_ptr<CUIText> ItemRightText = CUIText::Create(m_device, m_context);
	ItemRightText->Initialize(&ItemRightDesc);
	Add_Child(ItemRightText, L"ItemRightText", false);
	m_RightText = ItemRightText;

	// 상세 설명 - 변동 

	CUIText::TextDesc ItemDetailDesc = {};
	ItemDetailDesc.fontAlign = CUIText::EFontAlign::CENTER;
	ItemDetailDesc.fontTag = L"Noto_Sans_CJK_SC_24";
	ItemDetailDesc.text = L"상세 설명";
	ItemDetailDesc.textColor = _float4{ 0.6f,0.6f,0.6f,1.f };
	ItemDetailDesc.pivot = (_float2{ 0.5f,1.f });
	ItemDetailDesc.anchorPoint = (_float2{ 0.5f,1.f });

	shared_ptr<CUIText> ItemDetailText = CUIText::Create(m_device, m_context);
	ItemDetailText->Initialize(&ItemDetailDesc);
	Add_Child(ItemDetailText, L"ItemDetailText", false);
	m_DetailText = ItemDetailText;




	{


		m_vecIcon.resize(BUTTONINFO::END);
		for (size_t i = 0; i < BUTTONINFO::END; i++)
		{
			BUTTONINFO currentBtn = static_cast<BUTTONINFO>(i);
			string viewName = string(magic_enum::enum_name(currentBtn));
			wstring wName = S2W(viewName);

			//버튼인포 아이콘 - 변동
			CUIImage::UIImageDesc ButtonIconDesc = {};
			ButtonIconDesc.textureComLevel = ETOI(LEVEL::STATIC);
			ButtonIconDesc.textureProtoName = L"Prototype_Component_Texture_KEY";
			ButtonIconDesc.textureIndex = 119;
			ButtonIconDesc.pivot = (_float2{ 0.5f,1.f });
			ButtonIconDesc.anchorPoint = (_float2{ 0.5f,1.f });
			ButtonIconDesc.scale = (_float2{ 0.6f,0.6f });
			ButtonIconDesc.anchoredPos = _float2{ 10.f,0.f };

			shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
			icon->Initialize(&ButtonIconDesc);
			Add_Child(icon, L"icon" + wName, false);
			icon->UI_InActive();

			// 버튼 인포
			CUIText::TextDesc ButtonInfoDesc = {};
			ButtonInfoDesc.fontAlign = CUIText::EFontAlign::RIGHT;
			ButtonInfoDesc.fontTag = L"Noto_Sans_CJK_SC_24";
			ButtonInfoDesc.text = L"버튼인포";
			ButtonInfoDesc.pivot = _float2{ 1.f,0.5f };
			ButtonInfoDesc.anchorPoint = _float2{ 1.f,0.5f };
			ButtonInfoDesc.anchoredPos = _float2{ -(icon->GetUITransform()->Get_FinalSize().x + 5.f),0.f };

			shared_ptr<CUIText> ButtonInfoText = CUIText::Create(m_device, m_context);
			ButtonInfoText->Initialize(&ButtonInfoDesc);



			icon->Add_Child(ButtonInfoText, wName, false);
			ButtonInfoText->UI_InActive();

			m_vecIcon[i] = icon;
		}

	}
	return hr;
}
void CItemInfo::GetButtonInfo(_uint buttonInfo, _uint& Texindex, wstring& str, _float extraInfo)
{
	// 119 - 집기 / 213- z / 123 - 창고 / 53 - 판매 /119 놓기/ 121 회전
	switch (buttonInfo)
	{
	case ETOI(CItemInfo::BUTTONINFO::STORAGE):
		Texindex = 123;
		str = L"창고로 보내기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::BUY):
		Texindex = 123;
		str = format(L"구매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::BUTTONINFO::PICK):
		Texindex = 119;
		str = L"집기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::RELEASE):
		Texindex = 119;
		str = L"놓기";
		return;
	case ETOI(CItemInfo::BUTTONINFO::ROTATION):
		Texindex = 121;
		str = L"회전";
		return;
	case ETOI(CItemInfo::BUTTONINFO::SELL):
		Texindex = 53;
		str = format(L"판매 [${:.2f}]", extraInfo);
		return;
	case ETOI(CItemInfo::BUTTONINFO::THROWUP):
		Texindex = 213;
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

	GetUITransform()->SetLocalScale({ 1.4f, m_Targetsize.y / GetUITransform()->Get_SizeDelta().y });
	m_LineImg->GetUITransform()->SetLocalScale({ GetUITransform()->Get_FinalSize().x / m_LineImg->GetUITransform()->Get_SizeDelta().x, 0.5f });


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

_float CItemInfo::Active_ButtonInfo(BUTTONINFO btnInfo, _float2 AnchoredPos, _float cost, _float2 Pivot)
{
	wstring str = L"";
	_uint TexIndex = {};

	AnchoredPos.x = 100.f;

	GetButtonInfo(btnInfo, TexIndex, str, cost);
	m_vecIcon[ETOI(btnInfo)]->Set_TextureIndex(TexIndex);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot(Pivot);
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetAnchoredPos(AnchoredPos);
	auto tex = dynamic_pointer_cast<CUIText>(m_vecIcon[ETOI(btnInfo)]->GetChildren()[0]);
	tex->Set_Text(str);
	m_vecIcon[ETOI(btnInfo)]->UI_Active();
	m_vecIcon[ETOI(btnInfo)]->GetUITransform()->SetPivot({ 0.5f,1.f });

	//m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Set






	return m_vecIcon[ETOI(btnInfo)]->GetUITransform()->Get_FinalSize().y;// 사이즈
}

shared_ptr<CItemInfo> CItemInfo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CItemInfo> pInstance(new CItemInfo(pDevice, pContext), [](CItemInfo* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CItemInfo");

	}
	return pInstance;
}

void CItemInfo::Free()
{
	CUIPanel::Free();
}
