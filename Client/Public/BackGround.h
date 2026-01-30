#pragma once
#include "GameObject.h"

NS_BEGIN(Client)
class CBackGround final:
    public CGameObject
{
private:
    CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBackGround(const CBackGround&  rhs);
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
    static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBackGround* Clone(void* pArg) override;
    void Free() override;

};

NS_END