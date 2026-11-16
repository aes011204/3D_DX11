#include "CustomFont.h"

CCustomFont::CCustomFont(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
    : m_device{ device }
    , m_context{ context }
{
}



HRESULT CCustomFont::Initialize(const _tchar* fontFilePath)
{
    m_batch = make_unique<SpriteBatch>(m_context.Get());
    m_font = make_unique<SpriteFont>(m_device.Get(), fontFilePath);
    return S_OK;
}

void CCustomFont::Draw(const _tchar* text, const _float2& position, _fvector color)
{
    m_batch->Begin();

    m_font->DrawString(m_batch.get(), text, position, color);

    m_batch->End();
}

shared_ptr<CCustomFont> CCustomFont::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context, const _tchar* fontFilePath)
{
    shared_ptr<CCustomFont> instance(new CCustomFont(device, context), [](CCustomFont* p) {p->Free(); delete p;});


    if (FAILED(instance->Initialize(fontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");

    }
    return instance;

}

_float2 CCustomFont::Measure_String(const _tchar* text)
{
    
    XMVECTOR size = m_font->MeasureString(text);

    _float2 result;
    XMStoreFloat2(&result, size);

 
    return result;
}

void CCustomFont::Free()
{
    __super::Free();

    m_batch.reset();
    m_font.reset();

    m_device.Reset();
    m_context.Reset();
}
