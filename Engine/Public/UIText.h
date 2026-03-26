#pragma once
#include "UI.h"



NS_BEGIN(Engine)

class ENGINE_DLL CUIText :
    public CUI
{
public:
    enum class FONTALINE { CENTER, RIGHT, LEFT, BOTTOM, UP,DEFAULT, END };

    struct TEXT_DESC : public CUI::UI_DESC
    {
        _wstring strText = L"";
        _wstring strFontTag = L"DefaultFont";
        _float4 TextColor = _float4{ 1.f, 1.f, 1.f, 1.f };
        //_bool Center = true;
        FONTALINE fontaline = FONTALINE::CENTER;
    };
protected:
    CUIText(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIText(const CUIText& prototype);
public:
    ~CUIText() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

    void Set_Text(const _wstring& text) { m_strText = text; }
    void Set_TextColor(const _float4 TextColor) { m_vTextColor = TextColor; }
    /// <IMGUI>
    virtual void OnGui() override;

    /// </summary>
public:
 
private:
    _wstring m_strText = L"";
    _wstring m_strFontTag = L"DefaultFont";
    _float4 m_vTextColor = { 1.f, 1.f, 1.f, 1.f }; // ±âº» Èò»ö
    //_bool m_bCenter = true;
    FONTALINE m_FontAline = FONTALINE::END;
protected:

public:
    static shared_ptr<CUIText> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    void Free() override;
};

NS_END

