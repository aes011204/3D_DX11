#include "Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)

	:m_device(device), m_context(context)
{
}

CTarget_Manager::CTarget_Manager(const CTarget_Manager& prototype)
{
}

CTarget_Manager::~CTarget_Manager()
{
	Free();
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& targetTag, _uint sizeX, _uint sizeY,
                                          DXGI_FORMAT pixelFormat, const _float4& clearColor)
{

	if (nullptr != Find_RenderTarget(targetTag))
		return E_FAIL;

	shared_ptr<CRenderTarget> renderTarget = CRenderTarget::Create(m_device, m_context, sizeX, sizeY, pixelFormat, clearColor);

	if(nullptr== renderTarget)
		return E_FAIL;

	m_renderTargets.emplace(targetTag, renderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Bind_ShaderResource(shared_ptr<CShader> shader, const _char* constantName,
	const _wstring& targetTag)
{

	shared_ptr<CRenderTarget> renderTarget = Find_RenderTarget(targetTag);
	if (nullptr == renderTarget)
		return E_FAIL;

	return renderTarget->Bind_ShaderResource(shader, constantName);

	
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& MRTTag, const _wstring& targetTag)
{
	auto renderTarget = Find_RenderTarget(targetTag);
	if (nullptr == renderTarget)
		return E_FAIL;

	list <shared_ptr<CRenderTarget>>* foundMRTList = Find_MRT(MRTTag);
	if (nullptr == foundMRTList)
	{
		list<shared_ptr<CRenderTarget>>        newMRTList;
		newMRTList.push_back(renderTarget);

		m_MRTs.emplace(MRTTag, newMRTList);
	}
	else
		foundMRTList->push_back(renderTarget);

}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& MRTTag, bool useDepth)
{

	list <shared_ptr<CRenderTarget>>* foundMRTList = Find_MRT(MRTTag);
	if (nullptr == foundMRTList)
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

	ID3D11RenderTargetView* renderTargets[8] = { nullptr };

	_uint       numRenderTargets = { 0 };

	for (auto& renderTarget : *foundMRTList)
	{
		renderTarget->Clear();
		renderTargets[numRenderTargets++] = renderTarget->Get_RTV().Get();
	}

	//if (useDepth)
		m_context->OMSetRenderTargets(numRenderTargets, renderTargets, m_originalDSV.Get());
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
HRESULT CTarget_Manager::Ready_Debug(const _wstring& targetTag, _float x, _float y, _float sizeX, _float sizeY)
{
	shared_ptr<CRenderTarget> renderTarget = Find_RenderTarget(targetTag);
	if(nullptr == renderTarget)
	{
		return E_FAIL;
	}

	return renderTarget->Ready_Debug(x, y, sizeX, sizeY);

}

HRESULT CTarget_Manager::Render(shared_ptr<CVIBuffer_Rect> VIBuffer, shared_ptr <CShader> shader, const _wstring& MRTTag)
{
	list<shared_ptr<CRenderTarget>>* foundMRTList = Find_MRT(MRTTag);

	if (nullptr == foundMRTList)
		return E_FAIL;

	for (auto& renderTarget : *foundMRTList)
	{
		renderTarget->Render(VIBuffer, shader);
	}

	return S_OK;
}
#endif
list<shared_ptr<CRenderTarget>>* CTarget_Manager::Find_MRT(const _wstring& MRTTag)
{
	auto        iter = m_MRTs.find(MRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}

shared_ptr<CRenderTarget> CTarget_Manager::Find_RenderTarget(const _wstring& targetTag)
{
	auto        iter = m_renderTargets.find(targetTag);
	if (iter == m_renderTargets.end())
		return nullptr;

	return iter->second;

}

unique_ptr<CTarget_Manager> CTarget_Manager::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	unique_ptr<CTarget_Manager> instance(new CTarget_Manager(device, context));

	
	return instance;
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
