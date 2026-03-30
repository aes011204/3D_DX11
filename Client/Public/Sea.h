#pragma once
#include "GameObject.h"
#include "Client_Define.h"

NS_BEGIN(Engine)

class CVIBuffer_Sea;
	class CShader;
class CVIBuffer;
class CTexture;
NS_END

NS_BEGIN(Client)
class CSea final:
    public CGameObject
{
public:
    struct SEA_DESC final : public CGameObject::GAMEOBJECT_DESC
    {

    };
private:
    CSea(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSea(const CSea& prototype);
public:
    virtual ~CSea() = default;

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
    HRESULT Bind_ShaderResources();
private:
    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CVIBuffer> m_pVIBufferCom = { nullptr };
    shared_ptr<CVIBuffer_Sea> m_pVIBufferComCashing = { nullptr };
    shared_ptr<CTexture> m_pTextureCom = { nullptr };


    float m_AccTime = {};
public:
    static shared_ptr<CSea> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;

};

NS_END