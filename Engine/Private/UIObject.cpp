#include "UIObject.h"
#include "Shader.h"

CUIObject::CUIObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject { pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 직교투영 */
	D3D11_VIEWPORT			ViewPortDesc{};
	_uint					iNumViewports = { 1 };
	m_pContext->RSGetViewports(&iNumViewports, &ViewPortDesc);

	m_fViewportWidth = ViewPortDesc.Width;
	m_fViewportHeight = ViewPortDesc.Height;

	/* 트랜스폼이 들고 있는 월드행렬에 위의 네개상태를 표현할 수 있도록 볂형한다. */	
	Update_Transform();

	/* 항등 뷰. */	
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::VIEW)], XMMatrixIdentity());

	
	XMStoreFloat4x4(&m_TransformationMatrices[ETOI(D3DTS::PROJ)], XMMatrixOrthographicLH(m_fViewportWidth, m_fViewportHeight, 0.f, 1.f));

	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{

}

HRESULT CUIObject::Render()
{
	return S_OK;
}


void CUIObject::Update_Transform()
{
	m_pTransformCom->SetUp_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(STATE::POSITION,
		XMVectorSet(m_fX - m_fViewportWidth * 0.5f, -m_fY + m_fViewportHeight * 0.5f, 0.f, 1.f));
}

HRESULT CUIObject::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, D3DTS eTransformState)
{
	return pShader->Bind_Matrix(pConstantName, &m_TransformationMatrices[ETOI(eTransformState)]);	
}

void CUIObject::Free()
{
	__super::Free();

}
