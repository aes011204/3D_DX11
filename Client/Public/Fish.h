#pragma once

#pragma once
#include "GameObject.h"
#include "Client_Define.h"
#include "MiniGame_Logic.h"
NS_BEGIN(Client)

	class CFish :
    public CGameObject
{
public:
    struct FISH_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _uint FishCount = {};

        _float2 Height = {};
        _float2 Size = {};
        float InitPosRad = {};
        _float2 AlphaTime = {};
        _float2 Speed = {};
        _float2 Radius = {};

        _uint fish_DefID = {};

    };


    struct  FishSData
    {
        _float4x4 FishMatrices={};

        float Height = {};
        float Size = {};
        float InitPosRad = {};
        float Alpha = {};
        float Speed = {};
        float Radius = {};
       
    };


private:
    CFish(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CFish(const CFish& prototype);
public:

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    int GetFishCount() const { return m_FishCount; }
    shared_ptr<CMiniGame_Logic> GetMiniGameLogic() const {return m_MiniGame_Logic;}
    void SetMiniGameLogic(shared_ptr<CMiniGame_Logic> logic) {m_MiniGame_Logic = logic;}

	virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;


    void Change_Cam(shared_ptr<CGameObject> m_Player);

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();
    _uint Get_fish_DefID() { return fish_DefID; };



protected:
    HRESULT Ready_Components();
private:

    shared_ptr<CShader> m_pShaderCom = {nullptr};
    shared_ptr<CModel> m_pModelCom = {nullptr};
    shared_ptr<CCollider> m_pColliderCom = { nullptr };

private:


   // _uint m_CurFishCount = {};

    _uint m_FishCount = {};
    float m_acc = {};
    vector<FishSData> m_Fishs = {};
    _uint fish_DefID = {};


    shared_ptr<class CMiniGame_Logic> m_MiniGame_Logic = { nullptr };

public:
    static shared_ptr<CFish> Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    shared_ptr<CGameObject> Clone(void* pArg) override;
    void Free() override;



};

NS_END