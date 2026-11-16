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
    struct BackGroundDesc final:  public CUIObject::UIObjectDesc
    {
	    
    };
private:
    CBackGround(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CBackGround(const CBackGround&  prototype);
public:
	virtual ~CBackGround() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

protected:
    HRESULT Ready_Components();
private:
    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CVIBuffer_Rect> m_VIBufferCom = { nullptr };
    shared_ptr<CTexture> m_textureCom = { nullptr };

public:
    static shared_ptr<CBackGround> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;

};

NS_END