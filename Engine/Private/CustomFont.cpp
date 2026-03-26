#include "CustomFont.h"

CCustomFont::CCustomFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
}



HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
    m_pBatch = make_unique<SpriteBatch>(m_pContext.Get());
    m_pFont = make_unique<SpriteFont>(m_pDevice.Get(), pFontFilePath);
    return S_OK;
}

void CCustomFont::Draw(const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch.get(), pText, vPosition, vColor);

    m_pBatch->End();
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
    
    XMVECTOR vSize = m_pFont->MeasureString(pText);

    _float2 vResult;
    XMStoreFloat2(&vResult, vSize);

 
    return vResult;
}

void CCustomFont::Free()
{
    __super::Free();

    m_pBatch.reset();
    m_pFont.reset();
}
