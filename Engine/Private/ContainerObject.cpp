#include "ContainerObject.h"
#include "PartObject.h"

CContainerObject::CContainerObject(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: CGameObject{ device, context }
{
}

CContainerObject::CContainerObject(const CContainerObject& prototype)
	:CGameObject{ prototype }
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContainerObject::Initialize(void* arg)
{
	if(FAILED(__super::Initialize(arg)))
		return E_FAIL;

	return S_OK;
}

void CContainerObject::Priority_Update(_float timeDelta)
{
	for (auto& Pair : m_partObjects)
		Pair.second->Priority_Update(timeDelta);
}

void CContainerObject::Update(_float timeDelta)
{
	for (auto& Pair : m_partObjects)
		Pair.second->Update(timeDelta);
}

void CContainerObject::Late_Update(_float timeDelta)
{
	for (auto& Pair : m_partObjects)
		Pair.second->Late_Update(timeDelta);
}

HRESULT CContainerObject::Render()
{
	for (auto& Pair : m_partObjects)
		Pair.second->Render();

	return S_OK;
}

HRESULT CContainerObject::Add_PartObject(_uint prototypeLevelIndex, const _wstring& prototypeTag,
	const _wstring& partTag, void* arg)
{

	if (nullptr != Find_PartObject(partTag))
		return E_FAIL;

	auto partObject = dynamic_pointer_cast<CPartObject>(m_gameInstance.lock()->Clone_Prototype(EPrototype::GAMEOBJECT, prototypeLevelIndex, prototypeTag, arg));
	if (nullptr == partObject)
		return E_FAIL;

	m_partObjects.emplace(partTag, partObject);

	return S_OK;
}

shared_ptr<CPartObject> CContainerObject::Find_PartObject(const _wstring& partTag)
{
	auto	iter = m_partObjects.find(partTag);

	if (iter == m_partObjects.end())
		return nullptr;

	return iter->second;
}



void CContainerObject::Free()
{
	__super::Free();
}
