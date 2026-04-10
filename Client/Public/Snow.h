
#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Engine)
class CVIBuffer_Particle_Rect;
NS_END
NS_BEGIN(Client)
class CSnow :
    public CGameObject
{
private:
    CSnow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSnow(const CSnow& prototype);
public:
    virtual ~CSnow() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();

protected:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

   

private:

private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CVIBuffer_Particle_Rect> m_pVIBufferCom = { nullptr };
   
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
  

public:

    static shared_ptr<CSnow> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END