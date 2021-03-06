//--------------------------------------------------------------------------------
//!\file state_count_mgr.h
//!\author Aslan
//!
//!\date 2008-09-01
//! last 2008-09-01
//!
//!\brief 带计数功能的状态管理器，状态由一个tagDWORDFlagArray管理，加入计数数组，状态个数不能超过DWORD的位数，状态次数不会超过127
//!
//!	Copyright (c) 2004 CTCRST Entertainment All rights reserved.
//--------------------------------------------------------------------------------
#pragma once

#include "../WorldDefine/dword_flag.h"

//---------------------------------------------------------------------------------
// 带计数功能状态管理器
//---------------------------------------------------------------------------------
template<INT nSize>
class CountStateMgr
{
public:
	//-----------------------------------------------------------------------------
	// CONSTRUCT
	//-----------------------------------------------------------------------------
	CountStateMgr() { ZeroMemory(m_sCount, sizeof(m_sCount)); }

	//-----------------------------------------------------------------------------
	// 初始化
	//-----------------------------------------------------------------------------
	VOID Init(DWORD dwStateFlags);

	//-----------------------------------------------------------------------------
	// 重置
	//-----------------------------------------------------------------------------
	VOID Reset();

	//-----------------------------------------------------------------------------
	// 设置和撤销状态
	//-----------------------------------------------------------------------------
	VOID SetState(INT nIndex);
	VOID UnsetState(INT nIndex);

	//------------------------------------------------------------------------------
	// 查询状态
	//------------------------------------------------------------------------------
	BOOL IsInState(INT nIndex);

	//------------------------------------------------------------------------------
	// 得到状态标志位数组
	//------------------------------------------------------------------------------
	DWORD GetStateFlags();

private:
	tagDWORDFlagArray<nSize>		m_FlagsArray;			// 状态标志位数组
	SHORT							m_sCount[nSize];		// 计数数组
};

//----------------------------------------------------------------------------------
// 初始化
//----------------------------------------------------------------------------------
template<INT nSize>
inline VOID CountStateMgr<nSize>::Init(DWORD dwStateFlags)
{
	// 设置标志位数组
	m_FlagsArray.Init(dwStateFlags);

	// 清空次数
	ZeroMemory(m_sCount, sizeof(m_sCount));

	// 根据某些已设标志位设置数组
	for(INT n =0; n < nSize; ++n)
	{
		if( m_FlagsArray.IsSet(n) ) ++m_sCount[n];
	}
}

//-----------------------------------------------------------------------------------
// 重置
//-----------------------------------------------------------------------------------
template<INT nSize>
inline VOID CountStateMgr<nSize>::Reset()
{
	m_FlagsArray.Init(0);

	ZeroMemory(m_sCount, sizeof(m_sCount));
}

//-----------------------------------------------------------------------------------
// 设置状态
//-----------------------------------------------------------------------------------
template<INT nSize>
inline VOID CountStateMgr<nSize>::SetState(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	// 查看引用计数
	if( m_sCount[nIndex] > 0 )
	{
		++m_sCount[nIndex];
	}
	else
	{
		m_FlagsArray.SetFlag(nIndex);
		m_sCount[nIndex] = 1;
	}
}

//-------------------------------------------------------------------------------------
// 撤销状态
//-------------------------------------------------------------------------------------
template<INT nSize>
inline VOID CountStateMgr<nSize>::UnsetState(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return;

	if( m_sCount[nIndex] > 0 )
	{
		if( --m_sCount[nIndex] <= 0 )
		{
			m_FlagsArray.UnsetFlag(nIndex);
		}
	}
}

//---------------------------------------------------------------------------------------
// 是否在某个状态下
//---------------------------------------------------------------------------------------
template<INT nSize>
inline BOOL CountStateMgr<nSize>::IsInState(INT nIndex)
{
	if( nIndex < 0 || nIndex >= nSize ) return FALSE;

	return m_FlagsArray.IsSet(nIndex);
}

//-----------------------------------------------------------------------------------------
// 得到状态标志位数组
//-----------------------------------------------------------------------------------------
template<INT nSize>
inline DWORD CountStateMgr<nSize>::GetStateFlags()
{
	return m_FlagsArray.GetFlags();
}