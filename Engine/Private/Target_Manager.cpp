#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)

	:m_device(pDevice), m_context(pContext)
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

	shared_ptr<CRenderTarget> pRenderTarget = CRenderTarget::Create(m_device, m_context, iSizeX, iSizeY, ePixelFormat, vClearColor);

	if(nullptr== pRenderTarget)
		return E_FAIL;

	m_renderTargets.emplace(strTargetTag, pRenderTarget);

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
	m_context->OMGetRenderTargets(1, m_backBuffer.GetAddressOf(), m_originalDSV.GetAddressOf());

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

	ID3D11ShaderResourceView* nullSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
	m_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
	m_context->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);
	m_context->GSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, nullSRV);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint       iNumRenderTargets = { 0 };

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iNumRenderTargets++] = pRenderTarget->Get_RTV().Get();
	}

	//if (useDepth)
		m_context->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, m_originalDSV.Get());
	//else
	//	m_pContext->OMSetRenderTargets(iNumRenderTargets, pRenderTargets, nullptr);
	return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
	m_context->OMSetRenderTargets(1, m_backBuffer.GetAddressOf(), m_originalDSV.Get());

	m_backBuffer.Reset();
	m_originalDSV.Reset();

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
	auto        iter = m_renderTargets.find(strTargetTag);
	if (iter == m_renderTargets.end())
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
	m_renderTargets.clear();

	m_device.Reset();
	m_context.Reset();
}
