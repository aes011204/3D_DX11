#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CFont_Manager :
	public CBase
{
private:
	CFont_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
public:
	virtual ~CFont_Manager() ;
public:
	HRESULT Add_Font(const _wstring& fontTag, const _tchar* fontFilePath);
	void Draw(const _wstring& fontTag, const _tchar* text, const _float2& position, _fvector color);
	_float2 Measure_String(const _wstring& fontTag, const _tchar* text);
private:
	ComPtr<ID3D11Device> m_device = {  };
	ComPtr<ID3D11DeviceContext> m_context = {  };
	map<const _wstring, shared_ptr<class CCustomFont>> m_fonts;


	shared_ptr<class CCustomFont> Find_Font(const _wstring& fontTag);
public:
	static unique_ptr<CFont_Manager> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	virtual void Free() override;
};

NS_END
