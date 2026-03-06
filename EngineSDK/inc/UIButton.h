#pragma once
#include "UIRenderable.h"


NS_BEGIN(Engine)

class ENGINE_DLL CUIButton :
    public CUIRenderable
{
public:
    struct UIBUTTON_DESC : public CUIRenderable::RENDERABLE_DESC
    {

        function<void(CUIButton*)> ClickEvent = { nullptr };
        function<void(CUIButton*)> OverlapStartEvent = { nullptr };
        function<void(CUIButton*)> OverlapEndEvent = { nullptr };

        _uint Index = {};
    };
protected:
    CUIButton(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIButton(const CUIButton& prototype);
public:
    virtual ~CUIButton() = default;

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
    void ChangeState(BUTTON_STATE next);

  /*  HRESULT Ready_Components(_uint Level,_wstring protoName);

    void RebindCom() override;*/

    void Save_ToJson(nlohmann::json& j)override;
    void Load_FromJson(nlohmann::json& j)override;

private:
    bool  m_ClickInside = { false };
    BUTTON_STATE m_UIState = BUTTON_STATE::NONE;
    function<void(CUIButton*)> m_ClickEvent = { nullptr };
    function<void(CUIButton*)> m_OverlapStartEvent = { nullptr };
    function<void(CUIButton*)> m_OverlapEndEvent = {nullptr};


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