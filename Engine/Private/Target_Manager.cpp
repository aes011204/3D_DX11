#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

	:m_pDevice(pDevice), m_pContext(pContext)
{
}

CTarget_Manager::CTarget_Manager(const CTarget_Manager& Prototype)
{
}

CTarget_Manager::~CTarget_Manager()
{
	Free();
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY,
                                          DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{

	if (nullptr != Find_RenderTarget(strTargetTag))
		return E_FAIL;

	shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iSizeX, iSizeY, ePixelFormat, vClearColor);

	if(nullptr== pRenderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(shared_ptr<CShader> pShader, const _char* pConstantName,
	const _wstring& strTargetTag)
{

	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Bind_ShaderResource(pShader, pConstantName);

	
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
	auto pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list <shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
	{
		list<shared_ptr<CRenderTarget>>        MRTList;
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, bool useDepth)
{

	list <shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;
	m_pContext->OMGetRenderTargets(1, m_pBackBuffer.GetAddressOf(), m_pOriginalDSV.GetAddressOf());

	//ComPtr<ID3D11RenderTargetView> pRenderTargets[8] = { nullptr };
	//
	//_uint       iNumRenderTargets = { 0 };
	//
	//for (auto& pRenderTarget : *pMRTList)
	//{
	//
	//	pRenderTarget->Clear();
	//
	//	pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV();
	//}
	//
	//m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets->GetAddressOf(), m_pOriginalDSV.Get());

	/*ID3D11ShaderResourceView* nullSRV[16] = {};
	m_pContext->PSSetShaderResources(0, 16, nullSRV);*/

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint       iNumRenderTargets = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV().Get();
	}

	//if (useDepth)
		m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_pOriginalDSV.Get());
	//else
	//	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr);
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, m_pBackBuffer.GetAddressOf(), m_pOriginalDSV.Get());

	m_pBackBuffer.Reset();
	m_pOriginalDSV.Reset();

	return S_OK;

}
#ifdef _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	shared_ptr<CRenderTarget> pRenderTarget = Find_RenderTarget(strTargetTag);
	if(nullptr == pRenderTarget)
	{
		return E_FAIL;
	}

	return pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);

}

HRESULT CTarget_Manager::Render(shared_ptr<CVIBuffer_Rect> pVIBuffer, shared_ptr <CShader> pShader, const _wstring& strMRTTag)
{
	list<shared_ptr<CRenderTarget>>* pMRTList = Find_MRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pVIBuffer, pShader);
	}

	return S_OK;
}
#endif
list<shared_ptr<CRenderTarget>>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto        iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

shared_ptr<CRenderTarget> CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
	auto        iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;

}

unique_ptr<CTarget_Manager> CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CTarget_Manager> pInstance(new CTarget_Manager(pDevice, pContext));

	
	return pInstance;
}


void CTarget_Manager::Free()
{
	CBase::Free();

	for(auto mrts : m_MRTs)
	{
		mrts.second.clear();
	}
	m_MRTs.clear();
	m_RenderTargets.clear();

	m_pDevice.Reset();
	m_pContext.Reset();
}
