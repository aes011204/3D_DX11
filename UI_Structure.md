# 3D_DX11 UI 구조 정리

이 문서는 현재 프로젝트의 UI 엔진 구조와 클라이언트 사용 흐름을 정리한 문서이다. 핵심은 `CUI` 계층, `CUI_Manager`의 Pool/Layer 관리, 그리고 모든 UI 배치의 기준이 되는 `CUITransform`이다.

## 1. 전체 흐름

UI는 일반 게임 오브젝트와 분리되어 `CUI_Manager`가 관리한다.

```text
CGameInstance
  └─ CUI_Manager
       ├─ UI Pool: 이름별 UI 원본 보관
       └─ UI Layer: 현재 레벨에 떠 있는 UI 목록
            ├─ OVERRIDE
            ├─ WINDOW
            ├─ STACK
            └─ HUD
```

엔진 루프 기준 흐름은 다음과 같다.

```text
CGameInstance::Update_Engine()
  1. Input 갱신
  2. Object Update
  3. UI_Manager::Update()
  4. Object Late_Update
  5. UI_Manager::Late_Update()

CGameInstance::Draw()
  1. Renderer::Draw()
  2. Level Render
  3. UI_Manager::Render()
```

실제 UI 렌더 등록은 `CUI_Manager::Late_Update()`에서 `RENDERGROUP::UI`로 넘긴다.

## 2. 엔진 UI 클래스 구조

### CUI

위치: `Engine/Public/UI.h`, `Engine/Private/UI.cpp`

`CUI`는 새 UI 시스템의 베이스 클래스이다. `CEntity`를 상속하고 내부에 `CUITransform` 컴포넌트를 가진다.

주요 역할:

- `CUITransform` 생성 및 컴포넌트 등록
- 자식 UI 관리
- UI 상태 전환
- `Update`, `Late_Update`, `Render` 템플릿 메서드 제공
- 마우스 hover 판정
- ZOrder 정렬
- JSON 저장/로드

주요 데이터:

```cpp
vector<shared_ptr<CUI>> m_Children;
weak_ptr<CUI> m_Parent;
shared_ptr<CUITransform> m_pUITransformCom;
map<_wstring, weak_ptr<CUI>> m_mapChildren;

int m_ZOrder = 1;
UI_STATE m_UIState = UI_STATE::END;

bool m_bEnabled = true;
bool m_bVisible = true;
bool m_bInteractable = true;
```

`CUI::Initialize()`는 `CUITransform`을 만들고 `OnInit()`을 호출한다. 클라이언트 UI는 보통 `OnInit`, `OnActive`, `OnUpdate`, `OnRender` 같은 훅을 override해서 동작을 넣는다.

### CUIRenderable

위치: `Engine/Public/UIRenderable.h`, `Engine/Private/UIRenderable.cpp`

`CUIRenderable`은 실제 화면에 그릴 수 있는 UI 베이스이다. `CUI`를 상속하며 텍스처, 셰이더, 사각형 VIBuffer를 준비한다.

주요 기능:

- `Prototype_Component_VIBuffer_Rect` 사용
- 기본 UI 셰이더 `Prototype_Component_Shader_VtxTex` 사용
- Radial 타입이면 `Prototype_Component_Shader_VtxDonut` 사용
- 텍스처 컴포넌트 바인딩
- Alpha, Dark, ColorMix, ColorFlat 바인딩
- NineSlice 렌더 패스 지원

렌더 흐름:

```text
CUIRenderable::OnRender()
  1. 투명 UI면 return
  2. World/View/Proj/Texture/색상 파라미터 바인딩
  3. Shader Begin
  4. VIBuffer Bind
  5. VIBuffer Render
```

텍스처를 가진 UI는 초기화 시 텍스처 원본 크기를 기준으로 `CUITransform::SetSizeDelta()`를 호출한다.

```cpp
m_pUITransformCom->SetSizeDelta(
    (m_pTextureCom->Get_SizeFromSRV(m_TexIndex) / 3.f) * 2.f
);
```

따라서 클라이언트에서 `vSizeDelta`를 직접 주지 않아도 텍스처 기반 UI는 기본 크기가 잡힌다.

### CUIPanel

위치: `Engine/Public/UIPanel.h`, `Engine/Private/UIPanel.cpp`

`CUIPanel`은 배경 이미지와 레이아웃 기능을 가진 UI 컨테이너이다.

주요 옵션:

