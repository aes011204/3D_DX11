#pragma once
#include "Editor_Win.h"

NS_BEGIN(Editor)

class CConsoleWin : public CEditor_Win
{
public:
    CConsoleWin();
    virtual ~CConsoleWin();

public:

public:
    HRESULT Initialize() override { return S_OK; }
    void Update(float) override {}
    void Render() override;

private:

private:
    bool     m_EditingName = false;
    char     m_NameBuf[256] = {};
    uint64_t m_LastSelectedId = 0;
public:
    static shared_ptr<CConsoleWin> Create();
    void Free() override;

};

NS_END