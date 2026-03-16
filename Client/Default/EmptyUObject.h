#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CEmptyUObject final:
    public CGameObject
{
public:
    struct EMPTYOBJECT_DESC final : public CGameObject::GAMEOBJECT_DESC
    {

    };
private:
    CEmptyUObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CEmptyUObject(const CEmptyUObject& prototype);
public:
    virtual ~CEmptyUObject() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom()override;

protected:
    HRESULT Ready_Components();
private:


private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CVIBuffer> m_pVIBufferCom = { nullptr }; // 이거 보관을 이걸로 하는것 뿐 실제는 자식이 들어있음
    shared_ptr<CTexture> m_pTextureCom = { nullptr };

public:
    static shared_ptr<CEmptyUObject> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;

};

NS_END