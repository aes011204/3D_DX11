#include "UIRenderable.h"

CUIRenderable::CUIRenderable(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CUI(pDevice, pContext)
{

}

CUIRenderable::CUIRenderable(const CUIRenderable& prototype)
	: CUI(prototype),
	m_passIndex(0)
{
}


HRESULT CUIRenderable::OnInit(void* pArg)
{

	RenderableDesc* pDesc = static_cast<RenderableDesc*>(pArg);

	m_isTransparent = pDesc->isTransparent;
	m_useDark = pDesc->useDark;
	m_useNineSlice = pDesc->useNineSlice;

	m_texProtoLevel = pDesc->textureComLevel;
	 m_texProtoName = pDesc->textureProtoName;
	m_texIndex = pDesc->textureIndex;

	m_shaderType = pDesc->shaderType;
	if(m_shaderType == UIShaderType::Radial)
	{
		m_useRadial = true;
		
	}


	if (FAILED(Ready_Components(m_texProtoLevel, m_texProtoName)))
		return E_FAIL;

	if (m_isTransparent == false)
	{
		_float2 orignSize = m_textureCom->Get_SizeFromSRV(0);
		m_sliceDesc.texOriginalSize = orignSize;
		m_sliceDesc.UISize = _float2(1.f, 1.f);// 어짜피 트렌스폼이 정함 최종 ui 사이즈
		m_sliceDesc.pxSliceLRTB = _float4(orignSize.x / 3.f, orignSize.x / 3.f, orignSize.y / 3.f, orignSize.y / 3.f);
	}




	if (pDesc->pxSliceLRTB.x != 0.f && pDesc->pxSliceLRTB.y != 0.f
		&& pDesc->pxSliceLRTB.z != 0.f && pDesc->pxSliceLRTB.w != 0.f)
	{
		m_sliceDesc.pxSliceLRTB = (pDesc->pxSliceLRTB);
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
	if (m_isDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_isDirtyCom = false;
	}

	if (m_useNineSlice)
		m_sliceDesc.UISize = m_UITransformCom->Get_FinalSize();
	if (m_useNineSlice)
		m_sliceDesc.texCustomSize = m_UITransformCom->Get_SizeDelta();
}

void CUIRenderable::OnPrepareRender()
{
    CUIRenderable::OnUpdate(0.f);
    CUIRenderable::OnLateUpdate();
}

void CUIRenderable::OnLateUpdate()
{
	if (m_useNineSlice == true)
	{
		m_passIndex = 1;

	}
	else
	{
		m_passIndex = 0;
	}
}

HRESULT CUIRenderable::OnRender()
{
	if (m_isTransparent == true)
		return S_OK;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_shaderCom->Begin(m_passIndex)))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_VIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIRenderable::Change_Texture(shared_ptr<CTexture> texture, _uint TexIndex)
{
	//if(protoName != m_TexProtoName)
	//{

	m_textureCom = texture;
	m_texIndex = TexIndex;
	if (m_textureCom && m_UITransformCom) {

		m_UITransformCom->SetSizeDelta((m_textureCom->Get_SizeFromSRV(TexIndex) / 3.f) * 2.f);
	}
	return S_OK;
	//}

}

HRESULT CUIRenderable::Set_TextureIndex(_uint index)
{
	if (index >= m_textureCom->Get_NumSRVs())
		return E_FAIL;


	m_texIndex = index;
	return S_OK;
}

HRESULT CUIRenderable::Ready_Components(_uint Level, _wstring protoName)
{
	if (FAILED(Add_Component(0, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), &m_VIBufferCom, nullptr)))
		return E_FAIL;

	if (m_shaderType == UIShaderType::Radial)
	{
		if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxDonut"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
			return E_FAIL;
	}
	else if (m_shaderType == UIShaderType::Default)
	{

		if (FAILED(Add_Component(0, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
			return E_FAIL;

	}




	if (m_isTransparent == false)
	{
		if (FAILED(Add_Component(Level, /*TEXT(protoName)*/protoName, TEXT("Com_Texture"), &m_textureCom, nullptr)))
			return E_FAIL;
		if (m_textureCom && m_UITransformCom) {

			m_UITransformCom->SetSizeDelta((m_textureCom->Get_SizeFromSRV(m_texIndex) / 3.f) * 2.f);
		}
	}
	return S_OK;
}

HRESULT CUIRenderable::Bind_ShaderResources()
{
	if (FAILED(m_UITransformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_shaderCom, "g_ViewMatrix", ED3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(__super::Bind_ShaderResource(m_shaderCom, "g_ProjMatrix", ED3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_textureCom->Bind_ShaderResourceView(m_shaderCom, "g_Texture", m_texIndex)))
		return E_FAIL;


	switch (m_shaderType)
	{
	case UIShaderType::Default:
		if (FAILED(Bind_ShaderResources_Default()))
			return E_FAIL;
		break;
	case UIShaderType::Radial:
		if (FAILED(Bind_ShaderResources_Radius()))
			return E_FAIL;

		break;
	}



	return S_OK;
}


HRESULT CUIRenderable::Bind_ShaderResources_Default()
{
	if (m_useDark == true)
	{
		if (FAILED(m_shaderCom->Bind_RawValue("g_Dark", &m_dark, sizeof(_float))))
			return E_FAIL;
	}

	if (m_useColorFlat == true)
	{
		if (FAILED(m_shaderCom->Bind_RawValue("g_ColorFlat", &m_colorFlat, sizeof(_float4))))
			return E_FAIL;
	}

	if (m_useColorMix == true)
	{
		if (FAILED(m_shaderCom->Bind_RawValue("g_ColorMix", &m_color, sizeof(_float4))))
			return E_FAIL;
	}

	if (FAILED(m_shaderCom->Bind_RawValue("g_Alpha", &m_alpha, sizeof(_float))))
		return E_FAIL;


	if (m_passIndex == 1)
	{
		if (FAILED(m_shaderCom->Bind_RawValue("g_TexOriginalSize", &m_sliceDesc.texOriginalSize, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_TexCustomSize", &m_sliceDesc.texCustomSize, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_UISize", &m_sliceDesc.UISize, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_PxSliceLRTB", &m_sliceDesc.pxSliceLRTB, sizeof(_float4))))
			return E_FAIL;

	}
	return S_OK;
}

HRESULT CUIRenderable::Bind_ShaderResources_Radius()
{

	if (m_useRadial == false)
		return S_OK;

	if (FAILED(m_shaderCom->Bind_RawValue("g_BaseColor", &m_baseColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_shaderCom->Bind_RawValue("g_ZoonColor", &m_zoneColor, sizeof(_float4))))
		return E_FAIL;



	return S_OK;

}


void CUIRenderable::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_VIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	m_shaderCom = Get_Component<CShader>(L"Com_Shader");
	//m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");



}
void CUIRenderable::OnClear()
{

}

void CUIRenderable::Save_ToJson(nlohmann::json& j)
{
	__super::Save_ToJson(j);
	j["PxSliceLRTB"] = { m_sliceDesc.pxSliceLRTB.x,  m_sliceDesc.pxSliceLRTB.y, m_sliceDesc.pxSliceLRTB.z, m_sliceDesc.pxSliceLRTB.w };
	j["UseNineSlice"] = m_useNineSlice;

}

void CUIRenderable::Load_FromJson(nlohmann::json& j)
{
	__super::Load_FromJson(j);
	if (j.contains("PxSliceLRTB"))
	{
		m_sliceDesc.pxSliceLRTB.x = j["PxSliceLRTB"][0];
		m_sliceDesc.pxSliceLRTB.y = j["PxSliceLRTB"][1];
		m_sliceDesc.pxSliceLRTB.z = j["PxSliceLRTB"][2];
		m_sliceDesc.pxSliceLRTB.w = j["PxSliceLRTB"][3];
	}

	if (j.contains("UseNineSlice"))
	{
		m_useNineSlice = j["UseNineSlice"];
	}

}

void CUIRenderable::OnGui()
{
	ImGui::Checkbox("Use NineSlice", &m_useNineSlice);


	_float4& slice = m_sliceDesc.pxSliceLRTB;

	ImGui::Text("Nine Slice (px)");

	ImGui::DragFloat("Left", &slice.x, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Right", &slice.y, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Top", &slice.z, 1.0f, 0.0f, 1000.0f);
	ImGui::DragFloat("Bottom", &slice.w, 1.0f, 0.0f, 1000.0f);

	_float& TileScale = m_sliceDesc.tileScale;
	ImGui::DragFloat("TileScale", &TileScale, 1.0f, 0.0f, 1000.0f);

	ImGui::Text("ZOrder: %d", m_zOrder);

	__super::OnGui();
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
