#include "EmptyGObject.h"
#include "GameInstance.h"
#include "Model.h"

CEmptyGObject::CEmptyGObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CGameObject{ pDevice ,pContext }
{
}

CEmptyGObject::CEmptyGObject(const CEmptyGObject& prototype)
	: CGameObject{prototype}
{
}

HRESULT CEmptyGObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEmptyGObject::Initialize(void* pArg)
{

	m_isSavableClass = true;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEmptyGObject::Priority_Update(_float fTimeDelta)
{
}

void CEmptyGObject::Update(_float fTimeDelta)
{
	if (m_isDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_isDirtyCom = false;
	}
	int a = 1;

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CEmptyGObject::Late_Update(_float fTimeDelta)
{
	m_gameInstance.lock()->Add_RenderGroup(ERenderGroup::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CEmptyGObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_modelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_modelCom->Bind_Material(m_shaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		m_modelCom->Bind_Material(m_shaderCom, "g_EmissiveTexture", i, TextureType_EMISSIVE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	if (FAILED(m_shaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_modelCom->Render(i)))
		return E_FAIL;
	}




	return S_OK;
}

void CEmptyGObject::OnGui()
{
	ImGui::Checkbox("Custom Shader", &m_isCustomS);

	if (m_isCustomS == false)
		return;
	ImGui::DragFloat("Lamp 1 Intensity", &m_colorLamp1, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::SliderFloat("Lamp 2 Intensity", &m_colorLamp2, 0.0f, 1.0f, "%.3f");

	ImGui::Separator();



	ImGui::Text("Terrain Colors (RGBA)");

	ImGui::ColorEdit4("Sand Color", (float*)&m_sandColor);

	ImGui::ColorEdit4("Rock Color", (float*)&m_rockColor);

	ImGui::ColorEdit4("Grass Color", (float*)&m_grassColor);

}

void CEmptyGObject::Save_ToJson(nlohmann::json& j)
{

	CGameObject::Save_ToJson(j);

	if (m_isCustomS == false)
		return;
	j["bool_CustomShader"] = m_isCustomS;
	j["ColorLamp1"] = m_colorLamp1;
	j["ColorLamp2"] = m_colorLamp2;

	j["SandColor"] = { m_sandColor.x, m_sandColor.y, m_sandColor.z, m_sandColor.w };
	j["RockColor"] = { m_rockColor.x, m_rockColor.y, m_rockColor.z, m_rockColor.w };
	j["GrassColor"] = { m_grassColor.x, m_grassColor.y, m_grassColor.z, m_grassColor.w };

}

void CEmptyGObject::Load_FromJson(nlohmann::json& j)
{

	CGameObject::Load_FromJson(j);

	if (m_isCustomS == false)
		return;
	m_isCustomS = j["bool_CustomShader"];

	m_colorLamp1 = j["ColorLamp1"];
	m_colorLamp2 = j["ColorLamp2"];

	auto sand = j["SandColor"];
	m_sandColor = { sand[0], sand[1], sand[2], sand[3] };

	auto rock = j["RockColor"];
	m_rockColor = { rock[0], rock[1], rock[2], rock[3] };

	auto grass = j["GrassColor"];
	m_grassColor = { grass[0], grass[1], grass[2], grass[3] };


}
void CEmptyGObject::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_textureCom = Get_Component<CTexture>(L"Com_Texture");
	m_modelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	m_shaderCom = Get_Component<CShader>(L"Com_Shader");


}

HRESULT CEmptyGObject::Bind_ShaderResources()
{
	if (FAILED(m_transformCom->Bind_ShaderResource(m_shaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::VIEW, m_shaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_gameInstance.lock()->Bind_TransformMatrix(ED3DTS::PROJ, m_shaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_gameInstance.lock()->Bind_CamPosition(m_shaderCom, "g_vCamPosition")))
		return E_FAIL;

	/*const LIGHT_DESC* pLightDesc = m_pGameInstance.lock()->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;*/

	float fFar = m_gameInstance.lock()->Get_Far();
	if (FAILED(m_shaderCom->Bind_RawValue("g_Far",&fFar, sizeof(_float))))
		return E_FAIL;


	if (m_isCustomS == true)
	{
		if (FAILED(m_shaderCom->Bind_RawValue("g_ColorLamp1", &m_colorLamp1, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_ColorLamp2", &m_colorLamp2, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_SandColor", &m_sandColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_RockColor", &m_rockColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_shaderCom->Bind_RawValue("g_GrassColor", &m_grassColor, sizeof(_float4))))
			return E_FAIL;
	}

	float emissive =1.f;
	if (FAILED(m_shaderCom->Bind_RawValue("g_EmissiveStrength", &emissive, sizeof(_float))))
		return E_FAIL;
		
	
	return S_OK;
}

HRESULT CEmptyGObject::Ready_Components()
{
	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Shader_VtxMesh"),
	//	TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//	return E_FAIL;

	//if (FAILED(__super::Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Fiona"),
	//	TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//	return E_FAIL;

	// 쉐이더는 클래스를 갈아끼는게 아니라 안에 리소스를 바꾸는 거임
	if (FAILED(Add_Component(ETOI(ELevel::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_shaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
	//	return E_FAIL;
	//if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Texture_BackGround_1"), TEXT("Com_Texture"), &m_pTextureCom, nullptr)))
	//	return E_FAIL;


	return S_OK;
}

shared_ptr<CEmptyGObject> CEmptyGObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	shared_ptr<CEmptyGObject> pInstance(new CEmptyGObject(pDevice, pContext), [](CEmptyGObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEmptyGObject");
	}
	return pInstance;
}


shared_ptr<CGameObject> CEmptyGObject::Clone(void* pArg)
{
	shared_ptr<CEmptyGObject> pInstance(new CEmptyGObject(*this), [](CEmptyGObject* p) {p->Free(); delete p;});

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEmptyGObject");
	}
	return pInstance;
}

void CEmptyGObject::Free()
{
	__super::Free();
}