```cpp
struct UIPANEL_DESC : public CUIRenderable::RENDERABLE_DESC
{
    _bool IsFullScreen = false;
    _bool IsUseLayout = false;
    LAYOUT_DESC LayoutDesc;
};
```

`IsFullScreen == true`이면 현재 윈도우 크기를 가져와서 transform을 화면 중앙 기준 전체 크기로 세팅한다.

```cpp
m_pUITransformCom->SetAnchorPoint(Vector2(0.5f, 0.5f));
m_pUITransformCom->SetPivot(Vector2(0.5f, 0.5f));
m_pUITransformCom->SetSizeDelta(Vector2(m_CanvasSize.w, m_CanvasSize.h));
m_pUITransformCom->SetAnchoredPos(Vector2(0.f, 0.f));
m_pUITransformCom->SetLocalScale(Vector2(1.f, 1.f));
```

`IsUseLayout`이 켜진 패널은 `OnActive()`에서 `Layout()`을 호출한다. `Add_Layout_Child()`로 추가된 자식만 배치 대상이 된다.

레이아웃 방식:

- 자식의 최종 크기 `Get_FinalSize()`를 슬롯 크기로 사용
- `m_Col`, `m_Row`, `m_Spacing`, `m_Offset`으로 전체 영역 계산
- 패널 중심 기준으로 첫 슬롯 위치 계산
- 각 자식의 `AnchoredPos` 갱신

### 기본 UI 부품

- `CUIImage`: 단순 이미지 UI
- `CUIButton`: 클릭/hover 이벤트를 가진 버튼
- `CUISlot`: 인벤토리/창고 슬롯용 UI
- `CUIText`: 폰트 매니저 기반 텍스트 UI
- `CUIProgress`: 진행 바 UI

클라이언트에서는 이 기본 부품을 조립해서 `CUI_TabContainer`, `CUI_Inventory`, `CUI_HUD`, `CUI_NPC` 같은 화면 단위 UI를 만든다.

## 3. CUITransform 상세

위치: `Engine/Public/UITransform.h`, `Engine/Private/UITransform.cpp`

`CUITransform`은 UI 전용 transform 컴포넌트이다. 3D 월드 transform과 달리 부모 UI 또는 화면 크기 기준으로 2D rect와 렌더 행렬을 계산한다.

### Transform DESC

```cpp
struct UITRANSFORM_DESC
{
    Vector2 vAnchorPoint = { 0.5f, 0.5f };
    Vector2 vPivot       = { 0.5f, 0.5f };
    Vector2 vSizeDelta   = { 100.f, 100.f };
    Vector2 vAnchoredPos = { 0.f, 0.f };
    Vector2 vScale       = { 1.f, 1.f };

    _bool bSetParentSize = false;
};
```

각 값의 의미:

- `vAnchorPoint`: 부모 rect 안에서 기준점을 잡는 비율이다. `(0,0)`은 부모 좌상단, `(0.5,0.5)`는 중앙, `(1,1)`은 우하단이다.
- `vPivot`: 자기 rect 내부의 기준점이다. `(0,0)`은 자기 좌상단, `(0.5,0.5)`는 중앙, `(1,1)`은 우하단이다.
- `vSizeDelta`: UI의 기준 크기이다. 단위는 픽셀이다.
- `vAnchoredPos`: 부모 anchor 위치에서 자기 pivot 위치까지의 오프셋이다.
- `vScale`: `vSizeDelta`에 곱해지는 UI 배율이다.
- `bSetParentSize`: 부모의 size/scale을 따라가야 하는 자식 패널에서 사용한다.

### 부모 기준

부모 transform이 있으면 부모의 `WorldRect`를 기준으로 배치한다.

```cpp
if (m_Parent.lock())
    return m_Parent.lock()->Get_WorldRect();
else
    return m_pGameInstance.lock()->Get_WinSize();
```

부모가 없으면 화면 전체 `Get_WinSize()`가 부모 rect가 된다. 즉 루트 UI는 화면 기준으로 배치된다.

### WorldRect 계산

핵심 계산은 `Computing_WorldRect()`에 있다.

```cpp
Rect parentRect = GetParent_WorldRect();
Vector2 parentPos = parentRect.Pos();
Vector2 parentSize = parentRect.Size();

Vector2 anchorPoint = parentPos + Hadamard(parentSize, m_AnchorPoint);
Vector2 size = Hadamard(m_SizeDelta, m_LocalScale);
Vector2 pivotWorld = anchorPoint + m_AnchoredPos;
Vector2 topLeft = pivotWorld - Hadamard(m_Pivot, size);

m_WorldRect = {
    topLeft.x,
    topLeft.y,
    topLeft.x + size.x,
    topLeft.y + size.y
};
```

