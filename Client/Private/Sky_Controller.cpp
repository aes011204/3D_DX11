#include "Sky_Controller.h"


#include <numbers>

#include "ItemInfo.h"
#include "RotationModifier.h"
#include "UI_HUD.h"

#include "UI_MainMenu.h"
#include "UI_TabContainer.h"
#include "UI_Item.h"
#include "UIText.h"
#include "UI_NPC.h"
#include "UI_MiniGame.h"
#include "UI_Village.h"
#include "EventBus.h"
#include "DialogueDB.h"
#include "Light_Manager.h"

IMPLEMENT_SINGLETON(CSky_Controller)

CSky_Controller::CSky_Controller()

{
}

CSky_Controller::~CSky_Controller()
{
	Free();

}

HRESULT CSky_Controller::Initialize(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	m_gameInstance = CGameInstance::GetInstance();
	m_device = device;
	m_context = context;


	return S_OK;
}





void CSky_Controller::Update(float timeDelta)
{
	auto gameInstance = m_gameInstance.lock();

	// 빛
	float t = gameInstance->Get_TOD01();
	//auto lightDESC = pGameInstance->Get_LightDesc(0);

	float offset = XMConvertToRadians(90);
	float angle = t * XM_2PI - offset;

	_float4 dir;
	dir.x = cos(angle);
	dir.y = -sin(angle);
	dir.z = sin(angle );
	dir.w = 0.f;
	XMStoreFloat4( &m_direction , XMVector4Normalize(XMLoadFloat4(&dir)));


	//
	_float3 skyColor = m_nightColor;

	auto smooth = [](float a, float b, float t)
		{
			float w = (t - a) / (b - a);
			w = max(0.f, min(1.f, w));
			return w * w * (3.f - 2.f * w);
		};

	// 밤 → 새벽
	skyColor = Lerp3(skyColor, m_midNightColor, smooth(m_nightRange.x, m_nightRange.y, t));

	// 새벽 → 낮
	skyColor = Lerp3(skyColor, m_dayColor, smooth(m_midNightRange.x, m_midNightRange.y, t));

	// 낮 → 노을
	skyColor = Lerp3(skyColor, m_sunsetColor, smooth(m_dayRange.x, m_dayRange.y, t));

	// 노을 → 밤
	skyColor = Lerp3(skyColor, m_afterSunsetColor, smooth(m_sunsetRange.x, m_sunsetRange.y, t));
	//
	skyColor = Lerp3(skyColor, m_nightColor, smooth(m_nightRange2.x, m_nightRange2.y, t));

	m_skyColor = skyColor ;

	_float3 sunColor = Lerp3(_float3(1, 1, 1), skyColor, 0.5f);
	//_float3 ambient = skyColor * 0.25f;

	//m_Ambient = _float4{ ambient.x,ambient.y,ambient.z,1.f };
	m_diffuse = _float4{ sunColor.x,sunColor.y,sunColor.z,1.f };

	LightDesc light = {};
	light.direction = m_direction;
	light.diffuse = m_diffuse;
	light.ambient = m_ambient;
	light.specular = m_specular;
	gameInstance->Set_LightDesc(0, light);


	gameInstance->Set_SkyColor(m_skyColor);

}


void CSky_Controller::OnGui()
{
	ImGui::ColorEdit3("Diffuse", (float*)&m_diffuse);
	ImGui::ColorEdit3("Ambient", (float*)&m_ambient);
	ImGui::ColorEdit3("Specular", (float*)&m_specular);

	ImGui::DragFloat3("Direction", (float*)&m_direction, 0.01f, -1.f, 1.f);
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat4(&m_direction));
	XMStoreFloat4(&m_direction, dir);
}

void CSky_Controller::Free()
{
	CBase::Free();
}
