#pragma once
#include "Base.h"

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
    virtual ~CPrototype_Manager() = default;
public:
    HRESULT Initialize(_uint iNumLevels);
    HRESULT Add_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype);
    CBase* Clone_Prototype(PROTOTYPE ePrototy, _uint iLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
    HRESULT Clear_Prototype(_uint iLevelIndex);

private:
    map<const _wstring, CBase*>* m_pPrototypes = { nullptr }; //
    typedef map<const _wstring, CBase*> PROTOTYPES;

    _uint m_iNumLevel = {}; // 레벨의 총 갯수
private:
    CBase* Find_Prototype(_uint iLevelIndex, const _wstring& strPrototypeTag);
public:
    static CPrototype_Manager* Create(_uint iNumLevel);
    virtual void Free() override;
};

NS_END