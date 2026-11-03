#include "Font_Manager.h"
#include "CustomFont.h"

CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : m_device(device), m_context(context)
{
}

CFont_Manager::~CFont_Manager()
{
    Free();
}


HRESULT CFont_Manager::Add_Font(const _wstring& fontTag, const _tchar* fontFilePath)
{

    if (nullptr != Find_Font(fontTag))
        return E_FAIL;

    shared_ptr<CCustomFont> font = CCustomFont::Create(m_device, m_context, fontFilePath);
    if (nullptr == font)
        return E_FAIL;

    m_fonts.emplace(fontTag, font);

    return S_OK;
}

void CFont_Manager::Draw(const _wstring& fontTag, const _tchar* text, const _float2& position, _fvector color)
{
    auto    font = Find_Font(fontTag);

    if (nullptr == font)
        return;

    font->Draw(text, position, color);
}

_float2 CFont_Manager::Measure_String(const _wstring& fontTag, const _tchar* text)
{
    auto    font = Find_Font(fontTag);

    return font->Measure_String(text);
}

shared_ptr<class CCustomFont> CFont_Manager::Find_Font(const _wstring& fontTag)
{
    auto    iter = m_fonts.find(fontTag);
    if (iter == m_fonts.end())
        return nullptr;

    return iter->second;
}

unique_ptr<CFont_Manager> CFont_Manager::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
    unique_ptr<CFont_Manager> instance(new CFont_Manager(device, context));


    return instance;
}

void CFont_Manager::Free()
{

    __super::Free();

    for (auto& Pair : m_fonts)
        Pair.second.reset();
    m_fonts.clear();
}
