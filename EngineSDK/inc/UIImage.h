#pragma once
#include "UI.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUIImage :
    public CUI
{
public:
    struct UIIMAGE_DESC : public CUI::UI_DESC
    {
        _uint TextureComLevel = {};
        _wstring TextureProtoName =L"";
        _float4 PxSliceLRTB = {};
        bool bUseNineSlice = false;
        
    };
protected:
    CUIImage(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIImage(const CUIImage& prototype);
public:
    virtual ~CUIImage() = default;

protected:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    HRESULT OnInit(void* pArg)override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    HRESULT Ready_Components(_uint Level, _wstring protoName);
    HRESULT Bind_ShaderResources();

/// <IMGUI>
    virtual void OnGui() override;

/// </summary>

private:
    bool m_bUseNineSlice = false;
    _uint m_PassIndex = {0};
    //vector<IUIButtonBehavior*> m_behavior; // 인터페이스 클래스

    NINESLICE_DESC m_SliceDesc = {};
protected:

    shared_ptr<class CShader> m_pShaderCom = { nullptr };
    shared_ptr<class CVIBuffer_Rect> m_pVIBufferCom = { nullptr };
    shared_ptr<class CTexture> m_pTextureCom = { nullptr };



public:
    static shared_ptr<CUIImage> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    //virtual shared_ptr<CUI> Clone(void* pArg);
    void Free()override;
};

NS_END