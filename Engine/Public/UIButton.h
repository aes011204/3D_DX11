#pragma once
#include "UIRenderable.h"


NS_BEGIN(Engine)

class ENGINE_DLL CUIButton :
    public CUIRenderable
{
public:
    struct UIButtonDesc : public CUIRenderable::RenderableDesc
    {

        function<void(CUIButton*)> ClickEvent = { nullptr };
        function<void(CUIButton*)> OverlapStartEvent = { nullptr };
        function<void(CUIButton*)> OverlapEndEvent = { nullptr };

        _uint typeIndex = {};
    };
protected:
    CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIButton(const CUIButton& prototype);
public:
    virtual ~CUIButton() = default;

    void ChangeState(EButtonState next);
    _uint Get_TypeIndex();

    void Set_ButtonState(_bool useButtonState) { m_isButtonState = useButtonState; };
    void Set_SelectState(_bool useSelectState) { m_useSelect = useSelectState; };
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

    void ProcessInput();

  /*  HRESULT Ready_Components(_uint Level,_wstring protoName);

    void RebindCom() override;*/

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

private:

    _bool m_useSelect = {true};
    _uint m_typeIndex = {};

    bool  m_isClickInside = { false };
    EButtonState m_UIState = EButtonState::NORMAL;
    function<void(CUIButton*)> m_clickEvent = { nullptr };
    function<void(CUIButton*)> m_overlapStartEvent = { nullptr };
    function<void(CUIButton*)> m_overlapEndEvent = {nullptr};

    _bool  m_isButtonState = true;
protected:
  /*  shared_ptr<class CShader> m_pShaderCom = { nullptr };
    shared_ptr<class CVIBuffer> m_pVIBufferCom = { nullptr };
    shared_ptr<class CTexture> m_pTextureCom = { nullptr };*/



public:
    static shared_ptr<CUIButton> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	//virtual shared_ptr<CUI> Clone(void* pArg);
    void Free()override;

};

NS_END