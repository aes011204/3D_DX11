
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
    struct EffDesc : public CGameObject::GameObjectDesc
    {
        EEffType type = {};
        weak_ptr<class CGameObject>target = {};

    };
private:
    CWaterEff(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CWaterEff(const CWaterEff& prototype);
public:
    virtual ~CWaterEff() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    virtual void RebindCom();

    void Set_Player(weak_ptr<class CGameObject> player) { m_player = player; }

    _float3 m_curPos = {};
    _float3 m_prevPos = {};

protected:
    HRESULT Bind_ShaderResources();
    HRESULT Ready_Components();

   
    EEffType   m_type={};
private:
    weak_ptr<class CGameObject> m_player = {};
private:

    shared_ptr<CShader> m_shaderCom = { nullptr };
    shared_ptr<CVIBuffer_Particle_Point> m_VIBufferCom = { nullptr };
   
    shared_ptr<CTexture> m_textureCom = { nullptr };
  

public:

    static shared_ptr<CWaterEff> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;



};

NS_END