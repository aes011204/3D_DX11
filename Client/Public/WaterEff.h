
#pragma once
#include "GameObject.h"
#include "Client_Define.h"
NS_BEGIN(Engine)

class CVIBuffer_Particle_Point;
	class CVIBuffer_Particle_Rect;
NS_END
NS_BEGIN(Client)
class CWaterEff :
    public CGameObject
{
    public
        :
    struct EffDESC : public CGameObject::GAMEOBJECT_DESC
    {
        EFFTYPE eType = {};
        weak_ptr<class CGameObject>Target = {};

    };
private:
    CWaterEff(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CWaterEff(const CWaterEff& prototype);
public:
    virtual ~CWaterEff() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();

    void Set_Player(weak_ptr<class CGameObject> Player) { m_pPlayer = Player; }

    _float3 curPos = {};
    _float3 PrevPos = {};

protected:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

   
    EFFTYPE   eType={};
private:
    weak_ptr<class CGameObject> m_pPlayer = {};
private:

    shared_ptr<CShader> m_pShaderCom = { nullptr };
    shared_ptr<CVIBuffer_Particle_Point> m_pVIBufferCom = { nullptr };
   
    shared_ptr<CTexture> m_pTextureCom = { nullptr };
  

public:

    static shared_ptr<CWaterEff> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END