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
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));

	// 1. 깊이 테스트 기능을 끕니다.
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 기록 안 함
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;         // 항상 통과

	// 2. 스텐실(Stencil) 기능도 끕니다.
	dsDesc.StencilEnable = FALSE;

	// 3. 디바이스를 통해 상태 객체 생성
	m_pDevice->CreateDepthStencilState(&dsDesc, m_pDepthDisableState.GetAddressOf());

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
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::NONBLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::NONBLEND)].clear();
}

void CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::BLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();
	}

	m_RenderObject[ETOI(RENDERGROUP::BLEND)].clear();
}

void CRenderer::Render_UI()
{
	m_pContext->OMSetDepthStencilState(m_pDepthDisableState.Get(), 0);
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::UI)])
	{
		if(pRenderObject != nullptr)
			pRenderObject->Render();
	}
	m_pContext->OMSetDepthStencilState(nullptr, 0);
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