정리하면:

```text
anchorPoint = parentPos + parentSize * AnchorPoint
size        = SizeDelta * LocalScale
pivotWorld  = anchorPoint + AnchoredPos
topLeft     = pivotWorld - Pivot * size
worldRect   = [topLeft, topLeft + size]
```

예시:

```cpp
vAnchorPoint = { 1.f, 0.f };
vPivot       = { 1.f, 0.f };
vAnchoredPos = { -40.f, 40.f };
```

부모 우상단을 anchor로 잡고, 자기 우상단 pivot을 그 anchor에서 왼쪽 40px, 아래 40px 위치에 놓는다는 뜻이다. 현재 좌표계에서는 `y`가 아래 방향으로 증가하는 방식으로 쓰이고 있다.

### WorldMatrix 계산

`VIBuffer_Rect`가 중심 기준 `-0.5 ~ 0.5` 사각형이라는 전제로 행렬을 만든다.

```cpp
Vector2 pivotOffset = {
    (0.5f - m_Pivot.x) * size.x,
    (0.5f - m_Pivot.y) * size.y
};

XMMATRIX S = XMMatrixScaling(size.x, size.y, 1.f);
XMMATRIX matPivot = XMMatrixTranslation(pivotOffset.x, pivotOffset.y, 0.f);
XMMATRIX R = XMMatrixRotationZ(m_RotationRadian);
XMMATRIX T = XMMatrixTranslation(pivotWorld.x, pivotWorld.y, 0.f);

WorldMatrix = S * matPivot * R * T;
```

의미:

1. Rect 버퍼를 UI 크기로 스케일한다.
2. pivot이 중앙이 아닐 경우 `pivotOffset`으로 보정한다.
3. pivot 기준으로 회전한다.
4. 최종 pivot 위치인 `pivotWorld`로 이동한다.

주의할 점:

- `WorldRect`는 디버그/마우스 판정용 축 정렬 rect이다.
- 회전이 들어가도 `WorldRect`는 회전된 외곽선을 다시 계산하지 않는다.
- 따라서 회전 UI의 hover 판정은 실제 보이는 모양과 다를 수 있다.

### Dirty 갱신

setter는 모두 `MarkDirtyRecursive()`를 호출한다.

```cpp
void SetAnchorPoint(Vector2 v) { m_AnchorPoint = v; MarkDirtyRecursive(); }
void SetPivot(Vector2 v)       { m_Pivot = v; MarkDirtyRecursive(); }
void SetSizeDelta(Vector2 v)   { m_SizeDelta = v; MarkDirtyRecursive(); }
void SetAnchoredPos(Vector2 v) { m_AnchoredPos = v; MarkDirtyRecursive(); }
void SetLocalScale(Vector2 v)  { m_LocalScale = v; MarkDirtyRecursive(); }
```

부모가 움직이면 자식도 다시 계산되어야 하므로 dirty는 재귀적으로 전파된다. UI Update 중 `CUI::Update()`에서 `m_pUITransformCom->UpdateLayoutIfDirty()`가 호출되고, 필요한 경우에만 다시 계산한다.

### 부모 자식 연결

`CUI::Add_Child()`는 UI 계층과 transform 계층을 같이 연결한다.

```cpp
m_Children.push_back(child);
child->m_Parent = static_pointer_cast<CUI>(shared_from_this());
m_mapChildren.emplace(UITag, child);

child->GetUITransform()->SetParent(m_pUITransformCom, KeepWorldRect);
```

현재 `SetParent(..., keepWorldRect)`에서 `keepWorldRect` 보존 로직은 주석 처리되어 있다. 따라서 부모를 바꾸면 기존 월드 위치를 유지하기보다 새 부모 기준으로 다시 계산된다고 보면 된다.

## 4. UI 상태와 생명주기

`CUI`는 다음 상태/플래그로 활성 여부를 제어한다.

```text
UI_Active()
  UI_STATE::ACTIVE
  m_bEnabled = true
  m_bVisible = true
  OnActive()
  children UI_Active()

UI_InActive()
  UI_STATE::INACTIVE
  m_bEnabled = false
  m_bVisible = false
  OnInActive()
  children UI_InActive()

Set_UI_Disabled()
  UI_STATE::DISABLE
  m_bEnabled = true
  m_bVisible = true
  OnDisabled()
  children Set_UI_Disabled()
```

