#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class CTarget_Manager :
	public CBase
{
protected:
	CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CTarget_Manager(const CTarget_Manager& Prototype);
public:
	virtual ~CTarget_Manager();

public:

	HRESULT Add_RenderTarget(const _wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader> pShader, const _char* pConstantName, const _wstring& strTargetTag);

	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag);

	HRESULT Begin_MRT(const _wstring& strMRTTag);
	HRESULT End_MRT();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(shared_ptr<class CVIBuffer_Rect> pVIBuffer, shared_ptr<class CShader> pShader, const _wstring& strMRTTag);
#endif
protected:

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr <ID3D11DeviceContext> m_pContext = { nullptr };

private:
	map<const _wstring, shared_ptr<class CRenderTarget>> m_RenderTargets;
	map<const _wstring, list<shared_ptr<class CRenderTarget>>> m_MRTs;


	list<shared_ptr<class CRenderTarget>>* Find_MRT(const _wstring& strMRTTag);

private:
	ComPtr<ID3D11RenderTargetView> m_pBackBuffer = { nullptr };
	ComPtr<ID3D11DepthStencilView> m_pOriginalDSV = { nullptr };
private:
	shared_ptr<class CRenderTarget> Find_RenderTarget(const _wstring& strTargetTag);
public:
	static unique_ptr<CTarget_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;


};

NS_END