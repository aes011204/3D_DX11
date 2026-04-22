#include "UI.h"

#include "DInput_Manager.h"
#include "GameInstance.h"
#include "UITransform.h" 
#include "IModifier.h"
#include "Engine_Helper.h"

CUI::CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CEntity(pDevice, pContext)
{
}

CUI::CUI(const CUI& prototype)
	: CEntity(prototype),
	m_ZOrder(prototype.m_ZOrder),
	m_bLayoutTarget(prototype.m_bLayoutTarget)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;

}

// UI는 프로토 타입으로 아니셜라이 프로토타입으로 호출하고 직접Initalize 하니까 
HRESULT CUI::Initialize(void* pArg)
{

	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	Set_Zorder(pDesc->ZOrder);
	m_NO_DESTACH = pDesc->NoDISTACH;

	if (m_bInitialized == true)
		return S_OK;

	SetDefaultNameFromThisType();

	{
		m_pUITransformCom = CUITransform::Create(m_pDevice, m_pContext);
		if (nullptr == m_pUITransformCom)
			return E_FAIL;

		if (FAILED(m_pUITransformCom->Initialize(pArg)))
			return E_FAIL;

		m_pUITransformCom->SetDefaultNameFromThisType();

		if (nullptr == Get_Component(g_strUITransformTag))
		{
			m_Components.emplace(g_strUITransformTag, m_pUITransformCom);

		}
	}


	OnInit(pArg);


	// 최하위fianl 자식의 OnInit 호출 -> 
	// 자신의 부모가 UI이면 __super::OnInit 안해도됨 (해도 됨 {}이거임)
	// 아닌경우는 해줘야함 

	//for (auto& it : m_Children)
	//{
	//	it->Initialize(pArg); // 이거 ㄱㅊ?? 이미 자식을 밖에서 만들어서 이니셜라이즈 다하고 여기 붙히는거 아닌가?
	//}// 마즘 지금 2번돔 근데 이니셜라이즈 안에서 방어 해놧음


	//// 일단 초기화 -> CUi로 올림 
	//_float2 orignSize = m_pTextureCom->Get_SizeFromSRV(0);
	//m_SliceDesc.TexSize = orignSize;
	//m_SliceDesc.UISize = _float2(1.f, 1.f);// 어짜피 트렌스폼이 정함 최종 ui 사이즈
	//m_SliceDesc.PxSliceLRTB = _float4(orignSize.x / 3.f, orignSize.x / 3.f, orignSize.y / 3.f, orignSize.y / 3.f);


	//if (pDesc->PxSliceLRTB.x != 0.f && pDesc->PxSliceLRTB.y != 0.f
	//	&& pDesc->PxSliceLRTB.z != 0.f && pDesc->PxSliceLRTB.w != 0.f)
	//{
	//	m_SliceDesc.PxSliceLRTB = (pDesc->PxSliceLRTB);
	//}

	//m_bUseNineSlice = pDesc->bUseNineSlice;

	//// 행렬  //////////////////////////////////////
	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewPort = { 1 };
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPortDesc);

	float ViewportWidth = ViewPortDesc.Width;
	float ViewportHeight = ViewPortDesc.Height;

	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::VIEW)], XMMatrixIdentity());
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], XMMatrixOrthographicOffCenterLH(0.f, ViewportWidth,
		0.f, ViewportHeight,
		0.f, 1.f));

	//XMMatrixOrthographicOffCenterLH(0.f, ViewportWidth, ViewportHeight, 0.f, 0.f, 1.f);
	//XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], 
	//XMMatrixOrthographicOffCenterLH(
	//	0.f,              // Left
	//	ViewportWidth,    // Right
	//	ViewportHeight,   // Bottom (큰 값)
	//	0.f,              // Top (0)
	//	0.f, 1.f
	//));

	m_bInitialized = true;

	return S_OK;
}

HRESULT CUI::Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ETOI(eTransformState)]);
}

weak_ptr<CUI> CUI::Find_Children(_wstring strTag)
{
	auto it = m_mapChildren.find(strTag);
	if (it == m_mapChildren.end())
		return weak_ptr<CUI>(); // 빈 weak_ptr -> 널체크는 lock() 하면 주인 shared ptr 없으면 널 됨

	return it->second;

}

