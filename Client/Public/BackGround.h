#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CBackGround final:
    public CGameObject
{
public:
    struct BACKGROUND_DESC final:  public CGameObject::GAMEOBJECT_DESC
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

private:

public:
    static shared_ptr<CBackGround> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;

};

NS_END