#include "ColliderObject.h"

#include "GameInstance.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "PlayerBoat.h"

CColliderObject::CColliderObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    : CGameObject{ pDevice, pContext }
{
}

CColliderObject::CColliderObject(const CColliderObject& prototype)
    : CGameObject{ prototype }
{
}

HRESULT CColliderObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CColliderObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;




    return S_OK;
}

void CColliderObject::Priority_Update(_float fTimeDelta)
{
}

void CColliderObject::Update(_float fTimeDelta)
{
    if (m_bIsDirtyCom)
    {
        RebindCom();
        m_bIsDirtyCom = false;
    }

    if (m_pColliderCom != nullptr)
        m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CColliderObject::Late_Update(_float fTimeDelta)
{
    m_pGameInstance.lock()->Add_RenderGroup(RENDERGROUP::NONBLEND, static_pointer_cast<CEntity>(shared_from_this()));
}

HRESULT CColliderObject::Render()
{
#ifdef _DEBUG
    if (m_pGameInstance.lock()->Get_IsDebug() == false)
        return S_OK;

    if (m_pColliderCom != nullptr && m_pColliderCom->Get_CollisionActive())
        m_pGameInstance.lock()->Add_DebugenderGroup(m_pColliderCom);
#endif

    return S_OK;
}

void CColliderObject::OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (other == nullptr)
        return;

    if (other->Get_MyLayer() == COLLISION_LAYER::PLAYER)
    {
        auto player = dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner());
        if (player != nullptr)
            player->Get_Demage();
    }
}

void CColliderObject::OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
    if (self == nullptr || other == nullptr)
        return;

    if (other->Get_MyLayer() != COLLISION_LAYER::PLAYER)
        return;

    auto player = dynamic_pointer_cast<CPlayerBoat>(other->Get_GOwner());
    if (player == nullptr || player->Get_TransformCom() == nullptr)
        return;

    constexpr float pushStrength = 0.2f;
    constexpr float minDirLength = 0.001f;

    _float3 selfCenter = self->Get_WorldCenter();
    _float3 otherCenter = other->Get_WorldCenter();

    XMVECTOR selfPos = XMLoadFloat3(&selfCenter);
    XMVECTOR playerPos = player->Get_TransformCom()->Get_Position();
    XMVECTOR pushDir = playerPos - selfPos;

    XMVECTOR flatDir = XMVectorSet(
        XMVectorGetX(pushDir),
        0.f,
        XMVectorGetZ(pushDir),
        0.f
    );

    float dirLength = XMVectorGetX(XMVector3Length(flatDir));
    if (dirLength < minDirLength)
    {
        XMVECTOR fallbackDir = -player->Get_TransformCom()->Get_State(STATE::LOOK);
        flatDir = XMVectorSet(
            XMVectorGetX(fallbackDir),
            0.f,
            XMVectorGetZ(fallbackDir),
            0.f
        );

        dirLength = XMVectorGetX(XMVector3Length(flatDir));
        if (dirLength < minDirLength)
        {
            flatDir = XMVectorSet(
                otherCenter.x - selfCenter.x,
                0.f,
                otherCenter.z - selfCenter.z,
                0.f
            );
            dirLength = XMVectorGetX(XMVector3Length(flatDir));
        }

        if (dirLength < minDirLength)
            flatDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);
    }

    flatDir = XMVector3Normalize(flatDir);
    XMVECTOR pushedPos = playerPos + flatDir * pushStrength;
    pushedPos = XMVectorSetY(pushedPos, XMVectorGetY(playerPos));

    player->Get_TransformCom()->Set_Position(pushedPos);
}

void CColliderObject::OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other)
{
}

void CColliderObject::OnGui()
{
    if (m_pColliderCom != nullptr)
        m_pColliderCom->OnGui();
}

void CColliderObject::RebindCom()
{
}

HRESULT CColliderObject::Ready_Components()
{
    CBounding_OBB::BOUNDING_OBB_DESC OBBDesc{};
    OBBDesc.vExtents = _float3(2.0f, 2.f, 1.7f);
    OBBDesc.vRadians = _float3(0.f, 52.5f, 0.f);
    OBBDesc.vCenter = _float3(55.7f, 0.f, -22.2f);
    OBBDesc.MyLayer = COLLISION_LAYER::MONSTERATT;
    OBBDesc.OtherMask = COLLISION_LAYER::PLAYER;

    if (FAILED(Add_Component(
        ETOI(LEVEL::GAMEPLAY),
        TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"),
        &m_pColliderCom,
        &OBBDesc)))
        return E_FAIL;

    m_pGameInstance.lock()->Add_Collider(m_pColliderCom);

    return S_OK;
}

shared_ptr<CColliderObject> CColliderObject::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    shared_ptr<CColliderObject> pInstance(new CColliderObject(pDevice, pContext),
        [](CColliderObject* p) { p->Free(); delete p; });

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CColliderObject");
    }

    return pInstance;
}

shared_ptr<CGameObject> CColliderObject::Clone(void* pArg)
{
    shared_ptr<CColliderObject> pInstance(new CColliderObject(*this),
        [](CColliderObject* p) { p->Free(); delete p; });

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CColliderObject");
    }

    return pInstance;
}

void CColliderObject::Free()
{
    __super::Free();
}