`m_bEnabled`는 Update 여부, `m_bVisible`은 Render 여부에 가깝다.

클라이언트 UI에서 자주 쓰는 override:

- `OnInit`: 자식 UI 생성, 이벤트 구독, 초기 텍스처/버튼 세팅
- `OnActive`: UI가 열릴 때 데이터 갱신, 애니메이션 시작
- `OnInActive`: 닫힐 때 상태 초기화
- `OnUpdate`: 매 프레임 애니메이션, 입력 기반 처리
- `OnLateUpdate`: 렌더 직전 상태 정리
- `OnRender`: 별도 렌더가 필요할 때 사용
- `OnClear`: 제거/정리 시 호출

## 5. CUI_Manager

위치: `Engine/Public/UI_Manager.h`, `Engine/Private/UI_Manager.cpp`

`CUI_Manager`는 두 가지 저장소를 가진다.

```cpp
unordered_map<wstring, shared_ptr<CUI>> m_UIPool;
vector<shared_ptr<CUI>> m_UI[ETOI(UI_LAYER::END)];
```

### Pool

`m_UIPool`은 생성된 UI를 이름으로 보관하는 저장소이다.

```cpp
m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
```

UI는 보통 클라이언트의 `CUI_Controller::Ready_UI()`에서 한 번 생성되고 pool에 등록된다.

### Layer

`m_UI[layer]`는 현재 레벨에서 떠 있는 UI 목록이다.

```cpp
m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"TabContainer", false, nullptr);
```

`Push()`는 pool에서 UI를 찾아 해당 레이어에 넣는다. 이미 같은 UI가 레이어에 있으면 기존 위치에서 제거한 뒤 맨 뒤로 다시 넣는다.

`isOnActive == false`이면 push하면서 `UI_InActive()` 상태로 넣는다. 즉 레이어에는 존재하지만 화면에는 보이지 않을 수 있다.

### Layer 종류

```cpp
enum class UI_LAYER
{
    OVERRIDE,
    WINDOW,
    STACK,
    HUD,
    END
};
```

프로젝트 사용 경향:

- `HUD`: 항상 떠 있는 HUD
- `WINDOW`: 인벤토리, 상점, NPC, 미니게임 같은 화면 단위 패널
- `OVERRIDE`: 툴팁, 드래그 중 아이템, 로딩처럼 위에 올라오는 UI
- `STACK`: 시간 팝업처럼 별도 stack 성격의 UI

### 마우스 입력 점유

`CUI_Manager::Update()`는 레이어 순회 중 `bool bMouseHold`를 공유한다.

```cpp
bool bMouseHold = false;

for (int i = 0; i < ETOI(UI_LAYER::END); i++)
{
    vector<shared_ptr<CUI>> vecCopy = m_UI[i];
    for (auto it = vecCopy.rbegin(); it != vecCopy.rend(); ++it)
        (*it)->Update(m_fDeltaTime, bMouseHold);
}
```

`CUI::Update()`에서 현재 UI의 `WorldRect` 안에 마우스가 있고 interactable이면 `bMouseHold = true`로 바꾼다. 그 뒤 UI들은 hover를 받지 못한다. 즉 위쪽 UI가 입력을 먼저 가져가는 구조이다.

## 6. 클라이언트에서 UI를 쓰는 방식

클라이언트의 중심은 `Client/Private/UI_Controller.cpp`이다.

### 1) UI 생성 후 Pool 등록

`CUI_Controller::Ready_UI()`에서 화면 단위 UI들을 만든다.

```cpp
CUI_MainMenu::MAINMENU_DESC pDescPanel;
pDescPanel.IsFullScreen = true;
pDescPanel.IsTransparent = true;

shared_ptr<CUI_MainMenu> pInstance = CUI_MainMenu::Create(m_pDevice, m_pContext);
pInstance->Initialize(&pDescPanel);
m_pGameInstance.lock()->UI_InsertToPool(L"MainMenu", pInstance);
```

기본 패턴:

```text
DESC 작성
Create()
Initialize(&DESC)
UI_InsertToPool(key, ui)
```

### 2) 레벨에서 Push

레벨 진입 시 pool에 있는 UI를 필요한 레이어에 올린다.