void CUI::Save_ToJson(nlohmann::json& j)
{
	float m_fX = {};
	float m_fY = {};
	if (!std::isnan(m_fX) && !std::isinf(m_fX)) j["X"] = m_fX;
	if (!std::isnan(m_fY) && !std::isinf(m_fY)) j["Y"] = m_fY;
	//CUI 데이터
	if (m_ZOrder != 1)
		j["ZOrder"] = m_ZOrder;

	j["LayoutTarget"] = m_bLayoutTarget;

	// Componet데이터 저장 
	nlohmann::json jComponentArray = nlohmann::json::array();
	for (auto& pair : Get_ComponentMap())
	{
		nlohmann::json jCom;
		// 컴포넌트에 공통적으로 들어가는거
		jCom["ComProtoTag"] = W2S(pair.second->Get_ProtoTag());
		jCom["ComProtoLevel"] = pair.second->Get_ProtoLevel();
		jCom["ComponentTag"] = W2S(pair.first);
		// 각 컴포넌트 안의 세부내용
		pair.second->Save_ToJson(jCom);
		jComponentArray.push_back(jCom);
	}
	j["Components"] = jComponentArray;


	// 자식 재귀 시작
	nlohmann::json jChildrenArray = nlohmann::json::array();
	for (auto& pair : m_mapChildren)
	{
		auto pChild = pair.second.lock();
		if (nullptr == pChild) continue;

		nlohmann::json jChild;

		jChild["UIChildrenTag"] = W2S(pair.first);
		pair.second.lock()->Save_ToJson(jChild);

		jChildrenArray.push_back(jChild);
	}
	j["Children"] = jChildrenArray;
}

void CUI::Load_FromJson(nlohmann::json& j)
{
	if (j.contains("ZOrder")) {
		m_ZOrder = j["ZOrder"];
	}

	/*if (j.contains("LayoutTarget")) {
		m_bLayoutTarget = j["LayoutTarget"];
	}*/

	if (j.contains("Components") && j["Components"].is_array())
	{
		//Components배열 컨테이너가 있으면 엔티티에서 다 돌리면서 추가나(replace)해줌
		CEntity::Load_FromJson(j);
	}

	if (j.contains("Children") && j["Children"].is_array())
	{
		for (auto& jChild : j["Children"])
		{
			if (!jChild.contains("UIChildrenTag")) continue;

			string childTagStr = jChild["UIChildrenTag"];
			_wstring childTagW = S2W(childTagStr);

			// 내 자식 맵에서 태그로 찾음
			auto it = m_mapChildren.find(childTagW);
			if (it != m_mapChildren.end())
			{
				auto pChild = it->second.lock();
				if (pChild)
				{
					// 자식도 똑같이 이 함수를 타게 함
					pChild->Load_FromJson(jChild);
				}
			}
		}
	}
}

void CUI::OnGui()
{
	ImGui::InputInt("ZOrder Control", &m_ZOrder);
	ImGui::Text("bLayoutTarget : %s", m_bLayoutTarget ? "true" : "false");
	ImGui::Text("UIState : %s", magic_enum::enum_name(m_UIState).data());

}

void CUI::Update(_float fTimeDelta, bool& bMouseHold)
{



	if (m_bEnabled)
	{
		if (m_pUITransformCom)
			m_pUITransformCom->UpdateLayoutIfDirty();

		for (auto& it : m_Children)
		{
			it->Update(fTimeDelta, bMouseHold);
		}


		if (bMouseHold == false && m_bInteractable == true)
		{
			Vector2 mousePos = m_pGameInstance.lock()->Get_DInput_Manger()->Get_MousePos();
			if (true == m_pUITransformCom->Get_WorldRect().Contains(mousePos.x, mousePos.y))
			{
				m_bHovered = true;
				bMouseHold = true;
			}
			else
				m_bHovered = false;
		}
		else
			m_bHovered = false;


		OnUpdate(fTimeDelta);


	
		if (!m_bRenderReady)
			m_bRenderReady = true;


		//for (auto& it : m_Children)
		//{
		//	it->Late_Update(fTimeDelta);
		//}
	}

}

