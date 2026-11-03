#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"
CBounding_AABB::CBounding_AABB(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CBounding(device, context)
{
}

HRESULT CBounding_AABB::Initialize(BoundingDesc* desc)
{
	auto boundingDesc = static_cast<BoundingAABBDesc*>(desc);
	m_originalDesc = make_shared< BoundingBox>(boundingDesc->center, boundingDesc->extents);
	m_desc = make_shared< BoundingBox>(*m_originalDesc);

	return S_OK;
}

void CBounding_AABB::Update(_fmatrix worldMatrix)
{
	_matrix transformMatrix = worldMatrix;

	transformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(worldMatrix.r[0]);
	transformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(worldMatrix.r[1]);
	transformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(worldMatrix.r[2]);

	m_originalDesc->Transform(*m_desc, transformMatrix);

}

_bool CBounding_AABB::Intersect(shared_ptr<CBounding>  target)
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
		auto OBB = dynamic_pointer_cast<CBounding_OBB>(target);
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
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color)
{

	DX::Draw(batch, *m_desc, color);
	return S_OK;

}
#endif

shared_ptr<CBounding_AABB> CBounding_AABB::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
	BoundingDesc* desc)
{
	shared_ptr<CBounding_AABB> instance(new CBounding_AABB(device, context), [](CBounding_AABB* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(desc)))
	{
		MSG_BOX("Failed to Created : CBounding_AABB");
	}
	return instance;
}

void CBounding_AABB::Free()
{
	CBounding::Free();
}
