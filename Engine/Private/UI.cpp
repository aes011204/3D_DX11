#include "UI.h"

#include "DInput_Manager.h"
#include "GameInstance.h"
#include "UITransform.h" 
#include "IModifier.h"
#include "Engine_Helper.h"

CUI::CUI(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CEntity(device, context)
{
}

CUI::CUI(const CUI& prototype)
	: CEntity(prototype),
	m_zOrder(prototype.m_zOrder),
	m_isLayoutTarget(prototype.m_isLayoutTarget)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;

}

// UI는 프로토 타입으로 아니셜라이 프로토타입으로 호출하고 직접Initalize 하니까 
HRESULT CUI::Initialize(void* arg)
{

	UIDesc* desc = static_cast<UIDesc*>(arg);

	Set_Zorder(desc->ZOrder);
	m_noDetach = desc->isNoDetach;

	if (m_isInitialized == true)
		return S_OK;

	SetDefaultNameFromThisType();

	{
		m_UITransformCom = CUITransform::Create(m_device, m_context);
		if (nullptr == m_UITransformCom)
			return E_FAIL;

		if (FAILED(m_UITransformCom->Initialize(arg)))
			return E_FAIL;

		m_UITransformCom->SetDefaultNameFromThisType();

		if (nullptr == Get_Component(g_strUITransformTag))
		{
			m_components.emplace(g_strUITransformTag, m_UITransformCom);

		}
	}


	OnInit(arg);


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
	D3D11_VIEWPORT viewPortDesc{};
	_uint numViewPort = { 1 };
	m_context->RSGetViewports(&numViewPort, &viewPortDesc);

	float viewportWidth = viewPortDesc.Width;
	float viewportHeight = viewPortDesc.Height;

	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::VIEW)], XMMatrixIdentity());
	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::PROJ)], XMMatrixOrthographicOffCenterLH(0.f, viewportWidth,
		0.f, viewportHeight,
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

	m_isInitialized = true;

	return S_OK;
}

HRESULT CUI::Bind_ShaderResource(shared_ptr<CShader> shader, const _char* constantName, ED3DTS transformState)
{
	return shader->Bind_Matrix(constantName, &m_transformationMatrices[ETOI(transformState)]);
}

weak_ptr<CUI> CUI::Find_Children(_wstring tag)
{
	auto it = m_mapChildren.find(tag);
	if (it == m_mapChildren.end())
		return weak_ptr<CUI>(); // 빈 weak_ptr -> 널체크는 lock() 하면 주인 shared ptr 없으면 널 됨

	return it->second;

}

void CUI::Save_ToJson(nlohmann::json& j)
{
	float m_x = {};
	float m_y = {};
	if (!std::isnan(m_x) && !std::isinf(m_x)) j["X"] = m_x;
	if (!std::isnan(m_y) && !std::isinf(m_y)) j["Y"] = m_y;
	//CUI 데이터
	if (m_zOrder != 1)
		j["ZOrder"] = m_zOrder;

	j["LayoutTarget"] = m_isLayoutTarget;

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
		auto child = pair.second.lock();
		if (nullptr == child) continue;

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
		m_zOrder = j["ZOrder"];
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
				auto child = it->second.lock();
				if (child)
				{
					// 자식도 똑같이 이 함수를 타게 함
					child->Load_FromJson(jChild);
				}
			}
		}
	}
}

void CUI::OnGui()
{
	ImGui::InputInt("ZOrder Control", &m_zOrder);
	ImGui::Text("bLayoutTarget : %s", m_isLayoutTarget ? "true" : "false");
	ImGui::Text("UIState : %s", magic_enum::enum_name(m_UIState).data());

}

void CUI::Update(_float timeDelta, bool& isMouseHold)
{

	

	if (m_isEnabled)
	{
		if (m_UITransformCom)
			m_UITransformCom->UpdateLayoutIfDirty();

		for (auto& it : m_children)
		{
			it->Update(timeDelta, isMouseHold);
		}


		if (isMouseHold == false && m_isInteractable == true)
		{
			Vector2 mousePos = m_gameInstance.lock()->Get_DInput_Manger()->Get_MousePos();
			if (true == m_UITransformCom->Get_WorldRect().Contains(mousePos.x, mousePos.y))
			{
				m_isHovered = true;
				isMouseHold = true;
			}
			else
				m_isHovered = false;
		}
		else
			m_isHovered = false;


		OnUpdate(timeDelta);


	
		if (!m_isRenderReady)
			m_isRenderReady = true;


		//for (auto& it : m_Children)
		//{
		//	it->Late_Update(fTimeDelta);
		//}
	}

}