```cpp
m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"TabContainer", false, nullptr);
m_pGameInstance.lock()->UI_Push(UI_LAYER::WINDOW, L"repairShop", false, nullptr);
m_pGameInstance.lock()->UI_Push(UI_LAYER::HUD, L"HUD", true, nullptr);
m_pGameInstance.lock()->UI_Push(UI_LAYER::OVERRIDE, L"ToolTip", false, nullptr);
m_pGameInstance.lock()->UI_Push(UI_LAYER::OVERRIDE, L"HoldItem", false, nullptr);
```

`false`로 push된 UI는 레이어에 들어가지만 비활성 상태이다. 이후 상태 전환에서 `UI_Active()` 또는 커스텀 활성 함수로 켠다.

### 3) 상태에 따라 UI 활성화

`CUI_Controller`는 이벤트를 구독하고 플레이어 상태에 맞춰 UI를 켜고 끈다.

```cpp
CGameInstance::GetInstance()->Get_EventBus()->Subscribe<Evt_Cam_Arrived>(
    [this](const Evt_Cam_Arrived e)
    {
        m_PendingUIState = e.playerstate;
        StateUI();
    }
);
```

예: 낚시 상태

```cpp
auto Tab = m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::WINDOW, L"TabContainer");
dynamic_pointer_cast<CUI_TabContainer>(Tab)->UI_PanelActive(ETOI(TAB::INVEN), TAB::INVEN);

auto HoldItem = dynamic_pointer_cast<CUI_Item>(
    m_pGameInstance.lock()->Find_UI_InCurLevel(UI_LAYER::OVERRIDE, L"HoldItem")
);
HoldItem->UI_Active();
```

화면 단위 UI가 단순히 켜지는 것 이상이 필요하면 `UI_PanelActive()` 같은 클라이언트 전용 함수를 만든다.

### 4) UI 내부 조립

클라이언트 화면 UI는 대부분 `CUIPanel`을 상속하고 `OnInit()`에서 부품을 만든다.

```cpp
CUIButton::UIBUTTON_DESC ButDesc = {};
ButDesc.TextureComLevel = ETOI(LEVEL::STATIC);
ButDesc.TextureProtoName = L"Prototype_Component_Texture_Button_RED";
ButDesc.TypeIndex = ETOI(eTab);
ButDesc.ClickEvent = [this](CUIButton* pThis)
{
    SetActiveTab(static_cast<TAB>(pThis->Get_TypeIndex()));
    m_pGameInstance.lock()->Play_Once(L"Click");
};

shared_ptr<CUIButton> button = CUIButton::Create(m_pDevice, m_pContext);
button->Initialize(&ButDesc);
Add_Layout_Child(button, NameTag, false);
```

자식 추가 기준:

- `Add_Child(child, tag, false)`: 일반 자식 추가
- `Add_Layout_Child(child, tag, false)`: 패널 레이아웃 대상 자식 추가
- `Find_Children(tag)`: 자식 검색

### 5) Transform 세팅 예시

로딩 아이콘:

```cpp
CUIImage::UIIMAGE_DESC LoadingIconDesc;
LoadingIconDesc.vPivot = _float2{ 1.f, 0.f };
LoadingIconDesc.vAnchorPoint = _float2{ 1.f, 0.f };
LoadingIconDesc.vAnchoredPos = _float2{ -40.f, 40.f };
LoadingIconDesc.TextureComLevel = ETOI(LEVEL::STATIC);
LoadingIconDesc.TextureProtoName = L"Prototype_Component_Texture_LoadingIcon";

shared_ptr<CUIImage> pIcon = CUIImage::Create(m_pDevice, m_pContext);
pIcon->Initialize(&LoadingIconDesc);
pIcon->m_behavior.push_back(make_shared<CRotationModifier>(200.f));
pInstance->Add_Child(pIcon, L"ICON_LOADING", false);
```

해석:

- 부모 기준 우상단에 anchor를 둔다.
- 자기 우상단을 pivot으로 쓴다.
- anchor에서 왼쪽 40px, 아래 40px 위치에 배치한다.
- `CRotationModifier`를 behavior에 넣어 매 프레임 회전시킨다.

### 6) 애니메이션과 Modifier

`CUI`는 `vector<shared_ptr<IModifier>> m_behavior`를 가진다. `Late_Update()`에서 각 modifier의 `Tick()`을 호출하고, 끝난 modifier는 제거한다.

