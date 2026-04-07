#include "StateMachine.h"
#include "State.h"
CStateMachine::CStateMachine() :m_CurState(nullptr), m_PrevStateKey(0), m_CurStateKey(0)
{
}

CStateMachine::~CStateMachine()
{
	
}

void CStateMachine::Free()
{
	m_vecState.clear();
	__super::Free();
}
