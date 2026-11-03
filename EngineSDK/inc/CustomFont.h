#pragma once
#include "Base.h"
NS_BEGIN(Engine)

class CCustomFont :
    public CBase
{
private:

	CCustomFont(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

	virtual ~CCustomFont() = default;

public:
	HRESULT Initialize(const _tchar* fontFilePath);
	void Draw(const _tchar* text, const _float2& position, _fvector color);

	_float2 Measure_String(const _tchar* text);
private:
	ComPtr<ID3D11Device> m_device = {};
	ComPtr<ID3D11DeviceContext> m_context = { };

	unique_ptr<SpriteBatch> m_batch;
	unique_ptr<SpriteFont>  m_font;
public:
	static shared_ptr<CCustomFont> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* fontFilePath) ;
	virtual void Free() override;
};

NS_END