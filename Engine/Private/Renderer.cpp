#include "Renderer.h"

#include "DInput_Manager.h"
#include "GameInstance.h"
#include "Entity.h"


CRenderer::CRenderer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext)
	: m_pDevice(pDevice), m_pContext(m_pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{

}

CRenderer::~CRenderer() { Free(); };


HRESULT CRenderer::Initialize()
{
	_uint			iNumViewports = { 1 };
	D3D11_VIEWPORT	ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_WinSize = { ViewportDesc.Width, ViewportDesc.Height };

	/* For.RenderTargets */
	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Emissive"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	{

		if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Blur1"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;

		if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Blur2"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
			return E_FAIL;
	}


	/* For.MRTs */
	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;

	{
		if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Blur1"), TEXT("Target_Blur1"))))
			return E_FAIL;
		if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_Blur2"), TEXT("Target_Blur2"))))
			return E_FAIL;
	}
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../bin/ShaderFiles/Shader_Deferred.hlsl"), VTXTEX::Elements, VTXTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


#ifdef _DEBUG
	if (FAILED(m_pGameInstance.lock()->Ready_RT_Debug(TEXT("Target_Diffuse"), 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Ready_RT_Debug(TEXT("Target_Normal"), 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Ready_RT_Debug(TEXT("Target_Shade"), 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance.lock()->Ready_RT_Debug(TEXT("Target_Emissive"), 450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
#endif

	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, shared_ptr<CEntity> pGameObject)
{
	if(eRenderGroup == RENDERGROUP::END|| pGameObject == nullptr)
	{
		MSG_BOX("Failed to Add : Renderer");
		return;
	}
	m_RenderObject[ETOI(eRenderGroup)].push_back(pGameObject);

}

void CRenderer::Draw()
{
	

	Render_Priority();

	Render_NonBlend();
	Render_Lights();

	//RenderBloom();
	Render_Combined();

	Render_NonLight();
	

	Render_Sea();
	Render_Blend();

	Render_UI();



#ifdef _DEBUG
	if (m_pGameInstance.lock()->Get_DInput_Manger()->KeyPress(DIK_SPACE) == true)
	{
		Render_Debug();
	}
#endif
}

void CRenderer::Render_Priority()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::PRIORITY)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{

	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_GameObjects"))))
		return;




	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::NONBLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::NONBLEND)].clear();


	m_pGameInstance.lock()->End_MRT();
}

void CRenderer::Render_Blend()
{

	auto& vec = m_RenderObject[ETOI(RENDERGROUP::BLEND)];

	_vector vCamPos = XMLoadFloat4(m_pGameInstance.lock()->Get_CamPositon());


	sort(vec.begin(), vec.end(),
		[&](const shared_ptr<CEntity>& a, const shared_ptr<CEntity>& b)
		{
			_vector posA = a->Get_WorldPos();
			_vector posB = b->Get_WorldPos();

			float distA = XMVectorGetX(XMVector3LengthSq(posA - vCamPos));
			float distB = XMVectorGetX(XMVector3LengthSq(posB - vCamPos));

			return distA > distB; 
		});

	
	for (auto& obj : vec)
	{
		if (obj)
			obj->Render();
	}

	vec.clear();
	/*for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::BLEND)])
	{
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::BLEND)].clear();*/
}

void CRenderer::Render_Lights()
{
	//shade
	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	m_pContext->VSSetShaderResources(0, 16, nullSRV);
	m_pContext->GSSetShaderResources(0, 16, nullSRV);

	if(FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_LightAcc"))))
		return;

	m_pContext->OMSetDepthStencilState(nullptr, 0);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInverse", m_pGameInstance.lock()->Get_InverseTransfrom(D3DTS::VIEW))))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInverse", m_pGameInstance.lock()->Get_InverseTransfrom(D3DTS::PROJ))))
		return;


	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_NormalTexture", TEXT("Target_Normal"))))
		return;

	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_DepthTexture", TEXT("Target_Depth"))))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance.lock()->Get_CamPositon(), sizeof(_float4))))
		return;
	auto fFar = m_pGameInstance.lock()->Get_Far();
	if (FAILED(m_pShader->Bind_RawValue("g_Far",& fFar, sizeof(_float))))
		return;

	m_pVIBuffer->Bind_Resources();

	if (FAILED(m_pGameInstance.lock()->Render_Lights(m_pShader, m_pVIBuffer)))
		return;

	m_pGameInstance.lock()->End_MRT();

}

void CRenderer::Render_Combined()
{


	//ID3D11ShaderResourceView* nullSRV[16] = {};
	//m_pContext->PSSetShaderResources(0, 16, nullSRV);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
		return;
	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
		return;
	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_EmissiveTexture", TEXT("Target_Emissive"))))
		return;
	//if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_BloomTexture", TEXT("Target_Blur2"))))
	//	return;

	m_pShader->Begin(ETOI(DEFERRED::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

}
void CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		
	}

	m_RenderObject[ETOI(RENDERGROUP::NONLIGHT)].clear();
}

void CRenderer::Render_Sea()
{

	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::SEA)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::SEA)].clear();
}

