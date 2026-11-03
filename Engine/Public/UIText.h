#pragma once
#include "UI.h"



NS_BEGIN(Engine)

class ENGINE_DLL CUIText :
    public CUI
{
public:
    enum class EFontAlign { CENTER, RIGHT, LEFT, BOTTOM, UP,DEFAULT, END };

    struct TextDesc : public CUI::UIDesc
    {
        _wstring text = L"";
        _wstring fontTag = L"DefaultFont";
        _float4 textColor = _float4{ 1.f, 1.f, 1.f, 1.f };
        //_bool Center = true;
        EFontAlign fontAlign = EFontAlign::CENTER;
    };
protected:
    CUIText(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CUIText(const CUIText& prototype);
public:
    ~CUIText() = default;

public:

    HRESULT OnInit(void* arg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    void OnPrepareRender() override;
    HRESULT OnRender()override;
    void OnClear()override;

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

    _float2 Get_TextSize() { return m_textSize; }

    void Set_Text(const _wstring& text) {m_text = text; m_isChangeText = true; m_textSize = m_gameInstance.lock()->Measure_String(m_fontTag, m_text.c_str());
    }
    void Set_TextColor(const _float4 textColor) { m_textColor = textColor; }
    /// <IMGUI>
    virtual void OnGui() override;

    /// </summary>
public:
 
private:
    _wstring m_text = L"";
    _wstring m_fontTag = L"DefaultFont";
    _float4 m_textColor = { 1.f, 1.f, 1.f, 1.f }; // 기본 흰색
    //_bool m_bCenter = true;
    EFontAlign m_fontAline = EFontAlign::END;
    _float2 m_textSize = {};
    _bool m_isChangeText = {true};
protected:

public:
    static shared_ptr<CUIText> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);

    void Free() override;
};

NS_END

