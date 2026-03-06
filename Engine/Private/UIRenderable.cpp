#include "UIRenderable.h"

CUIRenderable::CUIRenderable(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUI(pDevice, pContext)
{

}

CUIRenderable::CUIRenderable(const CUIRenderable& prototype)
	: CUI(prototype),
	m_PassIndex(0)
{
}


HRESULT CUIRenderable::OnInit(void* pArg)
{

	RENDERABLE_DESC* pDesc = static_cast<RENDERABLE_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc->TextureComLevel, pDesc->TextureProtoName)))
		return E_FAIL;

	_float2 orignSize = m_pTextureCom->Get_SizeFromSRV(0);
	m_SliceDesc.TexOriginalSize = orignSize;
	m_SliceDesc.UISize = _float2(1.f, 1.f);// 어짜피 트렌스폼이 정함 최종 ui 사이즈
	m_SliceDesc.PxSliceLRTB = _float4(orignSize.x / 3.f, orignSize.x / 3.f, orignSize.y / 3.f, orignSize.y / 3.f);

	m_bUseNineSlice = pDesc->bUseNineSlice;

	if (pDesc->PxSliceLRTB.x != 0.f && pDesc->PxSliceLRTB.y != 0.f 
	    && pDesc->PxSliceLRTB.z != 0.f && pDesc->PxSliceLRTB.w != 0.f)
	{
	    m_SliceDesc.PxSliceLRTB = (pDesc->PxSliceLRTB);
	}


	return S_OK;
}

void CUIRenderable::OnActive()
{
	
}

void CUIRenderable::OnInActive()
{
	
}

void CUIRenderable::OnDisabled()
{
	
}

void CUIRenderable::OnUpdate(const _float& timeDelta)
{
	if (m_bIsDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_bIsDirtyCom = false;
	}

	if(m_bUseNineSlice)
		m_SliceDesc.UISize = m_pUITransformCom->Get_FinalSize();
	if (m_bUseNineSlice)
		m_SliceDesc.TexCustomSize = m_pUITransformCom->Get_SizeDelta();
}

void CUIRenderable::OnLateUpdate()
{
	if (m_bUseNineSlice == true)
	{
		m_PassIndex = 1;

	}
	else
	{
		m_PassIndex = 0;
	}
}

HRESULT CUIRenderable::OnRender()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(m_PassIndex)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIRenderable::Ready_Components(_uint Level, _wstring protoName)
{
	if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_pVIBufferCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
		return E_FAIL;
	if (m_pTextureCom && m_pUITransformCom) {

		m_pUITransformCom->SetSizeDelta(m_pTextureCom->Get_SizeFromSRV(0));
	}
	return S_OK;
}

HRESULT CUIRenderable::Bind_ShaderResources()
{
	if (FAILED(m_pUITransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (m_PassIndex == 1)
	 {
		 if (FAILED(m_pShaderCom->Bind_RawValue("g_TexOriginalSize", &m_SliceDesc.TexOriginalSize, sizeof(_float2))))
			 return E_FAIL;
		 if (FAILED(m_pShaderCom->Bind_RawValue("g_TexCustomSize", &m_SliceDesc.TexCustomSize, sizeof(_float2))))
			 return E_FAIL;
		 if (FAILED(m_pShaderCom->Bind_RawValue("g_UISize", &m_SliceDesc.UISize, sizeof(_float2))))
			 return E_FAIL;
		 if (FAILED(m_pShaderCom->Bind_RawValue("g_PxSliceLRTB", &m_SliceDesc.PxSliceLRTB, sizeof(_float4))))
			 return E_FAIL;

	 }



	return S_OK;
}

void CUIRenderable::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	m_pShaderCom = Get_Component<CShader>(L"Com_Shader");
}
void CUIRenderable::OnClear()
{
	
}

void CUIRenderable::Save_ToJson(nlohmann::json& j)
{
	j["PxSliceLRTB"] = { m_SliceDesc.PxSliceLRTB.x,  m_SliceDesc.PxSliceLRTB.y, m_SliceDesc.PxSliceLRTB.z, m_SliceDesc.PxSliceLRTB.w };
	j["UseNineSlice"] = m_bUseNineSlice;

	__super::Save_ToJson(j);
}

void CUIRenderable::Load_FromJson(nlohmann::json& j)
{
	if (j.contains("PxSliceLRTB"))
	{
		m_SliceDesc.PxSliceLRTB.x = j["PxSliceLRTB"][0];
		m_SliceDesc.PxSliceLRTB.y = j["PxSliceLRTB"][1];
		m_SliceDesc.PxSliceLRTB.z = j["PxSliceLRTB"][2];
		m_SliceDesc.PxSliceLRTB.w = j["PxSliceLRTB"][3];
	}

	if (j.contains("UseNineSlice"))
	{
		m_bUseNineSlice = j["UseNineSlice"];
	}

	__super::Load_FromJson(j);
}

void CUIRenderable::OnGui()
{
	ImGui::Checkbox("Use NineSlice", &m_bUseNineSlice);


	_float4& slice = m_SliceDesc.PxSliceLRTB;

	ImGui::Text("Nine Slice (px)");

	ImGui::DragFloat("Left", &slice.x, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Right", &slice.y, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Top", &slice.z, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Bottom", &slice.w, 1.0f, 0.0f, 1000.0f);

	_float& TileScale = m_SliceDesc.TileScale;
	ImGui::DragFloat("TileScale", &TileScale, 1.0f, 0.0f, 1000.0f);
}

shared_ptr<CUIRenderable> CUIRenderable::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CUIRenderable> pInstance(new CUIRenderable(pDevice, pContext), [](CUIRenderable* p) {p->Free(); delete(p); });

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUIImage");
	}
	return pInstance;
}

void CUIRenderable::Free()
{
	__super::Free();
}
