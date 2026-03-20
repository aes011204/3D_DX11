#pragma once
#include "UI.h"

NS_BEGIN(Engine)
class ENGINE_DLL  CUIRenderable :
    public CUI
{
public:
    struct RENDERABLE_DESC : public CUI::UI_DESC
    {
        _uint TextureComLevel = {};
        _wstring TextureProtoName = L"";

        _float4 PxSliceLRTB = {};
        bool bUseNineSlice = false;
        bool IsTransparent = false;
        bool bUseDark = true;
    };
protected:
    CUIRenderable(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIRenderable(const CUIRenderable& prototype);
public:
    virtual ~CUIRenderable() = default;


public:

    void Set_NotUseDark() { m_bUseDark = false; }
    void Set_Transparent(_bool isTransparent) { m_IsTransparent = isTransparent; }
    _bool Get_Transparent() { return m_IsTransparent; }

    //이건
    HRESULT Change_Texture(shared_ptr<CTexture> texture);
   

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
    HRESULT Bind_ShaderResources() override;
    virtual void RebindCom() override;

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

    void OnGui() override;

protected:

    _uint m_PassIndex = { 0 };
    bool m_bUseDark = true;
    float m_Dark = {}; //0~1

private:
    bool m_IsTransparent = { false };

    bool m_bUseNineSlice = false;
    NINESLICE_DESC m_SliceDesc = {};


    _uint m_TexProtoLevel = {};
    _wstring m_TexProtoName = L"";



    shared_ptr<class CShader> m_pShaderCom = { nullptr };
    shared_ptr<class CVIBuffer> m_pVIBufferCom = { nullptr };
    shared_ptr<class CTexture> m_pTextureCom = { nullptr };

public:
    static shared_ptr<CUIRenderable> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    //virtual shared_ptr<CUI> Clone(void* pArg);
    void Free()override;

};



NS_END