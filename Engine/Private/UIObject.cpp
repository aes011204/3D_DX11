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
	UIDesc* pDesc = static_cast<UIDesc*>(pArg);

	m_x = pDesc->x;
	m_y = pDesc->y;
	m_sizeX = pDesc->sizeX;
	m_sizeY = pDesc->sizeY;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 직교투영 */
	D3D11_VIEWPORT			ViewPortDesc{};
	_uint					iNumViewports = { 1 };
	m_context->RSGetViewports(&iNumViewports, &ViewPortDesc);

	m_viewportWidth = ViewPortDesc.Width;
	m_viewportHeight = ViewPortDesc.Height;

	/* 트랜스폼이 들고 있는 월드행렬에 위의 네개상태를 표현할 수 있도록 볂형한다. */	
	Update_Transform();

	/* 항등 뷰. */	
	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::VIEW)], XMMatrixIdentity());

	
	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::PROJ)], XMMatrixOrthographicLH(m_viewportWidth, m_viewportHeight, 0.f, 1.f));

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
	m_transformCom->SetUp_Scale(m_sizeX, m_sizeY, 1.f);
	m_transformCom->Set_Position(
		XMVectorSet(m_x - m_viewportWidth * 0.5f, -m_y + m_viewportHeight * 0.5f, 0.f, 1.f));
}

HRESULT CUIObject::Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, ED3DTS eTransformState)
{
	return pShader->Bind_Matrix(pConstantName, &m_transformationMatrices[ETOI(eTransformState)]);	
}

void CUIObject::Free()
{
	__super::Free();

}
