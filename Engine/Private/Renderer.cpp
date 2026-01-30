#include "Renderer.h"
#include "GameInstance.h"
#include "GameObject.h"


CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* m_pContext)
	: m_pDevice(pDevice), m_pContext(m_pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	return S_OK;
}

void CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pGameObject)
{
	if(eRenderGroup == RENDERGROUP::END|| pGameObject == nullptr)
	{
		MSG_BOX("Failed to Add : Renderer");
		return;
	}
	m_RenderObject[ETOI(eRenderGroup)].push_back(pGameObject);
	Safe_AddRef(pGameObject);
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

		Safe_Release(pRenderObject);
	}

	m_RenderObject[ETOI(RENDERGROUP::PRIORITY)].clear();
}

void CRenderer::Render_NonBlend()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::NONBLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObject[ETOI(RENDERGROUP::NONBLEND)].clear();
}

void CRenderer::Render_Blend()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::BLEND)])
	{
		if (pRenderObject != nullptr)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObject[ETOI(RENDERGROUP::BLEND)].clear();
}

void CRenderer::Render_UI()
{
	for (auto& pRenderObject : m_RenderObject[ETOI(RENDERGROUP::UI)])
	{
		if(pRenderObject != nullptr)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObject[ETOI(RENDERGROUP::UI)].clear();
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* m_pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice,m_pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for(int i =0; i < ETOI(RENDERGROUP::END); i++)
	{
		for (auto& it : m_RenderObject[i])
			Safe_Release(it);
		m_RenderObject[i].clear();
	}

}
