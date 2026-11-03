#pragma once
#include "UI.h"


NS_BEGIN(Engine)
class ENGINE_DLL  CUIRenderable :
    public CUI
{
public:

    enum class UIShaderType
    {
        Default,
        Radial,
        Prograss,
    };

    struct RenderableDesc : public CUI::UIDesc
    {
        _uint textureComLevel = {};
        _wstring textureProtoName = L"";
        _uint textureIndex = 0;

        UIShaderType shaderType = UIShaderType::Default;

        _float4 pxSliceLRTB = {};
        bool useNineSlice = false;
        bool isTransparent = false;
        bool useDark = true;
    };
protected:
    CUIRenderable(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIRenderable(const CUIRenderable& prototype);
public:
    virtual ~CUIRenderable() = default;


public:
    _bool Get_IsDark() { return m_useDark; }
    void Set_UseDark(_bool useDark) { m_useDark = useDark; }
    void Set_Dark01(_float dark01) { m_dark = dark01; }

    void Set_UseColorMix(_bool useColorMix) { m_useColorMix = useColorMix; }
    void Set_ColorMix(_float4 color) { m_color = color; }

    void Set_Alpha(_float alpha) { m_alpha = alpha; }

    void Set_UseColorFlat(_bool useColorFlat) { m_useColorFlat = useColorFlat; }
    void Set_ColorFlat(_float4 colorFlat) { m_colorFlat = colorFlat; }

    void Set_Transparent(_bool isTransparent) { m_isTransparent = isTransparent; }
    _bool Get_Transparent() { return m_isTransparent; }

    ////////////////////////
    const _float4& Get_BaseColor() const { return m_baseColor; }
    void Set_BaseColor(const _float4& vBaseColor) { m_baseColor = vBaseColor; }
    const _float4& Get_ZoneColor() const { return m_zoneColor; }
    void Set_ZoneColor(const _float4& vZoneColor) { m_zoneColor = vZoneColor; }


    //이건
    HRESULT Change_Texture(shared_ptr<CTexture> texture, _uint TexIndex = 0);
   
    HRESULT Set_TextureIndex(_uint index);



protected:
    // ui의 생명주기 정책에 따라 앤진 생명주기 안에서 호출 함
    HRESULT OnInit(void* pArg)override;
    void OnActive()override;
    void OnInActive()override;
    void OnDisabled()override;
    void OnUpdate(const _float& timeDelta)override;
    void OnLateUpdate()override;
    void OnPrepareRender() override;
    HRESULT OnRender()override;
    void OnClear()override;

    HRESULT Ready_Components(_uint Level, _wstring protoName);
    HRESULT Bind_ShaderResources() override;
    HRESULT Bind_ShaderResources_Default();
    HRESULT Bind_ShaderResources_Radius();
    virtual void RebindCom() override;

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

    void OnGui() override;

protected:

    _uint m_passIndex = { 0 };

private:

    UIShaderType m_shaderType = UIShaderType::Default;

    // Default 
    bool m_useDark = true;
    float m_dark = {}; //0~1

    bool m_useColorMix = true;
    bool m_useColorFlat = true;
    _float4 m_color = {}; //0~11
    _float4 m_colorFlat = {};

    //Radial
    bool m_useRadial = false;
    _float4 m_baseColor = {};
    _float4 m_zoneColor = {};

	//

    bool m_isTransparent = { false }; // 렌더 유무

    bool m_useNineSlice = false;
    NineSliceDesc m_sliceDesc = {};

    // 공통
    _float m_alpha = 1.f;

    _uint m_texProtoLevel = {};
    _wstring m_texProtoName = L"";
    _uint m_texIndex = 0;


    shared_ptr<class CShader> m_shaderCom = { nullptr };
    shared_ptr<class CVIBuffer> m_VIBufferCom = { nullptr };
    shared_ptr<class CTexture> m_textureCom = { nullptr };

public:
    static shared_ptr<CUIRenderable> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    //virtual shared_ptr<CUI> Clone(void* pArg);
    void Free()override;

};



NS_END