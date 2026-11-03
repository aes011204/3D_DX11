#include "UI_Village.h"

#include "UIText.h"

#include "UIButton.h"
#include "UICanvas.h"
#include "UIPanel.h"
#include "UIImage.h"
#include "CScaleModifier.h"
#include "IModifier.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Engine_Helper.h"
#include "EventBus.h"

CUI_Village::CUI_Village(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CUIPanel(device, context)
{
}

CUI_Village::CUI_Village(const CUIPanel& prototype)
	: CUIPanel(prototype)
{
}
void CUI_Village::UI_Active()
{
	CUIPanel::UI_Active();
	for (auto& it : m_selectBoat)
	{
		it->UI_InActive();
	}
	for (auto& it : m_select)
	{
		it->UI_InActive();

	}
}
HRESULT CUI_Village::OnInit(void* arg)
{
	//if(pArg==nullptr)

	VillageDesc* desc = static_cast<VillageDesc*>(arg);
	HRESULT hr = {};
	hr = __super::OnInit(desc);


	// 람다 제외 전부 데이터로 수정 가능

	wstring name = {};
	EVillageLocation level = {};
	EPlayerState playerState = {};
	wstring tex = {};
	wstring sound = {};
	for (_uint i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			level = EVillageLocation::REPAIR;
			name = L"조선공";
			tex = L"Prototype_Component_Texture_MerchantIcon";
			playerState = EPlayerState::REPAIR_SHOP;
			sound = L"Repair_Visit";
			break;
		case 1:
			level = EVillageLocation::FISH;
			name = L"생선장수";
			tex = L"Prototype_Component_Texture_FishIconVill";
			playerState = EPlayerState::FISH_SHOP;
			sound = L"Fishmonger_Visit";

			break;
		case 2:
			level = EVillageLocation::STORAGE;
			name = L"내창고";

			tex = L"Prototype_Component_Texture_StorageIcon";
			playerState = EPlayerState::STORAGE;
			sound = L"Storage_Visit";

			break;
			/*		case 3:
						eLevel = VIllAGE_LOCATION::BOAT;
						name = L"";

						break*/;
		}

		CUIImage::UIImageDesc underBarDesc{};
		underBarDesc.textureComLevel = ETOI(ELevel::STATIC);
		underBarDesc.textureProtoName = L"Prototype_Component_Texture_VillageUnderBar";
		shared_ptr<CUIImage> underBar = CUIImage::Create(m_device, m_context);
		underBar->Initialize(&underBarDesc);

		wstring nameTag = L"UnderBar_" + S2W(string(magic_enum::enum_name(level)));
		Add_Child(underBar, nameTag, false);


		{


			CUIButton::UIButtonDesc desc = {};
			desc.textureComLevel = ETOI(ELevel::STATIC);
			desc.textureProtoName = L"Prototype_Component_Texture_Button";
			desc.typeIndex = i;
			desc.OverlapStartEvent = [&](CUIButton* self) {auto& ch = self->GetChildren();
				m_select[self->Get_TypeIndex()]->UI_Active();
				_float2 tmp = { 4.f, 2.f };
				m_select[self->Get_TypeIndex()]->m_behavior.push_back((make_shared<CScaleModifier>(0.05f, 4.f, 0.f, tmp)));

				m_gameInstance.lock()->Play_Once(L"Hover");
				};
			desc.OverlapEndEvent = [&](CUIButton* self) {auto& ch = self->GetChildren();
			
				m_select[self->Get_TypeIndex()]->UI_InActive();
				m_select[self->Get_TypeIndex()]->m_behavior.clear();
		
				};

			desc.ClickEvent = [this, playerState,sound](CUIButton* self)
				{

					/*m_Select[pThis->Get_TypeIndex()]->UI_InActive();
					m_Select[pThis->Get_TypeIndex()]->m_behavior.clear();*/
					Evt_ChangeState event{};
					event.playerstate = playerState;

					CGameInstance::GetInstance()->Get_EventBus()->Publish(event);

					m_gameInstance.lock()->Play_Once(sound);

				};
			shared_ptr<CUIButton> but = CUIButton::Create(m_device, m_context);
			but->Initialize(&desc);
			but->Set_SelectState(false);

			//
			{
				CUIImage::UIImageDesc selectImage{};
				selectImage.textureComLevel = ETOI(ELevel::STATIC);
				selectImage.textureProtoName = L"Prototype_Component_Texture_Select";
				selectImage.pxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
				selectImage.useNineSlice = true;
				shared_ptr<CUIImage> Select = CUIImage::Create(m_device, m_context);
				Select->Initialize(&selectImage);
				Select->UI_InActive();
				wstring nameTag1 = L"Select_" + S2W(string(magic_enum::enum_name(level)));
				but->Add_Child(Select, nameTag1, false);
				m_select[i] = Select;




				CUIText::TextDesc nameTextDesc = {};
				nameTextDesc.fontTag = L"Noto_Sans_CJK_SC";
				nameTextDesc.text = name;
				shared_ptr<CUIText> nameText = CUIText::Create(m_device, m_context);
				nameText->Initialize(&nameTextDesc);

				but->Add_Child(nameText, L"text_" + S2W(string(magic_enum::enum_name(level))), false);


				CUIImage::UIImageDesc iconImage{};
				iconImage.textureComLevel = ETOI(ELevel::STATIC);
				iconImage.textureProtoName = tex;
				shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
				icon->Initialize(&iconImage);

				wstring nameTag2 = L"icon_" + S2W(string(magic_enum::enum_name(level)));
				but->Add_Child(icon, nameTag2, false);

			}

		nameTag = L"BUTTON_" + S2W(string(magic_enum::enum_name(level)));
		underBar->Add_Child(but, nameTag, false);
		}


	}

	CUIImage::UIImageDesc underBarDesc{};
	underBarDesc.textureComLevel = ETOI(ELevel::STATIC);
	underBarDesc.textureProtoName = L"Prototype_Component_Texture_VillageUnderBar";
	shared_ptr<CUIImage> underBar = CUIImage::Create(m_device, m_context);
	underBar->Initialize(&underBarDesc);

	Add_Child(underBar, L"UnderBar_Boat", false);


	//


	////////

	for (_uint i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			tex = L"Prototype_Component_Texture_UndockIcon";
			sound = L"Undocked";
			break;
		case 1:
			tex = L"Prototype_Component_Texture_SleepIcon";
			sound = L"Click";

			break;
		case 2:
			tex = L"Prototype_Component_Texture_cog_icon";
			sound = L"Click";

			break;
		}

		CUIButton::UIButtonDesc desc = {};
		desc.textureComLevel = ETOI(ELevel::STATIC);
		desc.textureProtoName = L"Prototype_Component_Texture_Button";
		desc.typeIndex =i;
		desc.OverlapStartEvent = [&](CUIButton* self) {auto& ch = self->GetChildren();
		
			m_selectBoat[self->Get_TypeIndex()]->UI_Active();
			
			_float2 tmp = { 1.3f, 3.6f };
			
			m_selectBoat[self->Get_TypeIndex()]->m_behavior.push_back((make_shared<CScaleModifier>(0.05f, 4.f, 0.f, tmp)));
			m_selectBoat[self->Get_TypeIndex()]->Set_Zorder(4);
			m_gameInstance.lock()->Play_Once(L"Hover");

			};
		desc.OverlapEndEvent = [&](CUIButton* self) {auto& ch = self->GetChildren();
		
			m_selectBoat[self->Get_TypeIndex()]->UI_InActive();
			m_selectBoat[self->Get_TypeIndex()]->m_behavior.clear();
			m_selectBoat[self->Get_TypeIndex()]->Set_Zorder(2);
	
			};
		desc.ClickEvent = [this,i,sound](CUIButton* self) {
			Evt_VillageBtn e = {};
			e.type = i;
				m_gameInstance.lock()->Get_EventBus()->Publish(e);
				m_gameInstance.lock()->Play_Once(sound);

			};
		shared_ptr<CUIButton> boatOP = CUIButton::Create(m_device, m_context);
		boatOP->Initialize(&desc);
		boatOP->Set_ColorFlat(_float4{0.f,0.f,0.f,1.f});
		underBar->Add_Child(boatOP, L"BoatOP" + to_wstring(i), false);
		boatOP->Set_SelectState(false);


		{
			CUIImage::UIImageDesc selectImage{};
			selectImage.textureComLevel = ETOI(ELevel::STATIC);
			selectImage.textureProtoName = L"Prototype_Component_Texture_Select";
			selectImage.pxSliceLRTB = _float4{ 46.f,46.f,22.f,22.f };
			selectImage.useNineSlice = true;
			shared_ptr<CUIImage> Select = CUIImage::Create(m_device, m_context);
			Select->Initialize(&selectImage);
			Select->UI_InActive();
			Select->Set_Zorder(4);
			wstring nameTag1 = L"select_" + to_wstring(i);
			/*BoatOP->*/underBar->Add_Child(Select, nameTag1, false);
			m_selectBoat[i] = Select;


			CUIImage::UIImageDesc iconImage{};
			iconImage.textureComLevel = ETOI(ELevel::STATIC);
			iconImage.textureProtoName = tex;
			shared_ptr<CUIImage> icon = CUIImage::Create(m_device, m_context);
			icon->Initialize(&iconImage);
			
			wstring nameTag2 = L"icon_" + to_wstring(i);
			boatOP->Add_Child(icon, nameTag2, false);
		}
	}



	CUIImage::UIImageDesc titleDesc{};
	titleDesc.textureComLevel = ETOI(ELevel::STATIC);
	titleDesc.textureProtoName = L"Prototype_Component_Texture_TitleBackground";
	shared_ptr<CUIImage> Title = CUIImage::Create(m_device, m_context);
	Title->Initialize(&titleDesc);
	Add_Child(Title, L"Title", false);

	CUIText::TextDesc textVillName = {};
	textVillName.fontTag = L"Noto_Sans_CJK_SC_32";
	textVillName.text = L"큰골마을";
	shared_ptr<CUIText> textVill = CUIText::Create(m_device, m_context);
	textVill->Initialize(&textVillName);

	Title->Add_Child(textVill, L"text_Vill", false);



	return hr;
}

void CUI_Village::OnActive()
{
	__super::OnActive();
	
}

void CUI_Village::OnInActive()
{
	__super::OnInActive();
}

void CUI_Village::OnDisabled()
{
	__super::OnDisabled();
}

void CUI_Village::OnUpdate(const _float& timeDelta)
{
	
	//for (auto& it : m_Select_boat)
	//{
	//	it->Set_Zorder(4);
	//}

	__super::OnUpdate(timeDelta);
}

void CUI_Village::OnLateUpdate()
{
	__super::OnLateUpdate();

}

HRESULT CUI_Village::OnRender()
{
	HRESULT hr = {};
	hr = __super::OnRender();

	return hr;
}

void CUI_Village::OnClear()
{
	__super::OnClear();

}



shared_ptr<CUI_Village> CUI_Village::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	shared_ptr<CUI_Village> instance(new CUI_Village(device, context), [](CUI_Village* p) {p->Free(); delete(p); });

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIPanel");

	}
	return instance;
}

void CUI_Village::Free()
{
	__super::Free();
}
