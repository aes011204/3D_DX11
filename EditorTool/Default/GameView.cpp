
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

	if (!m_pDevice || width == 0 || height == 0) return false;

	m_pTexture.Reset();
	m_pSRV.Reset();

	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 너 swapchain 포맷과 동일
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pTexture.ReleaseAndGetAddressOf())))
		return false;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture.Get(), nullptr, m_pSRV.ReleaseAndGetAddressOf())))
		return false;

	m_Width = width;
	m_Height = height;



	return true;
}

HRESULT CGameView::Initialize(const ENGINE_DESC& EngineDesc, ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_Width = EngineDesc.iViewportWidth;
	m_Height = EngineDesc.iViewportHeight;

	CreateRT(EngineDesc.iViewportWidth, EngineDesc.iViewportHeight);



	return S_OK;
}

bool CGameView::Resize(UINT width, UINT height)
{
	if (width < 16) width = 16;
	if (height < 16) height = 16;

	if (m_pTexture && width == m_Width && height == m_Height)
		return true;

	return CreateRT(width, height);
}

void CGameView::CaptureFromBackBuffer(ID3D11RenderTargetView* backBufferRTV)
{
	if (!m_pContext || !backBufferRTV || !m_pTexture) return;

	ComPtr<ID3D11Resource> srcRes;
	backBufferRTV->GetResource(srcRes.GetAddressOf());
	if (!srcRes) return;

	ComPtr<ID3D11Texture2D> srcTex;
	srcRes.As(&srcTex);
	if (!srcTex) return;

	D3D11_TEXTURE2D_DESC srcDesc{};
	srcTex->GetDesc(&srcDesc);

	UINT copyW = min(m_Width, srcDesc.Width);
	UINT copyH = min(m_Height, srcDesc.Height);

	D3D11_BOX box{};
	box.left = 0; box.top = 0; box.front = 0;
	box.right = copyW;
	box.bottom = copyH;
	box.back = 1;

	m_pContext->CopySubresourceRegion(
		m_pTexture.Get(), 0,
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
			float fTargetRatio = (float)m_Width / (float)m_Height;
			float fPanelRatio = vPanelSize.x / vPanelSize.y;

			if (fPanelRatio > fTargetRatio) {
				m_vRenderSize.y = vPanelSize.y;
				m_vRenderSize.x = vPanelSize.y * fTargetRatio;
			}
			else {
				m_vRenderSize.x = vPanelSize.x;
				m_vRenderSize.y = vPanelSize.x / fTargetRatio;
			}

			// 중앙 정렬 및 시작점 저장
			float fOffsetX = (vPanelSize.x - m_vRenderSize.x) * 0.5f;
			float fOffsetY = (vPanelSize.y - m_vRenderSize.y) * 0.5f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + fOffsetX);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + fOffsetY);

			// 중요: Update에서 쓸 시작점(0,0) 좌표 저장
			m_vContentPos = ImGui::GetCursorScreenPos();

			//출력 및 상태 체크
			if (m_pSRV)
			{
				ImGui::Image((void*)m_pSRV.Get(), m_vRenderSize);
				// 마우스가 실제 그림 영역 안에 있는지 체크
				m_bMouseInScene = ImGui::IsItemHovered();

				if (m_bMouseInScene)
				{
					//이미 구해놓은 m_vContentPos(그림 시작점)를 빼서 상대 좌표 구함
					float fLocalX = ImGui::GetMousePos().x - m_vContentPos.x;
					float fLocalY = ImGui::GetMousePos().y - m_vContentPos.y;

					// 이미 구해놓은 m_vRenderSize(실제 출력 크기)로 비율 계산 후 가상 해상도 곱함
					float fCalibratedX = (fLocalX / m_vRenderSize.x) * (float)m_Width;
					float fCalibratedY = (fLocalY / m_vRenderSize.y) * (float)m_Height;

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

	if (m_bMouseInScene)
	{
		ImVec2 vMousePos = ImGui::GetMousePos();

		// 그림 영역 내의 상대 좌표 계산
		float fLocalX = vMousePos.x - m_vContentPos.x;
		float fLocalY = vMousePos.y - m_vContentPos.y;

		// 0.0 ~ 1.0 비율 계산 ,매핑
		float fCalibratedX = (fLocalX / m_vRenderSize.x) * m_Width;
		float fCalibratedY = (fLocalY / m_vRenderSize.y) * m_Height;

		float fFixedY = (float)m_Height - fCalibratedY; //카르트 좌표계에 맞게 보정

		CGameInstance::GetInstance()->Set_MousePos(fCalibratedX, fFixedY);
	}
	else
		CGameInstance::GetInstance()->Set_MousePos(-10.f, -10.f);
}

unique_ptr<CGameView>  CGameView::Create(const ENGINE_DESC& EngineDesc,
	ComPtr<ID3D11Device> pDevice,
	ComPtr<ID3D11DeviceContext> pContext)
{
	unique_ptr<CGameView> pInstance(new CGameView()/*, [](CGameView* p) {p->Free();delete p;}*/);
	if (FAILED(pInstance->Initialize(EngineDesc, pDevice, pContext)))
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
	m_pDevice.Reset();
	m_pContext.Reset();

	m_pTexture.Reset();
	m_pSRV.Reset();
}
