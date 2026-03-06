#pragma once
#include "UIRenderable.h"
// 패널의 역할 : 패널의 백 이미지, 레이아웃, 
NS_BEGIN(Engine)

class ENGINE_DLL CUIPanel :
    public CUIRenderable
{
public:
    struct UIPANEL_DESC : public CUIRenderable::RENDERABLE_DESC
    {
       /* _uint TextureComLevel = {};
        _wstring TextureProtoName = L"";*/

        _bool IsTrnasparent = { false };
        _bool IsFullScreen = { false };
        _bool IsUseLayout = { false };

        LAYOUT_DESC LayoutDesc;
    };
protected:
    CUIPanel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CUIPanel(const CUIPanel& prototype);
public:
    ~CUIPanel() = default;

public:

    HRESULT OnInit(void* pArg) override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    HRESULT OnRender()override;
    void OnClear()override;

    //HRESULT Ready_Components(_uint Level, _wstring protoName);
    //HRESULT Bind_ShaderResources();

    //void RebindCom()override;

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;


    /// <IMGUI>
    virtual void OnGui() override;

    /// </summary>
public:
    void Layout();
    HRESULT Add_Layout_Child(shared_ptr<CUI> child, _wstring UITag, _bool KeepWorldRect);

private:
    // 내부 계산 용
    int index = 0;

    LAYOUT_DESC m_LayoutDesc ;
  

    _bool m_IsTransparent = {};
    _bool m_IsFullScreen = {};
    _bool m_IsUseLayout = {};

protected:
  /*  shared_ptr<class CShader> m_pShaderCom = { nullptr };
    shared_ptr<class CVIBuffer> m_pVIBufferCom = { nullptr };
    shared_ptr<class CTexture> m_pTextureCom = { nullptr };*/
public:
      //static shared_ptr<CUIPanel> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

    void Free() override;
};

NS_END


