#pragma once
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "Model.h"
#include "Collider.h"


// 생성한 원형 객체를 보관한다
// 요청에 따라 보관하고 있던 원형 객체를 복제하여 리턴한다

// 사용하는 이유
// 객체, 리소스 다 프로토 타입 - 비용이 싸다 속도 면에서/정보 저장 / 데이터 채우는거 파일 입출력, 서버 올려 놓고 셋팅 -> 이러한 행위가 느리다  
//로딩간에만 파일 입출력 , 서버 통신 하자 프로토 타입을 쓰는 이유

NS_BEGIN(Engine)

class CPrototype_Manager :
    public CBase
{
private:
    CPrototype_Manager();
public:
    virtual ~CPrototype_Manager();
public:
    HRESULT Initialize(_uint iNumLevels);
    HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, shared_ptr<CBase> pPrototype);
    shared_ptr<CBase> Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
    //shared_ptr<CBase> Clone_Prototype(shared_ptr<CBase> pPrototype, void* pArg);
    HRESULT Clear_Prototype(_uint iLevelIndex);


    shared_ptr<CBase> Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);
    void OnGui()override;
private:
    map<const _wstring, shared_ptr<CBase>>* m_pPrototypes = { nullptr }; //
    typedef map<const _wstring, shared_ptr<CBase>> PROTOTYPES;

    _uint m_iNumLevel = {}; // 레벨의 총 갯수
private:
    _wstring m_strSelectedTag = {};
    _uint m_iSelectedLevel = {};
    weak_ptr<CGameInstance> m_pGameInstance = {};
public:
    static unique_ptr<CPrototype_Manager> Create(_uint iNumLevel);
    virtual void Free() override;
};

NS_END