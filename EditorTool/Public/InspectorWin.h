#pragma once
#include "Editor_Win.h"

NS_BEGIN(Engine)
class CEntity;
NS_END

NS_BEGIN(Editor)

class CInspectorWin final : public CEditor_Win
{
public:
    CInspectorWin();

public:
    virtual ~CInspectorWin() { Free(); }

public:

public:
    HRESULT Initialize() override { return S_OK; }
    void Update(float) override {}
    void Render() override;

private:
    _string ConvertW2A(const wstring& wstr);

private:
    void RenderEntity(const shared_ptr<Engine::CEntity>& obj);

private:
    bool     m_EditingName = false;
    char     m_NameBuf[256] = {};
    uint64_t m_LastSelectedId = 0;
public:
    static shared_ptr<CInspectorWin> Create();
    void Free() override;
};

NS_END