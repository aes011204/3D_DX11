#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class CTarget_Manager :
	public CBase
{
protected:
	CTarget_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	CTarget_Manager(const CTarget_Manager& prototype);
public:
	virtual ~CTarget_Manager();

public:

	HRESULT Add_RenderTarget(const _wstring& targetTag, _uint sizeX, _uint sizeY, DXGI_FORMAT pixelFormat, const _float4& clearColor);
	virtual HRESULT Bind_ShaderResource(shared_ptr<class CShader> shader, const _char* constantName, const _wstring& targetTag);

	HRESULT Add_MRT(const _wstring& MRTTag, const _wstring& targetTag);

	HRESULT Begin_MRT(const _wstring& MRTTag, bool useDepth = true);
	HRESULT End_MRT();

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& targetTag, _float x, _float y, _float sizeX, _float sizeY);
	HRESULT Render(shared_ptr<class CVIBuffer_Rect> VIBuffer, shared_ptr<class CShader> shader, const _wstring& MRTTag);
#endif
protected:

private:
	ComPtr<ID3D11Device> m_device = { nullptr };
	ComPtr <ID3D11DeviceContext> m_context = { nullptr };

private:
	map<const _wstring, shared_ptr<class CRenderTarget>> m_renderTargets;
	map<const _wstring, list<shared_ptr<class CRenderTarget>>> m_MRTs;


	list<shared_ptr<class CRenderTarget>>* Find_MRT(const _wstring& MRTTag);

private:
	ComPtr<ID3D11RenderTargetView> m_backBuffer = { nullptr };
	ComPtr<ID3D11DepthStencilView> m_originalDSV = { nullptr };
private:
	shared_ptr<class CRenderTarget> Find_RenderTarget(const _wstring& targetTag);
public:
	static unique_ptr<CTarget_Manager> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual void Free() override;


};

NS_END