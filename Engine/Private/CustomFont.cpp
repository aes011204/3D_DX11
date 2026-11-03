#include "CustomFont.h"

CCustomFont::CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_device{ pDevice }
    , m_context{ pContext }
{
}



HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
    m_batch = make_unique<SpriteBatch>(m_context.Get());
    m_font = make_unique<SpriteFont>(m_device.Get(), pFontFilePath);
    return S_OK;
}

void CCustomFont::Draw(const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
    m_batch->Begin();

    m_font->DrawString(m_batch.get(), pText, vPosition, vColor);

    m_batch->End();
}

shared_ptr<CCustomFont> CCustomFont::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar* pFontFilePath)
{
    shared_ptr<CCustomFont> pInstance(new CCustomFont(pDevice, pContext), [](CCustomFont* p) {p->Free(); delete p;});


    if (FAILED(pInstance->Initialize(pFontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");

    }
    return pInstance;

}

_float2 CCustomFont::Measure_String(const _tchar* pText)
{
    
    XMVECTOR vSize = m_font->MeasureString(pText);

    _float2 vResult;
    XMStoreFloat2(&vResult, vSize);

 
    return vResult;
}

void CCustomFont::Free()
{
    __super::Free();

    m_batch.reset();
    m_font.reset();

    m_device.Reset();
    m_context.Reset();
}
