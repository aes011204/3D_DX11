#include "Bounding_Sphere.h"

#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "DebugDraw.h"
CBounding_Sphere::CBounding_Sphere(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	:CBounding(device, context)
{
}

HRESULT CBounding_Sphere::Initialize(BoundingDesc* desc)
{
	auto boundingDesc = static_cast<BoundingSphereDesc*>(desc);
	m_originalDesc = make_shared< BoundingSphere>(boundingDesc->center, boundingDesc->radius);
	m_desc = make_shared< BoundingSphere>(*m_originalDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix worldMatrix)
{

	m_originalDesc->Transform(*m_desc, worldMatrix);

}

_bool CBounding_Sphere::Intersect(shared_ptr<CBounding>  target)
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
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* batch, _fvector color)
{

	DX::Draw(batch, *m_desc, color);
	return S_OK;

}
#endif

shared_ptr<CBounding_Sphere> CBounding_Sphere::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context,
	BoundingDesc* desc)
{
	shared_ptr<CBounding_Sphere> instance(new CBounding_Sphere(device, context), [](CBounding_Sphere* p) {p->Free(); delete p; });

	if (FAILED(instance->Initialize(desc)))
	{
		MSG_BOX("Failed to Created : CBounding_Sphere");
	}
	return instance;
}

void CBounding_Sphere::Free()
{
	CBounding::Free();
}
