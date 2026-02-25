#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
    CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CComponent(const CComponent& rhs);
public:
    virtual ~CComponent() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

    virtual void Save_ToJson(nlohmann::json& j) {};

    //void Set_ProtoTag(_wstring str) { m_strProtoTag = str; }
    //void Set_ProtoLevel(_uint Level) { m_strProtoLevel = Level; }
    //_wstring Get_ProtoTag() { return m_strProtoTag; }
    //_uint Get_ProtoLevel() { return m_strProtoLevel; }

protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = {};

protected:
    //_wstring m_strProtoTag = {};
    //_uint m_strProtoLevel = {};

public:
    //void* pArg : 사본객체의 추가적인 초기화가 필요 할수 있음 런타임 할당 되는 주소 같은거
    virtual shared_ptr<CComponent> Clone(void* pArg) = 0;

	void Free() override;
};

NS_END