void CRenderer::Render_UI()
{
	
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::UI)])
	{
		if(pRenderObject != nullptr)
			pRenderObject->Render();
	}


	m_RenderObject[ETOI(RENDERGROUP::UI)].clear();
}

void CRenderer::RenderBloom()
{

	ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);
	m_pContext->VSSetShaderResources(0, 16, nullSRV);
	m_pContext->GSSetShaderResources(0, 16, nullSRV);

	if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_Blur1"),false)))
		return;


	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_InputTexture", TEXT("Target_Emissive"))))
		return;


	_float4x4 identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	//m_pShader->Bind_Matrix("g_WorldMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ViewMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ProjMatrix", &identity);
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))

	if (FAILED(m_pShader->Bind_RawValue("g_WinSize", &m_WinSize, sizeof(_float2))))
		return;

	m_pShader->Begin(4);

	m_pVIBuffer->Bind_Resources();


	m_pVIBuffer->Render();

	//if (FAILED(m_pGameInstance.lock()->Render_Lights(m_pShader, m_pVIBuffer)))
	//	return;
	m_pGameInstance.lock()->End_MRT();
	//////////////////////////////////////////////////////
	ID3D11ShaderResourceView* nullSRV2[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV2);
	m_pContext->VSSetShaderResources(0, 16, nullSRV);
	m_pContext->GSSetShaderResources(0, 16, nullSRV);
	if (FAILED(m_pGameInstance.lock()->Begin_MRT(TEXT("MRT_Blur2"),false)))
		return;
	
	if (FAILED(m_pGameInstance.lock()->Bind_RT_ShaderResource(m_pShader, "g_InputTexture", TEXT("Target_Blur1"))))
		return;

      identity;
	XMStoreFloat4x4(&identity, XMMatrixIdentity());

	//m_pShader->Bind_Matrix("g_WorldMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ViewMatrix", &identity);
	//m_pShader->Bind_Matrix("g_ProjMatrix", &identity);
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))


	if (FAILED(m_pShader->Bind_RawValue("g_WinSize", &m_WinSize, sizeof(_float2))))
		return;

	m_pShader->Begin(5);

	m_pVIBuffer->Bind_Resources();


	m_pVIBuffer->Render();

	m_pGameInstance.lock()->End_MRT();
}
#ifdef _DEBUG
void CRenderer::Render_Debug()
{

	for (auto& pDebugComponent : m_DebugComponents)
	{
		pDebugComponent->Render();
		
	}

	m_DebugComponents.clear();


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;

	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_GameObjects"))))
		return;

	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_LightAcc"))))
		return;
//	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_Blur2"))))
//		return;
//	if (FAILED(m_pGameInstance.lock()->Render_RT_Debug(m_pVIBuffer, m_pShader, TEXT("MRT_Blur1"))))
//		return;
}
#endif

#ifdef _DEBUG
void CRenderer::Add_DebugenderGroup(shared_ptr<CComponent> pDebugComponent)
{
	m_DebugComponents.push_back(pDebugComponent);

}
#endif

unique_ptr<CRenderer> CRenderer::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> m_pContext)
{
	unique_ptr<CRenderer> pInstance (new CRenderer(pDevice, m_pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
	}
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();


	for(int i =0; i < ETOI(RENDERGROUP::END); i++)
	{
		//for (auto& it : m_RenderObject[i])
		//	Safe_Release(it);
		m_RenderObject[i].clear();
	}

}
