#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CCustomFont :
    public CBase
{
private:

	CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* pFontFilePath);
	void Draw(const _tchar* pText, const _float2& vPosition, _fvector vColor);
private:
	ComPtr<ID3D11Device> m_pDevice = {};
	ComPtr<ID3D11DeviceContext> m_pContext = { };

	unique_ptr<SpriteBatch> m_pBatch;
	unique_ptr<SpriteFont>  m_pFont;
public:
	static shared_ptr<CCustomFont> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pFontFilePath) ;
	virtual void Free() override;
};

NS_END