#include "UI.h"

#include "DInput_Manager.h"
#include "GameInstance.h"
#include "UITransform.h" 
#include "IModifier.h"

CUI::CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CEntity(pDevice, pContext)
{
}

CUI::CUI(const CUI& prototype)
	: CEntity(prototype)
{
}

HRESULT CUI::Initialize_Prototype()
{
	return S_OK;

}

HRESULT CUI::Initialize(void* pArg)
{

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



	//for (auto& it : m_Children)
	//{
	//	it->Initialize(pArg); // 이거 ㄱㅊ?? 이미 자식을 밖에서 만들어서 이니셜라이즈 다하고 여기 붙히는거 아닌가?
	//}// 마즘 지금 2번돔 근데 이니셜라이즈 안에서 방어 해놧음

	//// 행렬
	D3D11_VIEWPORT ViewPortDesc{};
	_uint iNumViewPort = { 1 };
	m_pContext->RSGetViewports(&iNumViewPort, &ViewPortDesc);

	float ViewportWidth = ViewPortDesc.Width;
	float ViewportHeight = ViewPortDesc.Height;

	//m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);
	//m_pTransformCom->Set_State(STATE::POSITION,
	//	XMVectorSet(m_fX - m_fViewportWidth * 0.5f, -m_fY + m_fViewportHeight * 0.5f, 0.f, 1.f));

	Rect r = m_pGameInstance.lock()->Get_WinSize();
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::VIEW)], XMMatrixIdentity());
	//XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], XMMatrixOrthographicLH(ViewportWidth, ViewportHeight, -100.f, 100.f));
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], XMMatrixOrthographicOffCenterLH(0.f, ViewportWidth,
		0.f, ViewportHeight,
		0.f, 1.f));
	//

	m_bInitialized = true;

	return S_OK;
}

HRESULT CUI::Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ETOI(eTransformState)]);
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
			if (true == m_pUITransformCom->GetWorldRect().Contains(mousePos.x, mousePos.y))
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

		for (auto& it : m_behavior)
		{
			it->Tick(fTimeDelta, this);
		}
	}

}

void CUI::Late_Update(_float fTimeDelta)
{
	if (m_bEnabled)
	{
		OnLateUpdate();

		for (auto& it : m_Children)
		{
			it->Late_Update(fTimeDelta);
		}

	}
}

HRESULT CUI::Render()
{
	if (m_bVisible)
	{
		if (m_bIsDirty_Zorder)
		{
			if (m_Children.size() >=2)
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
	return S_OK;
}

void CUI::UI_Active()
{
	m_bEnabled = true;
	m_bVisible = true;
	//m_bInteractable = true;
	OnActive();

	for (auto& it : m_Children)
	{
		it->UI_Active();
	}
}

void CUI::UI_InActive()
{
	m_bEnabled = false;
	m_bVisible = false; //이건 정책에 따라
	//m_bInteractable = false;
	OnInActive(); // 자신의 행동 호출 가상함수

	for (auto& it : m_Children)
	{
		it->UI_InActive();
	}
}

void CUI::Set_UI_Disabled(bool isChangeEvent)
{
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

}

HRESULT CUI::Add_Child(shared_ptr<CUI> child, _bool KeepWorldRect)
{
	if (child == nullptr)
		return E_FAIL;

	m_Children.push_back(child);
	child->m_Parent = static_pointer_cast<CUI>(shared_from_this());

	// 트렌스폼 연결 (안에서 부모 자식 연결 다함)
	if (m_pUITransformCom && child->GetUITransform())
	{
		child->GetUITransform()->SetParent(m_pUITransformCom, KeepWorldRect);
	}

	return S_OK;
}

//shared_ptr<CUI> CUI::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
//{
//	//shared_ptr<CUI> pInstance = CUI::Create(pDevice, pContext);
//	//if (FAILED(pInstance->Initialize_Prototype()))
//	//{
//	//	MSG_BOX("failed prototype: CUIBase");
//	//	Safe_Release(pInstance);
//	//}
//	//return pInstance;
//	return nullptr;
//}
//
//shared_ptr<CUI> CUI::Clone(void* pArg)
//{
//	//shared_ptr<CUI> pInstance = new CUI(*this);
//	//if (FAILED(pInstance->Initialize(pArg)))
//	//{
//	//	MSG_BOX("failed prototype: CUIBase");
//	//	Safe_Release(pInstance);
//	//}
//	//return pInstance;
//	return nullptr;
//
//}

void CUI::Free()
{
	UI_Clear(); // 자식 먼저 처리 (자식들이 부모의 컴포넌트나 정보를 참조가능성)

	__super::Free();


}
