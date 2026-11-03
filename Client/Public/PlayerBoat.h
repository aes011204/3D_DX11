#pragma once
#include "ContainerObject.h"
#include "Client_Define.h"


NS_BEGIN(Engine)
	class CCollider;
class CDInput_Manager;
class CLight;
NS_END

NS_BEGIN(Client)
class CPlayerBoat :
    public CContainerObject
{
public:

    struct PlayerBoatDesc : public CContainerObject::ContainerObjDesc
    {


    };
private:
    CPlayerBoat(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CPlayerBoat(const CPlayerBoat& prototype);
public:
    virtual ~CPlayerBoat() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    void Location_Sea(_float timeDelta, CDInput_Manager* dinput);
    void Dead();



    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    void Get_Damage(_uint damage=1);
    _uint Get_DamageFixPrice() { return (m_maxHP - m_HP) * 80.f; };

    float Get_RodSpeed() const{return m_rodSpeed;}
    shared_ptr<CInventory> GetInventory() const{return m_invenCom;}

    virtual void OnGui() override;
    _bool Get_WasDead() {
        return m_wasDead;
    };
    //virtual void RebindCom();
    HRESULT Bind_ShaderResources();

    weak_ptr<class CInventory> Get_StorageCom() { return m_storageCom; }

protected:
    HRESULT Ready_Components();
    HRESULT Ready_PartObjects();
public:
    //_float Get_Money() { return m_Money; }
    void Add_Money(_float money);

    int GetMoney() const{return m_money;}
    _uint Get_BoatSpeed() const{return m_boatSpeed;}
    void Set_BoatSpeed(_uint speed){m_boatSpeed = speed;}
    void SetHPFull();
    bool IsInputBlocked() const { return m_inputBlockTime > 0.f; }

    bool MinusMoney(int amount);
    weak_ptr<CLight> Get_Light() const {return m_lightObj;}

   /* void Set_BoatSpeed(_uint speed) { m_BoatSpeed = speed; }
    void Set_FishingSpeed(_uint speed) { m_RodSpeed = speed; }
    void Set_Light(_uint light) { m_Light = light; }
    void Set_SeaMask(SEA_MASK mask) { m_SeaMask = mask; }*/
    void Set_ShipStats(_uint boatSpeed, _uint fishingSpeed, _uint light, SeaMask seaMask, _float invenMoney);
private:
    weak_ptr<class CSea_Manager> m_seaManager = {};

    //shared_ptr<CShader> m_pShaderCom = { nullptr };
    //shared_ptr<CModel> m_pModelCom = { nullptr };
    //shared_ptr<CTexture> m_pTextureCom = { nullptr };

    shared_ptr<class CInventory> m_invenCom = { nullptr };
    shared_ptr<class CInventory> m_storageCom = { nullptr };
    shared_ptr< CCollider> m_colliderCom = { nullptr };

    float m_invincibleTime = {};
    float m_inputBlockTime = {};
    _bool m_wasDead = { false };
private:
    int m_curBoatLevel = {};
    int m_HP = {4 };
    int m_maxHP = {4};
    _float m_money = {};
   _uint m_boatSpeed = {10}; //기본은 10
   _float m_rodSpeed = {10.f};
   _uint m_light = {};
   SeaMask m_seaMask = {}; // d이거 왜 있음
   float m_invenMoney = {}; // d이거 왜 있음

   shared_ptr<CLight> m_lightObj = { nullptr };
  // _bool On_Light = { false };
    ELocationState m_location = { ELocationState:: SEA };

    shared_ptr<class CPlayerStateMachine> m_playerStateMachine = {};
public:
    static shared_ptr<CPlayerBoat> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;



};

NS_END