void CUI::Late_Update(_float fTimeDelta)
{
	if (m_bEnabled)
	{
		OnLateUpdate();

		//for (auto& it : m_Children)
		//{
		//	it->Late_Update(fTimeDelta);
		//}

		{
			for (auto it = m_Children.begin(); it != m_Children.end(); )
			{
				if (nullptr != *it)
					(*it)->Late_Update(fTimeDelta);

				if ((*it)->Is_PendingDestroy() == true)
				{
				
					it = m_Children.erase(it);
				}
				else
					it++;

			}
		}

		for (auto it = m_behavior.begin(); it != m_behavior.end(); )
		{
			(*it)->Tick(fTimeDelta, this);

			
			if ((*it)->IsFinished() == true)
			{
				it = m_behavior.erase(it);
			}
			else
			{
				it++;
			}
		}


	}
}

HRESULT CUI::Render()
{
	if (m_bVisible && m_bRenderReady == true)
	{
		if (m_bIsDirty_Zorder)
		{
			if (m_Children.size() >= 2)
			{
				stable_sort(m_Children.begin(), m_Children.end(), [](const shared_ptr<CUI>& a, const shared_ptr<CUI>& b) {
					//if(a->m_ZOrder==b->m_ZOrder)
					//	return 
					return a->m_ZOrder < b->m_ZOrder;
					});
				m_bIsDirty_Zorder = false;
			}
		}
		OnRender();

		for (auto& it : m_Children)
		{
			it->Render();
		}
	}
	//m_bRenderReady = true;
	return S_OK;
}

void CUI::UI_Active()
{
	m_UIState = UI_STATE::ACTIVE;

	m_bEnabled = true;
	m_bVisible = true;

	m_bRenderReady = false;

	if (m_pUITransformCom)
		m_pUITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	for (auto& it : m_Children)
	{
		it->UI_Active();
	}
}

void CUI::UI_InActive()
{
	m_UIState = UI_STATE::INACTIVE;

	m_bEnabled = false;
	m_bVisible = false; //이건 정책에 따라
	//m_bInteractable = true;

	OnInActive(); // 자신의 행동 호출 가상함수

	for (auto& it : m_Children)
	{
		it->UI_InActive();
	}
}

void CUI::Set_UI_Disabled(bool isChangeEvent)
{
	m_UIState = UI_STATE::DISABLE;

	m_bEnabled = true;
	m_bVisible = true; //이건 정책에 따라
	//m_bInteractable = false;

	if (isChangeEvent == true)
	{
		OnDisabled();
	}

	for (auto& it : m_Children)
	{
		it->Set_UI_Disabled(isChangeEvent);
	}
}

void CUI::UI_Clear() // 이건 삭제
{


	for (auto& it : m_Children)
	{
		it->UI_Clear();
		it->Mark_Destroy();
	}


	//m_Children.clear();


	// 트랜스폼 끊기
	if (m_pUITransformCom)
	{
		m_Parent.reset();
		//
		// m_pUITransformCom->SetParent(shared_ptr<CUITransform>(nullptr), false); //이거 왜한거임??
	}// 아 혹시 free 용이 아닌가?? 일단 패스

	m_behavior.clear();

	OnClear();

}

HRESULT CUI::Add_Child(shared_ptr<CUI> child, _wstring UITag, _bool KeepWorldRect)
{
	if (child == nullptr)
		return E_FAIL;
	// 백터에 넣기 (본체)
	m_Children.push_back(child);
	child->m_Parent = static_pointer_cast<CUI>(shared_from_this());

	// 맵에 넣기 (검색용)
	auto ui = Find_Children(UITag).lock();
	if (ui != nullptr)
		return E_FAIL;
	m_mapChildren.emplace(UITag, child);

	// 트렌스폼 연결 (안에서 부모 자식 연결 다함)
	if (m_pUITransformCom && child->GetUITransform())
	{
		child->GetUITransform()->SetParent(m_pUITransformCom, KeepWorldRect);
	}

	return S_OK;
}

void CUI::Set_Zorder(_uint Z)
{
	m_ZOrder = Z;
	if (m_Parent.lock() != nullptr)
	{
		m_Parent.lock()->m_bIsDirty_Zorder = true;
	}
	for (auto& it : m_Children)
	{
		it->Set_Zorder(Z);
	}
}




void CUI::Free()
{
	for (auto& it : m_Children)
		it->UI_Clear();

	m_Children.clear();

	// 트랜스폼 끊기
	if (m_pUITransformCom)
	{
		m_Parent.reset();
		//
		// m_pUITransformCom->SetParent(shared_ptr<CUITransform>(nullptr), false); //이거 왜한거임??
	}// 아 혹시 free 용이 아닌가?? 일단 패스

	m_behavior.clear();

	OnClear();

	__super::Free();


}
