
#include"GameView.h"
#include"GameInstance.h"
#include"Camera.h"
#include"Engine_Helper.h"

CGameView::CGameView()
	: CEditor_Win("Game View")
{
}

CGameView::~CGameView()
{
	Free();
}


bool CGameView::CreateRT(UINT width, UINT height)
{

	if (!m_device || width == 0 || height == 0) return false;

	m_texture.Reset();
	m_SRV.Reset();

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 너 swapchain 포맷과 동일
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_device->CreateTexture2D(&desc, nullptr, m_texture.ReleaseAndGetAddressOf())))
		return false;

	if (FAILED(m_device->CreateShaderResourceView(m_texture.Get(), nullptr, m_SRV.ReleaseAndGetAddressOf())))
		return false;

	m_width = width;
	m_height = height;



	return true;
}

HRESULT CGameView::Initialize(const EngineDesc& engineDesc, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	m_device = pDevice;
	m_context = pContext;

	m_width = engineDesc.viewportWidth;
	m_height = engineDesc.viewportHeight;

	CreateRT(engineDesc.viewportWidth, engineDesc.viewportHeight);



	return S_OK;
}

bool CGameView::Resize(UINT width, UINT height)
{
	if (width < 16) width = 16;
	if (height < 16) height = 16;

	if (m_texture && width == m_width && height == m_height)
		return true;

	return CreateRT(width, height);
}

void CGameView::CaptureFromBackBuffer(ID3D11RenderTargetView* backBufferRTV)
{
	if (!m_context || !backBufferRTV || !m_texture) return;

	ComPtr<ID3D11Resource> srcRes;
	backBufferRTV->GetResource(srcRes.GetAddressOf());
	if (!srcRes) return;

	ComPtr<ID3D11Texture2D> srcTex;
	srcRes.As(&srcTex);
	if (!srcTex) return;

	D3D11_TEXTURE2D_DESC srcDesc{};
	srcTex->GetDesc(&srcDesc);

	UINT copyW = min(m_width, srcDesc.Width);
	UINT copyH = min(m_height, srcDesc.Height);

	D3D11_BOX box{};
	box.left = 0; box.top = 0; box.front = 0;
	box.right = copyW;
	box.bottom = copyH;
	box.back = 1;

	m_context->CopySubresourceRegion(
		m_texture.Get(), 0,
		0, 0, 0,
		srcTex.Get(), 0,
		&box
	);
}

void CGameView::Render()
{

	ImGui::Begin("Game View");

	//Render_CameraToolbar();

	{// 마우스 위치 
		ImVec2 vPanelSize = ImGui::GetContentRegionAvail();
		if (vPanelSize.x > 0 && vPanelSize.y > 0)
		{
			// 비율 및 크기 계산
			float fTargetRatio = (float)m_width / (float)m_height;
			float fPanelRatio = vPanelSize.x / vPanelSize.y;

			if (fPanelRatio > fTargetRatio) {
				m_renderSize.y = vPanelSize.y;
				m_renderSize.x = vPanelSize.y * fTargetRatio;
			}
			else {
				m_renderSize.x = vPanelSize.x;
				m_renderSize.y = vPanelSize.x / fTargetRatio;
			}

			// 중앙 정렬 및 시작점 저장
			float fOffsetX = (vPanelSize.x - m_renderSize.x) * 0.5f;
			float fOffsetY = (vPanelSize.y - m_renderSize.y) * 0.5f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fOffsetX);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + fOffsetY);

			// 중요: Update에서 쓸 시작점(0,0) 좌표 저장
			m_contentPos = ImGui::GetCursorScreenPos();

			//출력 및 상태 체크
			if (m_SRV)
			{
				ImGui::Image((void*)m_SRV.Get(), m_renderSize);
				// 마우스가 실제 그림 영역 안에 있는지 체크
				m_isMouseInScene = ImGui::IsItemHovered();

				if (m_isMouseInScene)
				{
					//이미 구해놓은 m_vContentPos(그림 시작점)를 빼서 상대 좌표 구함
					float fLocalX = ImGui::GetMousePos().x - m_contentPos.x;
					float fLocalY = ImGui::GetMousePos().y - m_contentPos.y;

					// 이미 구해놓은 m_vRenderSize(실제 출력 크기)로 비율 계산 후 가상 해상도 곱함
					float fCalibratedX = (fLocalX / m_renderSize.x) * (float)m_width;
					float fCalibratedY = (fLocalY / m_renderSize.y) * (float)m_height;

					//  화면에 출력
                    //ImGui::SetCursorScreenPos(ImVec2(m_vContentPos.x + 10.f, m_vContentPos.y + 10.f));
                    //ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "X: %.1f, Y: %.1f", fCalibratedX, fCalibratedY);
				}

			}
			else
			{
				ImGui::TextDisabled("(No GameView SRV)");
			}
		}
	}



	ImGui::End();
}

void CGameView::Update(float fTimeDelta)
{
	// 패널 정보 미리 파악 (Begin/End 사이에서만 유효한 값들이라 여기서 체크)

	if (m_isMouseInScene)
	{
		ImVec2 vMousePos = ImGui::GetMousePos();

		// 그림 영역 내의 상대 좌표 계산
		float fLocalX = vMousePos.x - m_contentPos.x;
		float fLocalY = vMousePos.y - m_contentPos.y;

		// 0.0 ~ 1.0 비율 계산 ,매핑
		float fCalibratedX = (fLocalX / m_renderSize.x) * m_width;
		float fCalibratedY = (fLocalY / m_renderSize.y) * m_height;

		float fFixedY = (float)m_height - fCalibratedY; //카르트 좌표계에 맞게 보정

		CGameInstance::GetInstance()->Set_MousePos(fCalibratedX, fFixedY);
	}
	else
		CGameInstance::GetInstance()->Set_MousePos(-10.f, -10.f);
}

unique_ptr<CGameView>  CGameView::Create(const EngineDesc& engineDesc,
	ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CGameView> pInstance(new CGameView()/*, [](CGameView* p) {p->Free();delete p;}*/);
	if (FAILED(pInstance->Initialize(engineDesc, pDevice, pContext)))
		return nullptr;

	//CreateRT(EngineDesc.iViewportWidth, EngineDesc.iViewportHeight);
	return pInstance;
}

void CGameView::Render_CameraToolbar()
{
//	ImGui::BeginGroup(); // 관련 요소들 묶기
//
//	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " [CAM] ");
//	ImGui::SameLine();
//	CGameInstance::GetInstance()->CAM_Manger_OnGui();
//
//ImGui::EndGroup();
//ImGui::NewLine(); // 툴바 끝났으니 다음 줄로
}

void CGameView::Free()
{
	m_device.Reset();
	m_context.Reset();

	m_texture.Reset();
	m_SRV.Reset();
}
