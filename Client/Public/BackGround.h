#pragma once

#include "UIObject.h"
#include "Client_Define.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CTexture;
NS_END

NS_BEGIN(Client)
class CBackGround final:
    public CUIObject
{
public:
    struct BACKGROUND_DESC final:  public CUIObject::UIObject_Desc
    {
	    
    };
private:
    CBackGround(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CBackGround(const CBackGround&  prototype);
public:
	virtual ~CBackGround() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

protected:
    HRESULT Ready_Components();
private:
    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CVIBuffer_Rect> m_pVIBufferCom = { nullptr };
    shared_ptr<CTexture> m_pTextureCom = { nullptr };

public:
    static shared_ptr<CBackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;

};

NS_END