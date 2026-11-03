#include "UIObject.h"
#include "Shader.h"

CUIObject::CUIObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject { device, context }
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

HRESULT CUIObject::Initialize(void* arg)
{
	UIDesc* desc = static_cast<UIDesc*>(arg);

	m_x = desc->x;
	m_y = desc->y;
	m_sizeX = desc->sizeX;
	m_sizeY = desc->sizeY;

	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	/* 직교투영 */
	D3D11_VIEWPORT			viewPortDesc{};
	_uint					numViewports = { 1 };
	m_context->RSGetViewports(&numViewports, &viewPortDesc);

	m_viewportWidth = viewPortDesc.Width;
	m_viewportHeight = viewPortDesc.Height;

	/* 트랜스폼이 들고 있는 월드행렬에 위의 네개상태를 표현할 수 있도록 볂형한다. */	
	Update_Transform();

	/* 항등 뷰. */	
	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::VIEW)], XMMatrixIdentity());

	
	XMStoreFloat4x4(&m_transformationMatrices[ETOI(ED3DTS::PROJ)], XMMatrixOrthographicLH(m_viewportWidth, m_viewportHeight, 0.f, 1.f));

	return S_OK;
}

void CUIObject::Priority_Update(_float timeDelta)
{
}

void CUIObject::Update(_float timeDelta)
{
}

void CUIObject::Late_Update(_float timeDelta)
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

HRESULT CUIObject::Bind_ShaderResource(shared_ptr<class CShader> shader, const _char* constantName, ED3DTS transformState)
{
	return shader->Bind_Matrix(constantName, &m_transformationMatrices[ETOI(transformState)]);	
}

void CUIObject::Free()
{
	__super::Free();

}