void CUI::Late_Update(_float timeDelta)
{
	if (m_isEnabled)
	{
		OnLateUpdate();

		//for (auto& it : m_Children)
		//{
		//	it->Late_Update(fTimeDelta);
		//}

		{
			for (auto it = m_children.begin(); it != m_children.end(); )
			{
				if (nullptr != *it)
					(*it)->Late_Update(timeDelta);

				if ((*it)->Is_PendingDestroy() == true)
				{
				
					it = m_children.erase(it);
				}
				else
					it++;

			}
		}

		for (auto it = m_behavior.begin(); it != m_behavior.end(); )
		{
			(*it)->Tick(timeDelta, this);

			
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

void CUI::Prepare_Render()
{
    if (!m_isEnabled)
        return;

    if (m_UITransformCom)
        m_UITransformCom->UpdateLayoutIfDirty();
    OnPrepareRender();
    if (!m_isEnabled)
        return;
    if (m_UITransformCom)
        m_UITransformCom->UpdateLayoutIfDirty();

    for (auto& child : m_children)
    {
        if (child)
            child->Prepare_Render();
    }
    m_isRenderReady = true;
}

HRESULT CUI::Render()
{
	if (m_isVisible && m_isRenderReady == true)
	{
		if (m_isDirtyZOrder)
		{
			if (m_children.size() >= 2)
			{
				stable_sort(m_children.begin(), m_children.end(), [](const shared_ptr<CUI>& a, const shared_ptr<CUI>& b) {
					//if(a->m_ZOrder==b->m_ZOrder)
					//	return 
					return a->m_zOrder < b->m_zOrder;
					});
				m_isDirtyZOrder = false;
			}
		}
		OnRender();

		for (auto& it : m_children)
		{
			it->Render();
		}
	}
	//m_bRenderReady = true;
	return S_OK;
}

void CUI::UI_Active()
{
	m_UIState = EUIState::ACTIVE;

	m_isEnabled = true;
	m_isVisible = true;

	m_isRenderReady = false;

	if (m_UITransformCom)
		m_UITransformCom->UpdateLayoutIfDirty();

	//m_bInteractable = true;
	OnActive();

	for (auto& it : m_children)
	{
		it->UI_Active();
	}
}

void CUI::UI_InActive()
{
	m_UIState = EUIState::INACTIVE;

	m_isEnabled = false;
	m_isVisible = false; //이건 정책에 따라
	//m_bInteractable = true;

	OnInActive(); // 자신의 행동 호출 가상함수

	for (auto& it : m_children)
	{
		it->UI_InActive();
	}
}

void CUI::Set_UI_Disabled(bool isChangeEvent)
{
	m_UIState = EUIState::DISABLE;

	m_isEnabled = true;
	m_isVisible = true; //이건 정책에 따라
	//m_bInteractable = false;

	if (isChangeEvent == true)
	{
		OnDisabled();
	}

	for (auto& it : m_children)
	{
		it->Set_UI_Disabled(isChangeEvent);
	}
}

void CUI::UI_Clear() // 이건 삭제
{


	for (auto& it : m_children)
	{
		it->UI_Clear();
		it->Mark_Destroy();
	}


	//m_Children.clear();


	// 트랜스폼 끊기
	if (m_UITransformCom)
	{
		m_parent.reset();
		//
		// m_pUITransformCom->SetParent(shared_ptr<CUITransform>(nullptr), false); //이거 왜한거임??
	}// 아 혹시 free 용이 아닌가?? 일단 패스

	m_behavior.clear();

	OnClear();

}

HRESULT CUI::Add_Child(shared_ptr<CUI> child, _wstring UITag, _bool keepWorldRect)
{
	if (child == nullptr)
		return E_FAIL;
	// 백터에 넣기 (본체)
	m_children.push_back(child);
	child->m_parent = static_pointer_cast<CUI>(shared_from_this());

	// 맵에 넣기 (검색용)
	auto ui = Find_Children(UITag).lock();
	if (ui != nullptr)
		return E_FAIL;
	m_mapChildren.emplace(UITag, child);

	// 트렌스폼 연결 (안에서 부모 자식 연결 다함)
	if (m_UITransformCom && child->GetUITransform())
	{
		child->GetUITransform()->SetParent(m_UITransformCom, keepWorldRect);
	}

	return S_OK;
}

void CUI::Set_Zorder(_uint Z)
{
	m_zOrder = Z;
	if (m_parent.lock() != nullptr)
	{
		m_parent.lock()->m_isDirtyZOrder = true;
	}
	for (auto& it : m_children)
	{
		it->Set_Zorder(Z);
	}
}




void CUI::Free()
{
	for (auto& it : m_children)
		it->UI_Clear();

	m_children.clear();

	// 트랜스폼 끊기
	if (m_UITransformCom)
	{
		m_parent.reset();
		//
		// m_pUITransformCom->SetParent(shared_ptr<CUITransform>(nullptr), false); //이거 왜한거임??
	}// 아 혹시 free 용이 아닌가?? 일단 패스

	m_behavior.clear();

	OnClear();

	__super::Free();


}
