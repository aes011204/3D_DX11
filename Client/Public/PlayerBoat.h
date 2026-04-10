#pragma once
#include "ContainerObject.h"
#include "Client_Define.h"

NS_BEGIN(Engine)
class CCollider;
NS_END

NS_BEGIN(Client)
class CPlayerBoat :
    public CContainerObject
{
public:

    struct PLAYERBOAT_DESC : public CContainerObject::ContainerOBJ_DESC
    {


    };
private:
    CPlayerBoat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayerBoat(const CPlayerBoat& prototype);
public:
    virtual ~CPlayerBoat() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    void Location_Sea(_float fTimeDelta, CDInput_Manager* dinput);



    virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    void Get_Demage();



    virtual void OnGui() override;

    //virtual void RebindCom();
    HRESULT Bind_ShaderResources();

protected:
    HRESULT Ready_Components();
    HRESULT Ready_PartObjects();
public:
    //_float Get_Money() { return m_Money; }
    void Add_Money(_int money);
   /* void Set_BoatSpeed(_uint speed) { m_BoatSpeed = speed; }
    void Set_FishingSpeed(_uint speed) { m_FishingSpeed = speed; }
    void Set_Light(_uint light) { m_Light = light; }
    void Set_SeaMask(SEA_MASK mask) { m_SeaMask = mask; }*/
    void Set_ShipStats(_uint boatSpeed, _uint fishingSpeed, _uint light, SEA_MASK seaMask);
private:
    weak_ptr<class CSea_Manager> m_pSea_Manager = {};

    //shared_ptr<CShader> m_pShaderCom = { nullptr };
    //shared_ptr<CModel> m_pModelCom = { nullptr };
    //shared_ptr<CTexture> m_pTextureCom = { nullptr };

    shared_ptr<class CInventory> m_pInvenCom = { nullptr };
    shared_ptr< CCollider> m_pColliderCom = { nullptr };


private:
    int curBoatLevel = {};
    int m_Hp = { 3 };
    int m_MAXHp = {3};
    _float m_Money = {};
   _uint m_BoatSpeed = {};
   _uint m_FishingSpeed = {};
   _uint m_Light = {};
    SEA_MASK m_SeaMask = {}; // d이거 왜 있음

    LOCATIONSTATE m_Loacation = { LOCATIONSTATE:: SEA };

    shared_ptr<class CPlayerStateMachine> m_pPlayerStateMachine = {};
public:
    static shared_ptr<CPlayerBoat> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END