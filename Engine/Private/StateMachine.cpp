#include "StateMachine.h"
#include "State.h"
CStateMachine::CStateMachine() :m_curState(nullptr), m_prevStateKey(0), m_curStateKey(0)
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
