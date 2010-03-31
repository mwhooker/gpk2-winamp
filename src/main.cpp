/************************************************************************************
	gpk2-winamp is a winamp plugin for interfacing with the gerpok.com web service
	Copyright (C) 2006, 2007  Matthew Hooker, Gerpok Inc.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

	email at mwhooker [at] gmail <dot> com
************************************************************************************/


/**
\file main.cpp
\bug sometimes it does not grab to correct length from the song. This often happens on the first play.
\bug does not know that someone has replayed the same song

*/
#include <constants.h>
#include <stdafx.h>
#include <utilities.h>
#include <gerpok.h>
#include <WinampController.h>
#include <errors.h>
#include <state controller.h>
#include "main.h"
#include <fstream>
#include <resource.h>
#include <loki/Singleton.h>
#include "messaging.h"
CWinampController g_Controller;
HWND g_hwndParent = NULL;
HINSTANCE g_hDllInstance = NULL; 
HANDLE hGerpokThread;
DWORD dwGerpokThread;
HANDLE hGerpokSem;

char *szSemName = "GerpokBetaSemaphore";


// Global Functions
void config();
void quit();
int mlinit();
int GetLoginInformationDlg();
bool g_bEnable = false;



BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			//g_bEnable = false;
			//break;
		case DLL_PROCESS_DETACH:
			//g_bEnable = false;
			break;
    }
    return TRUE;
}



void config()
{
	GetLoginInformationDlg();
	//DebugSpew("Config()");	
}


/*****************************************************
******************************************************
***
*** M E D I A   L I B R A R Y   P L U G I N
***
******************************************************
*****************************************************/
int msgproc(int message_type, int param1, int param2, int param3)
{
	BOOL bRet = FALSE;

	switch(message_type)
	{
	case ML_MSG_CONFIG:
		config();
		bRet = TRUE;
		break;
	default:;
	}

	return bRet;
}

winampMediaLibraryPlugin mlplugin = 
{
	MLHDR_VER,
	"Gerpok's Winamp ml Plugin",
	mlinit,
	quit,
	msgproc,
};

extern "C" {

	__declspec( dllexport ) winampMediaLibraryPlugin * winampGetMediaLibraryPlugin()
	{
		return &mlplugin;
	}

};


bool FindAndSetDllSearchPath()
{
	DebugSpew(L"FindAndSetDllSearchPath()");
	DWORD Length = MAX_PATH;
	HKEY HKey;
	
	
	long lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Gerpok\\libgerpok\\"),
		0, KEY_READ, &HKey);
	
	if( lRet == ERROR_SUCCESS )
	{
		char szInstall[MAX_PATH] = {0};
		RegQueryValueEx(HKey, TEXT("InstallDir"), 0, 0, (LPBYTE)szInstall, &Length);
		RegCloseKey(HKey);
		DebugSpew(L"FindAndSetDllSearchPath(opened key correctly)");
		SetDllDirectory(szInstall);
		return true;
	} else {
		throw gpk::ReInstallError();
	}
	return false;
}


BOOL CALLBACK UPDialogProc(HWND hwndDlg, 
						   UINT message, 
						   WPARAM wParam, 
						   LPARAM lParam) 
{
	Configure* pConfig = Configure::GetInstance();
	HWND hwndOwner; 
	RECT rc, rcDlg, rcOwner; 
	char szPassword[256] = {0};
	char szUserName[256] = {0};
	switch (message) 
	{ 
	case WM_INITDIALOG: 
		//		DebugSpew(L"UPDialogProg(init)");
		if ((hwndOwner = GetParent(hwndDlg)) == NULL) 
		{
			hwndOwner = GetDesktopWindow(); 
		}
		GetWindowRect(hwndOwner, &rcOwner); 
		GetWindowRect(hwndDlg, &rcDlg); 
		CopyRect(&rc, &rcOwner); 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
		OffsetRect(&rc, -rc.left, -rc.top); 
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

		SetWindowPos(hwndDlg, 
			HWND_TOP, 
			rcOwner.left + (rc.right / 2), 
			rcOwner.top + (rc.bottom / 2), 
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE); 

		if (GetDlgCtrlID((HWND) wParam) != IDD_UNP) 
		{ 
			SetFocus(GetDlgItem(hwndDlg, IDD_UNP)); 
			return FALSE; 
		} 
		return true;
	case WM_COMMAND: 
		switch (LOWORD(wParam)) 
		{ 
		case IDOK: 
			if (!GetDlgItemText(hwndDlg, IDC_EDIT_USERNAME, szUserName, 255)) 
				*szUserName=0;
			if (!GetDlgItemText(hwndDlg, IDC_EDIT_PASSWORD, szPassword, 255)) 
				*szPassword=0;
			if (0 != szPassword && 0 != szUserName)
				pConfig->SetUnameAndPassword(szUserName, szPassword);
			//return true;
		case IDCANCEL: 
			//on cancel, I need to break and pretend like we're off line
			EndDialog(hwndDlg, wParam); 
			return TRUE; 
		} 
	} 
	return FALSE; 
} 


