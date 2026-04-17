#include "Renderer.h"
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

	/* For.RenderTargets */
	if (FAILED(m_pGameInstance.lock()->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;


	/* For.MRTs */
	if (FAILED(m_pGameInstance.lock()->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
	//	return E_FAIL;






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
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	m_pContext->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	m_pContext->OMSetDepthStencilState(nullptr, 0);
	m_pContext->RSSetState(nullptr);

	Render_Priority();

	Render_NonBlend();

	Render_Sea();
	Render_Blend();

	Render_UI();
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
