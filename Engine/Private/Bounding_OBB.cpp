#include "Bounding_OBB.h"
#include "Bounding_AABB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"
CBounding_OBB::CBounding_OBB(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CBounding(device, context)
{
}

HRESULT CBounding_OBB::Initialize(BoundingDesc* desc)
{
	auto boundingDesc = static_cast<BoundingOBBDesc*>(desc);

	_float4 rotation = {};

	XMStoreFloat4(&rotation,
		XMQuaternionRotationRollPitchYaw(boundingDesc->radians.x, boundingDesc->radians.y, boundingDesc->radians.z));

	m_originalDesc = make_shared< BoundingOrientedBox>(boundingDesc->center, boundingDesc->extents, rotation);
	m_desc = make_shared< BoundingOrientedBox>(*m_originalDesc);



	return S_OK;
}

void CBounding_OBB::Update(_fmatrix worldMatrix)
{
	
	m_originalDesc->Transform(*m_desc, worldMatrix);

}

_bool CBounding_OBB::Intersect(shared_ptr<CBounding>  target)
{
	_bool isCollision = { false };
	const _char* name = typeid(*target).name();

	if(false ==strcmp("class Engine::CBounding_AABB", name))
	{
		auto AABB = dynamic_pointer_cast<CBounding_AABB>(target);
		isCollision = m_desc->Intersects(*AABB->Get_Desc());
	}
	else if(false == strcmp("class Engine::CBounding_OBB", name))
	{
		auto OBB = (dynamic_pointer_cast<CBounding_OBB>(target));
		isCollision = m_desc->Intersects(*OBB->Get_Desc());
	}
	else
	{
		auto sphere = dynamic_pointer_cast<CBounding_Sphere>(target);
		isCollision = m_desc->Intersects(*sphere->Get_Desc());

	}
	return isCollision;
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color)
{

	DX::Draw(batch, *m_desc, color);
	return S_OK;

}
#endif

shared_ptr<CBounding_OBB> CBounding_OBB::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
	BoundingDesc* desc)
{
	shared_ptr<CBounding_OBB> instance(new CBounding_OBB(device, context), [](CBounding_OBB* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(desc)))
	{
		MSG_BOX("Failed to Created : CBounding_OBB");
	}
	return instance;
}

void CBounding_OBB::Free()
{
	CBounding::Free();
}
