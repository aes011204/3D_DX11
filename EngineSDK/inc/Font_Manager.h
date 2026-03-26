#pragma once
#include "Base.h"
NS_BEGIN(Engine)
class CFont_Manager :
	public CBase
{
private:
	CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
public:
	virtual ~CFont_Manager() ;
public:
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
	void Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor);
	_float2 Measure_String(const _wstring& strFontTag, const _tchar* pText);
private:
	ComPtr<ID3D11Device> m_pDevice = {  };
	ComPtr<ID3D11DeviceContext> m_pContext = {  };
	map<const _wstring, shared_ptr<class CCustomFont>> m_Fonts;


	shared_ptr<class CCustomFont> Find_Font(const _wstring& strFontTag);
public:
	static unique_ptr<CFont_Manager> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual void Free() override;
};

NS_END