```cpp
for (auto it = m_behavior.begin(); it != m_behavior.end(); )
{
    (*it)->Tick(fTimeDelta, this);

    if ((*it)->IsFinished())
        it = m_behavior.erase(it);
    else
        ++it;
}
```

클라이언트 modifier 예:

- `RotationModifier`
- `TransformModifier`
- `FadeModifier`
- `CScaleModifier`

단순 위치 애니메이션은 UI 클래스의 `OnUpdate()`에서 직접 처리하기도 한다.

```cpp
m_vecAni = Vector2{ lerp(m_vecAni.x, 0.f, t), 0.f };
GetUITransform()->SetAnchoredPos(m_vecAni);
```

## 7. 새 UI 추가 절차

1. `Client/Public/UI_XXX.h`, `Client/Private/UI_XXX.cpp`를 만든다.
2. 화면 단위 UI라면 `CUIPanel`을 상속한다.
3. 전용 `DESC`를 만든다. 보통 `CUIPanel::UIPANEL_DESC` 또는 `CUIRenderable::RENDERABLE_DESC`를 상속한다.
4. `OnInit()`에서 자식 이미지, 버튼, 텍스트를 생성한다.
5. 필요한 자식은 `Add_Child()` 또는 `Add_Layout_Child()`로 붙인다.
6. `UI_Controller::Ready_UI()`에서 생성 후 `UI_InsertToPool()`에 등록한다.
7. 레벨 초기화에서 `UI_Push()`로 원하는 레이어에 올린다.
8. 플레이어 상태, 이벤트, 버튼 클릭 등에 맞춰 `UI_Active()`, `UI_InActive()`, 전용 `UI_PanelActive()`를 호출한다.

## 8. 클라이언트 작성 시 주의점

- UI를 만들었다고 바로 화면에 뜨는 것은 아니다. `UI_InsertToPool()`은 보관이고, `UI_Push()`가 현재 레이어 등록이다.
- `UI_Push(..., false)`로 넣으면 레이어에는 있지만 비활성이다.
- `Find_UI_InCurLevel()`은 현재 레이어에 올라온 UI만 찾는다. pool에만 있는 UI는 찾지 못한다.
- `Add_Child()`를 호출해야 transform 부모도 연결된다.
- `KeepWorldRect`는 현재 실질 보존 로직이 구현되어 있지 않다.
- 회전 UI의 `WorldRect`는 회전 전 축 정렬 rect라서 hover 판정과 실제 이미지가 다를 수 있다.
- `Set_Zorder()`는 부모의 정렬 dirty를 켜고, 렌더 시 자식들을 `stable_sort`한다.
- 풀에 등록된 UI는 재사용되므로 `OnActive()`에서 표시 데이터 갱신을 해주는 편이 안전하다.
- `OnInit()`에서 이벤트 구독을 하면 UI 생명주기 동안 계속 살아 있으므로 중복 초기화 방지와 캡처 대상 수명에 주의한다.

## 9. 핵심 파일 맵

엔진:

- `Engine/Public/UI.h`, `Engine/Private/UI.cpp`: UI 베이스, 상태, 자식, 입력, 렌더 흐름
- `Engine/Public/UITransform.h`, `Engine/Private/UITransform.cpp`: UI 배치/행렬 계산
- `Engine/Public/UIRenderable.h`, `Engine/Private/UIRenderable.cpp`: 텍스처 UI 렌더링
- `Engine/Public/UIPanel.h`, `Engine/Private/UIPanel.cpp`: 패널과 레이아웃
- `Engine/Public/UI_Manager.h`, `Engine/Private/UI_Manager.cpp`: UI pool/layer 관리
- `Engine/Public/UIImage.h`, `UIButton.h`, `UIText.h`, `UISlot.h`, `UIProgress.h`: 기본 UI 부품

클라이언트:

- `Client/Private/UI_Controller.cpp`: UI 생성, pool 등록, 이벤트 기반 UI 상태 제어
- `Client/Private/Level_GamePlay.cpp`: 게임 플레이 레벨의 UI push
- `Client/Private/UI_TabContainer.cpp`: 탭/인벤토리/창고 패널 조립 예시
- `Client/Private/UI_Inventory.cpp`: 슬롯 재구성, 아이템 렌더링 예시
- `Client/Private/ItemInfo.cpp`: 툴팁 UI 예시
- `Client/Private/UI_HUD.cpp`, `UI_NPC.cpp`, `UI_MiniGame.cpp`, `UI_RepairShop.cpp`, `UI_Box.cpp`: 화면 단위 UI 예시
