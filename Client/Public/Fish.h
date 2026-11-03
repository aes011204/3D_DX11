#pragma once

#pragma once
#include "GameObject.h"
#include "Client_Define.h"
#include "MiniGame_Logic.h"
NS_BEGIN(Client)
class CMiniGame;

	class CFish :
    public CGameObject
{
public:
    struct FishDesc : public CGameObject::GameObjectDesc
    {
        _uint fishCount = {};

        _float2 height = {};
        _float2 size = {};
        float initPosRad = {};
        _float2 alphaTime = {};
        _float2 speed = {};
        _float2 radius = {};

        _uint fishDefID = {};
        EMiniGameType miniGameType = {};

    };


    struct  FishData
    {
        _float4x4 fishMatrices={};

        float height = {};
        float size = {};
        float initPosRad = {};
        float alpha = {};
        float speed = {};
        float radius = {};
       
    };


private:
    CFish(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    CFish(const CFish& prototype);
public:

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Priority_Update(_float timeDelta) override;
    virtual void Update(_float timeDelta) override;
    virtual void Late_Update(_float timeDelta) override;
    virtual HRESULT Render() override;

    virtual void OnGui() override;

    int GetInitFishCount() const { return m_initFishCount; }
    int GetCurFishCount() const {
        if (m_miniGameLogic == nullptr)
            return -1;

        return m_miniGameLogic->GetFishCount();
    }
    shared_ptr<CMiniGame> GetMiniGameLogic() const {return m_miniGameLogic;}
    void SetMiniGameLogic(shared_ptr<class CMiniGame> logic) {m_miniGameLogic = logic;}

	virtual void OnBeginOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnEndOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;
    virtual void OnStayOverlap(shared_ptr<CCollider> self, shared_ptr<CCollider> other) override;

    EMiniGameType GetMiniGameType() const{return m_miniGameType;}

    void Change_Cam(shared_ptr<CGameObject> m_player);

    virtual void RebindCom();
    HRESULT Bind_ShaderResources();
    _uint Get_FishDefID() { return m_fishDefID; };

    //void Set_FishCount(_uint cunt) { m_CurFishCount = cunt; };

protected:
    HRESULT Ready_Components();
private:

    shared_ptr<CShader> m_shaderCom = {nullptr};
    shared_ptr<CModel> m_modelCom = {nullptr};
    shared_ptr<CCollider> m_colliderCom = { nullptr };

private:

    EMiniGameType m_miniGameType = {};
   //_uint m_CurFishCount = {};

    _uint m_initFishCount = {};
    float m_acc = {};
    vector<FishData> m_fishes = {};
    _uint m_fishDefID = {};


    shared_ptr<class CMiniGame> m_miniGameLogic = { nullptr };

public:
    static shared_ptr<CFish> Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
    shared_ptr<CGameObject> Clone(void* arg) override;
    void Free() override;



};

NS_END