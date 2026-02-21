#pragma once
#include "Base.h"


NS_BEGIN(Engine)

class CComponent;

class ENGINE_DLL CEntity abstract:
    public CBase
{
protected:
    CEntity(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CEntity(const CEntity& prototype);
public:
    virtual ~CEntity() = default;

public:
    //virtual HRESULT Initialize_Prototype() {};
    //virtual HRESULT Initialize(void* pArg) {};
    //virtual void Priority_Update(_float fTimeDelta) {};
    //virtual void Update(_float fTimeDelta) {};
    //virtual void Late_Update(_float fTimeDelta) {};
    virtual HRESULT Render() { return S_OK; }

	shared_ptr<CComponent> Get_Component(const _wstring& strComponentTag);
	template <typename T>
	shared_ptr<T> Get_Component(const _wstring& strComponentTag)
	{
		auto it = m_Components.find(strComponentTag);
		if (it == m_Components.end())
			return nullptr;
		shared_ptr<T> pCom = dynamic_pointer_cast<T>(it->second);

		return pCom;
	}


	HRESULT Remove_Component(const _wstring& strComponentTag);

	virtual void Load_FromJson(nlohmann::json& j) ;


/// <IMGUI>

    map<const _wstring, class shared_ptr<CComponent> >& Get_ComponentMap() {return m_Components;};
/// </summary>
protected:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
    weak_ptr<class CGameInstance> m_pGameInstance = { };

protected:
    // 원형객체를 복제하여 맵에도 추가하고 + 자식객체의 멤버변수에도 셋팅해준다
	template <typename T>
	HRESULT Add_Component(_uint iProtoLevelIndex, const _wstring& strProtoTag, const _wstring& strComponentTag,
		shared_ptr<T>& pOut, void* pArg)
	{

		shared_ptr<CComponent> tmpComp = Get_Component(strComponentTag);
		if (tmpComp != nullptr)
		{
			/*MSG_BOX("Failed Add Component : Same Key exist");
			return E_FAIL;*/
			if (FAILED(Remove_Component(strComponentTag)))
			{
				return E_FAIL;
			}
		}

		shared_ptr<CComponent> CloneComp = dynamic_pointer_cast<CComponent>(
			m_pGameInstance.lock()->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevelIndex, strProtoTag, pArg));
		if (CloneComp == nullptr)
			return E_FAIL;

		shared_ptr<T> pCastComp = dynamic_pointer_cast<T>(CloneComp);
		if (pCastComp == nullptr)
		{
			MSG_BOX("Failed Add Component : Wrong Type(it's should Component's Child)");
			return E_FAIL;
		}

	
			pOut = pCastComp;
		
		m_Components.emplace(strComponentTag, pCastComp);

		m_bIsDirtyCom = true;

		return S_OK;

	}

	// 
	//template <typename T>
	//HRESULT Change_Component(_uint iProtoLevelIndex, const _wstring& strProtoTag, const _wstring& strComponentTag,
	//	shared_ptr<T>& pOut, void* pArg)
	//{

	//	//shared_ptr<CComponent> tmpComp = Get_Component(strComponentTag);
	//	//if (tmpComp != nullptr)
	//	//{
	//	//	Remove_Component(strComponentTag);
	//	//}

	//	if (FAILED(Remove_Component(strComponentTag)))
	//	{
	//		return E_FAIL;
	//	}

	//	shared_ptr<CComponent> CloneComp = dynamic_pointer_cast<CComponent>(
	//		m_pGameInstance.lock()->Clone_Prototype(PROTOTYPE::COMPONENT, iProtoLevelIndex, strProtoTag, pArg));

	//	if (CloneComp == nullptr)
	//		return E_FAIL;

	//	shared_ptr<T> pCastComp = dynamic_pointer_cast<T>(CloneComp);
	//	if (pCastComp == nullptr)
	//	{
	//		MSG_BOX("Failed Add Component : Wrong Type(it's should Component's Child)");
	//		return E_FAIL;
	//	}

	//
	//		pOut = pCastComp;
	//	
	//	m_Components.emplace(strComponentTag, pCastComp);

	//	return S_OK;

	//}
protected:
	_bool m_bIsDirtyCom = {true};// 처음은 해야하니까

    map<const _wstring, class shared_ptr<CComponent> > m_Components;

public:
    virtual void Free()override;
};

NS_END;