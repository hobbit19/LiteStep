/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/ 
/****************************************************************************
****************************************************************************/
#include "BangCommand.h"
#include "../utility/core.hpp"
#include "ThreadedBangCommand.h"

Bang::Bang(DWORD dwThread, BangCommand* pfnBang, LPCSTR pszCommand)
:m_szCommand(pszCommand)
{
    m_bEX = false;
	m_bBang = pfnBang;
	m_dwThreadID = dwThread;
}

Bang::Bang(DWORD dwThread, BangCommandEx* pfnBang, LPCSTR pszCommand)
:m_szCommand(pszCommand)
{
	m_bEX = true;
	m_bBangEX = pfnBang;
	m_dwThreadID = dwThread;
}

Bang::~Bang()
{}

void Bang::Execute(HWND hCaller, LPCSTR pszParams)
{
	DWORD dwThreadID = GetCurrentThreadId();

	if (dwThreadID != m_dwThreadID)
	{
		ThreadedBangCommand * pInfo =
            new ThreadedBangCommand(hCaller, m_szCommand.c_str(), pszParams);

		if (pInfo != NULL)
		{
			// target thread releases pInfo
            PostThreadMessage(m_dwThreadID, LM_THREAD_BANGCOMMAND, (WPARAM)pInfo, 0);
        }
    }
    else
    {
#if !defined(LS_NO_EXCEPTION)
        try
        {
#endif /* LS_NO_EXCEPTION */
            if (m_bEX)
            {
                m_bBangEX(hCaller, m_szCommand.c_str(), pszParams);
            }
            else
            {
                m_bBang(hCaller, pszParams);
            }
#if !defined(LS_NO_EXCEPTION)
        }
        catch (...)
        {
            ErrorEx(m_szCommand.c_str(), IDS_LITESTEP_BANGEXCEPTION,
                _T("Exception during bang command execution.\n")
                _T("Please contact the module author.\n\nBang parameters: %s"),
                pszParams != NULL ? pszParams : _T("(null)"));
        }
#endif /* LS_NO_EXCEPTION */
	}
}