int GetLoginInformationDlg()
{
	//make this modeless if at all possible
	HWND hLoginDialog = 0;
	hLoginDialog = (HWND)DialogBox(g_hDllInstance, 
		MAKEINTRESOURCE(IDD_UNP), 
		GetDesktopWindow(), 
		UPDialogProc);
	return 0;
	/*
	if(hLoginDialog != NULL)
	{
		ShowWindow(hLoginDialog, SW_SHOW);
		UpdateWindow(hLoginDialog);

		EnableWindow(hLoginDialog, FALSE);
	} else
    {
        char buf [100];
        wsprintf (buf, "Error x%x", GetLastError ());
        MessageBox (0, buf, "CreateDialog", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

	DebugSpew(L"GetLoginInformationDlg()");
	MSG  msg;
	int status;
	while ((status = GetMessage(& msg, 0, 0, 0)) != 0)
	{
		DebugSpew(L"while message box(%d)", status);
		if (status == -1)
			return -1;
		if (!IsDialogMessage (hLoginDialog, & msg))
		{
			TranslateMessage ( & msg );
			DispatchMessage ( & msg );
		}
		
	} 
	return msg.wParam;*/
}

DWORD WINAPI runIt(LPVOID iValue)
{
/*	if (!FindAndSetDllSearchPath())
	{
		MessageBox(g_hwndParent, 
				"There was an error retrieving the libgerpok install path. Please reinstall libgerpok.", 
				"Please Reinstall Libgerpok", MB_OK | MB_ICONERROR);
		return 2;
	}
	*/
	
	Configure* pConfig;
	
	try {
		FindAndSetDllSearchPath();
		pConfig = Configure::GetInstance();
		pConfig->_Init();
	}
	catch (gpk::ReInstallError)
	{
		DebugSpew(L"ReInstallError");
		MessageBox(g_hwndParent, 
			"There was an error retrieving the libgerpok install path. Please reinstall libgerpok.", 
			"Please Reinstall Libgerpok", MB_OK | MB_ICONERROR);
		return 2;
	
	} catch (...) {
		DebugSpew(L"ML caught unknown error");
	}
	
	
	OMESSAGE()->Log()<<"\n\n\n *********GERPOK STARTUP**********\n"<<std::endl;
	GetLoginInformationDlg();

	//pConfig->SetUnameAndPassword("kint", "102486");
	//WAMetaRetriever MetaRet();
	WAStateMachine State;//&MetaRet);
	DWORD oldState = 0;
	DWORD state = WA_STATE_STOP;

	//set this to be the first track's stuff?
	long oldLen = g_Controller.GetCurrentSongLength();
	long oldPos = g_Controller.GetPlayListPos();

	while (g_bEnable)
	{
		Sleep(400);
		state = g_Controller.GetWinampState();
		if (state != oldState)
		{
			switch (state)
			{
			case WA_STATE_PLAY:
				oldLen = g_Controller.GetCurrentSongLength();
				oldPos = g_Controller.GetPlayListPos();
				Sleep(400);
				State.Play();
				DebugSpew(L"case WA_STATE_PLAY:");
				break;
			case WA_STATE_PAUSE:
				State.Pause();
				DebugSpew(L"case WA_STATE_PAUSE:");
				break;
			case WA_STATE_STOP:
				State.Stop();
				DebugSpew(L"case WA_STATE_STOP:");
				break;
			default:
				break;
			};
		} else if (state == WA_STATE_PLAY)
		{
			long newLen = g_Controller.GetCurrentSongLength();
			long newPos = g_Controller.GetPlayListPos();
			
			if(oldPos == newPos) 
				continue;
			else if (newLen == oldLen)
			{
				oldPos = newPos;
				continue;
			}
			DebugSpew(L"state == WA_STATE_PLAY && oldLen %d, newLen %d",oldLen, newLen);
			DebugSpew(L"state == WA_STATE_PLAY && oldPos %d, newPos %d",oldPos, newPos);
			
			//think we need to wait a little to get the correct time. Maybe I can rewrite getcurrentsong length
			//if (newLen == -1)
			//	continue;
			

			oldLen = newLen;
			oldPos = newPos;
			State.Play();

		}
		oldState = state;
		
	}

	
	return 0;
}


