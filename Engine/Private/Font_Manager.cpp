#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
}

CFont_Manager::~CFont_Manager()
{
    Free();
}


HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{

    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    shared_ptr<CCustomFont> pFont = CCustomFont::Create(m_pDevice, m_pContext, pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}

void CFont_Manager::Draw(const _wstring& strFontTag, const _tchar* pText, const _float2& vPosition, _fvector vColor)
{
    auto    pFont = Find_Font(strFontTag);

    if (nullptr == pFont)
        return;

    pFont->Draw(pText, vPosition, vColor);
}

_float2 CFont_Manager::Measure_String(const _wstring& strFontTag, const _tchar* pText)
{
    auto    pFont = Find_Font(strFontTag);

    return pFont->Measure_String(pText);
}

shared_ptr<class CCustomFont> CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);
    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

unique_ptr<CFont_Manager> CFont_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    unique_ptr<CFont_Manager> pInstance(new CFont_Manager(pDevice, pContext));


    return pInstance;
}

void CFont_Manager::Free()
{

    __super::Free();

    for (auto& Pair : m_Fonts)
        Pair.second.reset();
    m_Fonts.clear();
}
