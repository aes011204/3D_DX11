
#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Engine)

class CVIBuffer_Particle_Point;
	class CVIBuffer_Particle_Rect;
NS_END
NS_BEGIN(Client)
class CExplosion :
    public CGameObject
{
private:
    CExplosion(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CExplosion(const CExplosion& prototype);
public:
    virtual ~CExplosion() = default;

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
    shared_ptr<CVIBuffer_Particle_Point> m_pVIBufferCom = { nullptr };
   
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
  

public:

    static shared_ptr<CExplosion> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END