int mlinit()
{
	
	DebugSpew(L"mlInit()");
	g_bEnable = true;
	
// Handle Multiple Instances
	hGerpokSem = CreateSemaphore (NULL, 1, 1,szSemName);
    if (WaitForSingleObject (hGerpokSem, 0) == WAIT_TIMEOUT) 
	{
		CloseHandle (hGerpokSem);
		DebugSpew(L"RunIt wait timeout");
		::MessageBox(mlplugin.hwndWinampParent, "Error!", "Problem creating library. Check to see if another instance is running, or try removing previous versions", MB_ICONERROR | MB_OK); 
		return -1;
	}
	g_hDllInstance = mlplugin.hDllInstance;
	g_hwndParent = mlplugin.hwndWinampParent;

	g_Controller.Init(0, g_hwndParent, mlplugin.hwndLibraryParent );

	hGerpokThread = CreateThread(NULL,0,runIt,NULL,0,&dwGerpokThread);
	
	return 0;
}

//extern Configure *Configure::pInstance;


void quit()
{
//	CloseHandle(hGerpokThread);
//	WaitForSingleObject(hGerpokThread, 300);
	DebugSpew(L"quit()");

	// Before shutdown remove semaphore
	ReleaseSemaphore (hGerpokSem, 1, NULL);
	CloseHandle (hGerpokSem);
	
	
	g_bEnable = false;	
	WaitForSingleObject (hGerpokThread, 500L); 
	CloseHandle (hGerpokThread);
	Configure::GetInstance()->_Release();
}





WAMetaRetriever::WAMetaRetriever()/*CWinampController *controller) : Controller_(controller)*/
{

}

bool WAMetaRetriever::CopyCurrentMetaInfo(MetaInfo &Song)
{

	DebugSpew(L"CopyCurrentMetaInfo()");
	MetaInfo info;

	
	///need to test this
	if(g_Controller.GetCurrentSongInfo(&info))
	{
		wcsncpy(Song.wzArtist, 
			info.wzArtist,
			sizeof(Song.wzArtist));
		wcsncpy(Song.wzTitle, 
			info.wzTitle,
			sizeof(Song.wzTitle));
		wcsncpy(Song.wzAlbum, 
			info.wzAlbum,
			sizeof(Song.wzAlbum));

		Song.nLength		= info.nLength;
		Song.nTrackNum	= info.nTrackNum;
		DebugSpew(L"METAINFO(winamp <2.90): album: %s, artist: %s, title: %s, #: %d, len: %d", Song.wzAlbum, Song.wzArtist, Song.wzTitle, Song.nTrackNum, Song.nLength);
	}
	else
	{
		// Winamp 5 or Winamp 2.90 onwards
		mbstowcs(Song.wzArtist, 
			g_Controller.GetCurrentSongMetadata("Artist").c_str(),
			sizeof(Song.wzArtist));
		mbstowcs(Song.wzTitle, 
			g_Controller.GetCurrentSongMetadata("Title").c_str(),
			sizeof(Song.wzTitle));
		mbstowcs(Song.wzAlbum, 
			g_Controller.GetCurrentSongMetadata("Album").c_str(),
			sizeof(Song.wzAlbum));
		Song.nTrackNum = atoi(g_Controller.GetCurrentSongMetadata("Track").c_str());

		///this doesn't always work.
		Song.nLength = g_Controller.GetCurrentSongLength();

		DebugSpew(L"METAINFO(winamp >= 2.90): album: %s, artist: %s, title: %s, #: %d, len: %d", Song.wzAlbum, Song.wzArtist, Song.wzTitle, Song.nTrackNum, Song.nLength);
	}
	//}


	//X<<L"Metainfo: "<<((wchar_t *)Song.wzArtist)<<endl;
	if (Song.nLength <= 0)
		Song.nLength = g_Controller.GetCurrentSongLength();
	
	return ((wcslen(Song.wzArtist) > 0) && (wcslen(Song.wzTitle) > 0));
}
