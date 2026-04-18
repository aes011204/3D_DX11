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

	m_bIsSavableClass = true;

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
	if (m_bIsDirtyCom) {
		RebindCom();      // "바뀐 것"만 한 번 갱신
		m_bIsDirtyCom = false;
	}
	int a = 1;

	//m_pModelCom->Play_Animation(fTimeDelta);
}

void CEmptyGObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CEmptyGObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t iNumMesh = m_pModelCom->Get_NumMeshes();

	for(size_t i=0; i< iNumMesh; i++)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType_DIFFUSE, 0);
		//m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pModelCom->Render(i)))
		return E_FAIL;
	}




	return S_OK;
}

void CEmptyGObject::OnGui()
{
	ImGui::Checkbox("Custom Shader", &m_bCustomS);

	if (m_bCustomS == false)
		return;
	ImGui::DragFloat("Lamp 1 Intensity", &m_ColorLamp1, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::SliderFloat("Lamp 2 Intensity", &m_ColorLamp2, 0.0f, 1.0f, "%.3f");

	ImGui::Separator();



	ImGui::Text("Terrain Colors (RGBA)");

	ImGui::ColorEdit4("Sand Color", (float*)&m_SandColor);

	ImGui::ColorEdit4("Rock Color", (float*)&m_RockColor);

	ImGui::ColorEdit4("Grass Color", (float*)&m_GrassColor);

}

void CEmptyGObject::Save_ToJson(nlohmann::json& j)
{

	CGameObject::Save_ToJson(j);

	if (m_bCustomS == false)
		return;
	j["bool_CustomShader"] = m_bCustomS;
	j["ColorLamp1"] = m_ColorLamp1;
	j["ColorLamp2"] = m_ColorLamp2;

	j["SandColor"] = { m_SandColor.x, m_SandColor.y, m_SandColor.z, m_SandColor.w };
	j["RockColor"] = { m_RockColor.x, m_RockColor.y, m_RockColor.z, m_RockColor.w };
	j["GrassColor"] = { m_GrassColor.x, m_GrassColor.y, m_GrassColor.z, m_GrassColor.w };

}

void CEmptyGObject::Load_FromJson(nlohmann::json& j)
{

	CGameObject::Load_FromJson(j);

	if (m_bCustomS == false)
		return;
	m_bCustomS = j["bool_CustomShader"];

	m_ColorLamp1 = j["ColorLamp1"];
	m_ColorLamp2 = j["ColorLamp2"];

	auto sand = j["SandColor"];
	m_SandColor = { sand[0], sand[1], sand[2], sand[3] };

	auto rock = j["RockColor"];
	m_RockColor = { rock[0], rock[1], rock[2], rock[3] };

	auto grass = j["GrassColor"];
	m_GrassColor = { grass[0], grass[1], grass[2], grass[3] };


}
void CEmptyGObject::RebindCom()
{
	// 이제 모든 컴포넌트는 널체크 잘하기 없는경우도 있을수 있으니까
	m_pTextureCom = Get_Component<CTexture>(L"Com_Texture");
	m_pModelCom = Get_Component<CModel>(L"Com_Model");
	//m_pVIBufferCom = Get_Component<CVIBuffer>(L"Com_VIBuffer");
	m_pShaderCom = Get_Component<CShader>(L"Com_Shader");


}

HRESULT CEmptyGObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::VIEW, m_pShaderCom, "g_ViewMatrix")))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Bind_TransformMatrix(D3DTS::PROJ, m_pShaderCom, "g_ProjMatrix")))
		return E_FAIL;



	if (FAILED(m_pGameInstance.lock()->Bind_CamPosition(m_pShaderCom, "g_vCamPosition")))
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



	if (m_bCustomS == true)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorLamp1", &m_ColorLamp1, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_ColorLamp2", &m_ColorLamp2, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_SandColor", &m_SandColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RockColor", &m_RockColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_GrassColor", &m_GrassColor, sizeof(_float4))))
			return E_FAIL;
	}
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
	if (FAILED(Add_Component(ETOI(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), &m_pShaderCom, nullptr)))
		return E_FAIL;
	// 이거는 필수로 있어야 하지만 클래스를 갈아 끼울수 있어야 함 
	if (FAILED(Add_Component(ETOI(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_ForkLift"), TEXT("Com_Model"), &m_pModelCom, nullptr)))
		return E_FAIL;
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
