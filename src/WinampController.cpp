// WinampController.cpp: implementation of the CWinampController class.
//
//////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include <constants.h>
#include <gerpok.h>
#include "utilities.h"
//#include "CSTDStringWrapper.h"
#include <WinampController.h>
#include <shellapi.h>


//#include "dbg.h"
#ifdef UNICODE
#undef UNICODE
#endif

//////////////////////////////////////////////////////////////////////
// General Stuff
//////////////////////////////////////////////////////////////////////

CWinampController::CWinampController()
{
	m_hWndWinamp		= NULL;
	m_hWndWinampEQ		= NULL;
	m_hWndWinampPE		= NULL;
	m_hWndWinampMB		= NULL;
	m_hWndWinampML		= NULL;
	m_strVersionString	= "";		// Winamp Version String
	m_strWinampDir		= "";		// Winamp Directory
}

CWinampController::~CWinampController()
{
}

BOOL CWinampController::Init(BOOL bStartWinampIfNotRunning,
							 HWND WinampWnd /* = NULL */,
							 HWND WinampWndML /* = NULL */)
{	
	if(WinampWnd == NULL)
		m_hWndWinamp = FindWindow("Winamp v1.x", NULL);
	else
		m_hWndWinamp = WinampWnd;

	if(m_hWndWinamp == NULL)
	{
		if(bStartWinampIfNotRunning)
		{
			::ShellExecute(NULL, "open", "c:\\progra~1\\winamp\\winamp.exe", NULL, "c:\\progra~1\\winamp\\", SW_MINIMIZE);

			//Give it time to show itself
			Sleep(1000);
			m_hWndWinamp = FindWindow("Winamp v1.x", NULL);

			if(m_hWndWinamp == NULL)
				return FALSE;
		}
	}/*
	else
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp Window");			
	
	if((m_hWndWinampEQ = FindWindow("Winamp EQ", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp EQ Window");

	if((m_hWndWinampPE = FindWindow("Winamp PE", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp PE Window");

	if((m_hWndWinampMB = FindWindow("Winamp MB", NULL)) == NULL)
		PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp MB Window");
*/
	if(WinampWndML == NULL)
	{

		long libhwndipc = (LONG)SendMessage(m_hWndWinamp, WM_WA_IPC, (WPARAM)"LibraryGetWnd", IPC_REGISTER_WINAMP_IPCMESSAGE);

//		if((m_hWndWinampML = (HWND)SendMessage(m_hWndWinamp,WM_WA_IPC,-1,(LPARAM)libhwndipc)) == NULL)
//			PRINTF(DEBUG_DETAIL, "CWinampController::Init", "Failed to find Winamp ML Window");
	}
	else
		m_hWndWinampML = WinampWndML;

	GetWinampDir();
	GetPlayerVersion();
	return TRUE;
}

BOOL CWinampController::WindowsReady()
{
	BOOL bReady = TRUE;

	if(m_hWndWinamp == NULL || !IsWindow(m_hWndWinamp))
		bReady = FALSE;

	if(m_hWndWinampEQ == NULL || !IsWindow(m_hWndWinampEQ))
		bReady = FALSE;

	if(m_hWndWinampPE == NULL || !IsWindow(m_hWndWinampPE))
		bReady = FALSE;

	if(m_hWndWinampMB == NULL || !IsWindow(m_hWndWinampMB))
		bReady = FALSE;

	if(!bReady)
		Init(TRUE);

	return TRUE;
}

void CWinampController::SendCommand(long Code)
{
	SendMessage(m_hWndWinamp, WM_COMMAND, Code, 0);
}


//////////////////////////////////////////////////////////////////////
// General Winamp Stuff
//////////////////////////////////////////////////////////////////////
long CWinampController::GetVersion()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION);
}

long CWinampController::GetStatus()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_ISPLAYING);
}

void CWinampController::SetStatus(long newVal)
{
	
	switch(newVal)
	{
	case WINAMP_STOPPED:
		SendCommand(WINAMP_STOP);
		break;
	case WINAMP_PAUSED:
		SendCommand(WINAMP_PAUSE);
		break;
	default:
		Play();
		break;
	}
}


void CWinampController::NextTrack()
{
	SendCommand(WINAMP_FASTFWD);
}

void CWinampController::PrevTrack()
{
	SendCommand(WINAMP_REWIND);
}

long CWinampController::GetCurrentPos()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
}

void CWinampController::SetCurrentPos(long newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_JUMPTOTIME);
}

void CWinampController::SetVolume(long newVal)
{
	
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 255)
	{
		newVal = 255;
	}
	long CurrentValue(SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETVOLUME));
}

void CWinampController::IncreaseVolume()
{
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_VOLUMEUP, 0);
}

void CWinampController::DecreaseVolume()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_VOLUMEDOWN, 0);
}

long CWinampController::GetSampleRate()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETINFO);
}

long CWinampController::GetBitrate()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,1,IPC_GETINFO);
}

long CWinampController::GetChannels()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,2,IPC_GETINFO);
}

void CWinampController::SetPanning(long PanIndex)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,PanIndex,IPC_SETPANNING);
}

void CWinampController::ChangeDirectory(string NewDir)
{
	if(!NewDir.length())
	{
		COPYDATASTRUCT cds;
		cds.dwData = IPC_CHDIR;
		cds.lpData = (void *) NewDir.c_str();
		cds.cbData = NewDir.length(); // include space for null char
		SendMessage(m_hWndWinamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
	}
}

string CWinampController::GetSongFileName(long PlayListPos)
{	
	string Name;
	Name = (char *)SendMessage(m_hWndWinamp,WM_WA_IPC,PlayListPos,IPC_GETPLAYLISTFILE);
	
	return Name;
}

string CWinampController::GetSongTitle(long PlayListPos)
{
	string Name = (char *)SendMessage(m_hWndWinamp,WM_WA_IPC,PlayListPos,IPC_GETPLAYLISTTITLE);
	return Name;
}

string CWinampController::GetCurrentSongFileName()
{
	return GetSongFileName(GetPlayListPos());
}

string CWinampController::GetCurrentSongTitle()
{
	return GetSongTitle(GetPlayListPos());
}

string CWinampController::GetSkinName()
{
	
	string Temp;
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)Temp.c_str(),IPC_GETSKIN);
	return Temp;
}

void CWinampController::SetSkinName(string Name)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)Name.c_str(),IPC_SETSKIN);	
}

void CWinampController::StartPlugIn(string PlugInName)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,(WPARAM)PlugInName.c_str(),IPC_EXECPLUG);
}

void CWinampController::ToggleShufflePlay()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FILE_SHUFFLE, 0);
}

void CWinampController::ToggleRepeatPlay()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FILE_REPEAT, 0);
}

void CWinampController::Stop()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_STOP, 0);
}

void CWinampController::Pause()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_PAUSE, 0);
}

void CWinampController::FastForward()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FASTFWD, 0);
}

void CWinampController::Rewind()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_REWIND, 0);
}

void CWinampController::CloseWinamp()
{
	
	SendMessage(m_hWndWinamp, WM_CLOSE, 0, 0);
}

void CWinampController::SongChange()
{
}

long CWinampController::GetCurrentSongLength()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC, 1,IPC_GETOUTPUTTIME);
}

void CWinampController::FastFwd5Secs()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_FFWD5S, 0);
}

void CWinampController::Rewind5Secs()
{
	
	SendMessage(m_hWndWinamp, WM_COMMAND, WINAMP_REW5S, 0);
}

BOOL CWinampController::GetWinampVisible()
{
	return IsWindowVisible(m_hWndWinamp);
}

void CWinampController::SetWinampVisible(BOOL newVal)
{
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinamp))
		{
			ShowWindow(m_hWndWinamp, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinamp))
		{
			ShowWindow(m_hWndWinamp, SW_HIDE);
		}
	}
}

BOOL CWinampController::GetShuffleStatus()
{
	
	if(GetMenuState(GetOptionMenu(), WINAMP_MENU_SHUFFLE, MF_BYCOMMAND) == MF_CHECKED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CWinampController::SetShuffleStatus(BOOL newVal)
{
	
	BOOL CurVal = GetShuffleStatus();
	if(CurVal != newVal)
	{
		ToggleShufflePlay();
	}
}

BOOL CWinampController::GetRepeatStatus()
{
	
	if(GetMenuState(GetOptionMenu(), WINAMP_MENU_REPEAT, MF_BYCOMMAND) == MF_CHECKED)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CWinampController::SetRepeatStatus(BOOL newVal)
{
	
	BOOL CurVal = GetRepeatStatus();
	if(CurVal != newVal)
	{
		ToggleRepeatPlay();
	}
}

HMENU CWinampController::GetOptionMenu()
{
	
	// Get System menu handle
	HMENU hMenuSystem = GetSystemMenu(m_hWndWinamp, 0);

	//Get Winamp sub-menu handle
	HMENU hMenuWinamp = GetSubMenu(hMenuSystem, 0);

	//Get Options sub-menu handle
	return GetSubMenu(hMenuWinamp, WINAMP_MENU_OPTIONS_POS);
}

//////////////////////////////////////////////////////////////////////
// Playlist Stuff
//////////////////////////////////////////////////////////////////////

void CWinampController::ClearPlaylist()
{
	SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_DELETE);
}

long CWinampController::GetPlayListPos()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
}

void CWinampController::SetPlayListPos(long newVal)
{
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETPLAYLISTPOS);
}

long CWinampController::GetPlayListCount()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH);
}

BOOL CWinampController::GetPlaylistVisible()
{
	return IsWindowVisible(m_hWndWinampPE);
}

void CWinampController::SetPlaylistVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampPE))
		{
			ShowWindow(m_hWndWinampPE, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampPE))
		{
			ShowWindow(m_hWndWinampPE, SW_HIDE);
		}
	}
}

void CWinampController::DeletePEItem(int Pos)
{
	MovePEHighlight(Pos);
	DeletePECurHighlight();
}

void CWinampController::DeletePECurHighlight()
{
	SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_DEL_FRM_PE, 0 );
}

void CWinampController::MovePEHighlight(int Pos)
{
	int PELength = GetPlayListCount();

	if(Pos < PELength)
	{
		SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_HLT_UP, 0);

		// Move the selector down to the song to remove
		for(int i = 0; i < Pos; i++ )
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_HLT_DOWN, 0 );
	}
}

bool CWinampController::MovePEItem(int curPos, int NewPos)
{
	int diff = 0;

	if(curPos == NewPos)
		return false;

	MovePEHighlight(curPos);

	if(curPos > NewPos)
	{
		diff = curPos - NewPos;

		for(int i = 0; i < diff; i++)
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_ITEM_UP, 0 );
	}
	else
	{
		diff = NewPos - curPos;

		for(int i = 0; i < diff; i++)
			SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_MV_PE_ITEM_DOWN, 0 );
	}

	return true;
}

typedef struct 
{ 
	char file[MAX_PATH]; 
	int index; 
} fileinfo; 
 
void CWinampController::PLInsertTrack(string strFile, int nPos)
{
/*	int nCount = GetPlayListCount();

	nCount++;
	Enqueue(strFile);
	MovePEItem(nCount, nPos);*/

	COPYDATASTRUCT cds; 
	fileinfo f; 
	cds.dwData = IPC_PE_INSERTFILENAME; 
	strcpy(f.file, strFile.c_str()); // Path to the new entry 
	f.index=nPos;                 // Position to insert at 
	cds.lpData = (void *)&f; 
	cds.cbData = sizeof(fileinfo); 
	::SendMessage(m_hWndWinampPE,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
}

//////////////////////////////////////////////////////////////////////
// EQ Stuff
//////////////////////////////////////////////////////////////////////
long CWinampController::GetEqPosition(long Band)
{
	
	if(Band < 0)
	{
		Band = 0;
	}
	if(Band > 9)
	{
		Band = 9;
	}
	return SendMessage(m_hWndWinamp,WM_WA_IPC,Band,IPC_GETEQDATA);
}

void CWinampController::SetEqPosition(long Band, long newVal)
{
	
	if(Band < 0)
	{
		Band = 0;
	}
	if(Band > 9)
	{
		Band = 9;
	}
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 63)
	{
		newVal = 63;
	}
	SendMessage(m_hWndWinamp,WM_WA_IPC,Band,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

long CWinampController::GetPreAmpPosition()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,10,IPC_GETEQDATA);
	
}

void CWinampController::SetPreAmpPosition(long newVal)
{
	
	if(newVal < 0)
	{
		newVal = 0;
	}
	if(newVal > 63)
	{
		newVal = 63;
	}
	SendMessage(m_hWndWinamp,WM_WA_IPC,10,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

BOOL CWinampController::GetEqEnabled()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,11,IPC_GETEQDATA);
}

void CWinampController::SetEqEnabled(BOOL newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,11,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}

BOOL CWinampController::GetAutoloadEnabled()
{
	
	return SendMessage(m_hWndWinamp,WM_WA_IPC,12,IPC_GETEQDATA);
}

void CWinampController::SetAutoloadEnabled(BOOL newVal)
{
	
	SendMessage(m_hWndWinamp,WM_WA_IPC,12,IPC_GETEQDATA);
	SendMessage(m_hWndWinamp,WM_WA_IPC,newVal,IPC_SETEQDATA);
}
BOOL CWinampController::GetEqualizerVisible()
{
	
	return IsWindowVisible(m_hWndWinampEQ);
}

void CWinampController::SetEqualizerVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampEQ))
		{
			ShowWindow(m_hWndWinampEQ, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampEQ))
		{
			ShowWindow(m_hWndWinampEQ, SW_HIDE);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// MB Stuff
//////////////////////////////////////////////////////////////////////

BOOL CWinampController::GetMiniBrowserVisible()
{
	
	return IsWindowVisible(m_hWndWinampMB);
}

void CWinampController::SetMiniBrowserVisible(BOOL newVal)
{
	
	if(newVal)
	{
		if(!IsWindowVisible(m_hWndWinampMB))
		{
			ShowWindow(m_hWndWinampMB, SW_SHOW);
		}
	}
	else
	{
		if(IsWindowVisible(m_hWndWinampMB))
		{
			ShowWindow(m_hWndWinampMB, SW_HIDE);
		}
	}
}

void CWinampController::SetMiniBrowserURL(string strUrl, BOOL bForceOpen)
{
	char* lpszUrl = NULL;
	
	// Force open the mini-browser window
	if(bForceOpen)
		::SendMessage(m_hWndWinamp, WM_WA_IPC, NULL, IPC_MBOPEN);
	
	//Build the url string to be displayed
	lpszUrl = new char[strUrl.length()+1];
	
	sprintf(lpszUrl, "%s", strUrl.c_str());
	
	//Make the mini-browser navigate to the URL 
	::SendMessage(m_hWndWinamp,WM_WA_IPC, reinterpret_cast<WPARAM>(lpszUrl), IPC_MBOPENREAL);
	
	delete[] lpszUrl;
}

BOOL CWinampController::IsInetAvailable()
{
	return ::SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_INETAVAILABLE);
}

void CWinampController::JumpToTrack(unsigned int Number)
{
	
	PostMessage(m_hWndWinamp,WM_COMMAND,WINAMP_BUTTON4,0);
	Number--;
	PostMessage(m_hWndWinamp,WM_WA_IPC,Number,IPC_SETPLAYLISTPOS);
	PostMessage(m_hWndWinamp,WM_COMMAND,WINAMP_BUTTON2,0);
}

/*BOOL CWinampController::GetWinampStats(WA_STATS& stats)
{
	CStdString strVal, strTemp;
	int nPos;

	// Get the data
	char buffer[MAX_PATH];
	char buffer2[MAX_PATH];
	GetWindowsDirectory(buffer, MAX_PATH);
	strcat(buffer, "\\winamp.ini");

	DWORD dwRes = GetPrivateProfileString("WinampReg", "Stats",	NULL, buffer2, MAX_PATH, buffer);
	
	if(dwRes <= 0)
		return FALSE;

	strTemp = buffer2;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
		stats.nWinampRuns =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunning =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunningMB =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
//		stats.tRunningNoneMin =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
	//	stats.tRunningPlay =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	nPos = strTemp.Find(',');
	if(nPos > 0)
	{
		strVal = strTemp.Left(nPos);
//		stats.tRunningPlayNonMin =	strtoul(strVal.GetBuffer(0), NULL,16);
		strTemp = strTemp.Mid(nPos+1);
	}
	else 
		return FALSE;

	return TRUE;
}
*/
string CWinampController::GetWinampDir()
{
//	PRINTF(DEBUG_FLOW, "CWinampController::GetWinampDir", "Getting Winamp Dir.");
	if(!m_strWinampDir.length())
		return m_strWinampDir;

	char filename[512];
	string Path;
	GetModuleFileName(NULL,filename,sizeof(filename));
	Path = filename;

	int pos = Path.find_last_of('\\');
	if(pos > 0)
	{
		// Get Winamp Directory
		m_strWinampDir = Path[pos + 1];
		return m_strWinampDir;
	}
	//else
	//	PRINTF(DEBUG_ERROR, "CWinampController::GetWinampDir", "Failed to Get Winamp Dir.");

	return "";
}

void CWinampController::SetVolume(int nVolume)
{
	if(nVolume < 0 || nVolume > 255)
		return;

	SendMessage(m_hWndWinamp,WM_WA_IPC,nVolume,IPC_SETVOLUME);
}

string CWinampController::GetPlayerVersion()
{
	if(m_strVersionString.length())
	{
		string Text;
		
		//Text.Format("%x", SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION));
//		sprintf(Text, "%x", SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETVERSION));
//		sprintf(m_strVersionString, "%s.%s%s", Text.find_first_of(1), Text.find_first_of(1,2), Text.find_first_of(3));
		//m_strVersionString.Format("%s.%s%s", Text.Left(1).c_str(), Text.Mid(1, 1).c_str(), Text.Mid(3).c_str());
	}

	return m_strVersionString;
}

BOOL CWinampController::Enqueue(string strFileName)
{
	COPYDATASTRUCT cds;
	cds.dwData = IPC_PLAYFILE;
	cds.lpData = (void *)strFileName.c_str();
	cds.cbData = strlen((char *) cds.lpData) + 1; // include space for null char

	int nRes = ::SendMessage(m_hWndWinamp, WM_COPYDATA,1,(LPARAM)&cds);

	return TRUE;
}

BOOL CWinampController::Play(string strFileName /*= ""*/, BOOL bKeepList /* = TRUE */)
{
	if(!strFileName.length())
	{
		if(!bKeepList)
		{
			ClearPlaylist();
			Enqueue(strFileName);
		}
		else
		{
			PLInsertTrack(strFileName, 0);
		}
	}

	::SendMessage(m_hWndWinamp, WM_COMMAND,WINAMP_BUTTON4, 0);
	::SendMessage(m_hWndWinamp, WM_WA_IPC,0,IPC_STARTPLAY);

	return TRUE;
}

string CWinampController::GetFileNameFromPos(int nPos)
{
	// What are we playing?
	string Text;
	char buffer[MAX_PATH];
	memset(buffer, 0, MAX_PATH);
	char* p = buffer;
	p = reinterpret_cast<char*>(SendMessage(m_hWndWinamp,WM_WA_IPC,nPos,IPC_GETPLAYLISTFILE));
	Text = p;
	//??? what were they thinking, returning off the stack.
	return Text;
}

BOOL CWinampController::IsPlaying()
{
	return (GetWinampState() == WA_STATE_PLAY);
}

BOOL CWinampController::IsPaused()
{
	return (GetWinampState() == WA_STATE_PAUSE);
}

// Returned in SECONDS
int	CWinampController::GetTrackLength()
{
	int nTrackLength = SendMessage(m_hWndWinamp,WM_WA_IPC,1,IPC_GETOUTPUTTIME);
	
	return nTrackLength;
}

//Returned in Milliseconds
int	CWinampController::GetPlayingTime()
{
	return SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETOUTPUTTIME);
}

string CWinampController::GetCurrTrackPath()
{
	int nPos = SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
	return GetFileNameFromPos(nPos);
}

int CWinampController::GetWinampState()
{
	// Are actually playing?
	int ret = SendMessage(m_hWndWinamp,WM_WA_IPC,0,IPC_ISPLAYING);
	if(ret == 1)
		return WA_STATE_PLAY;
	else if(ret == 3)
		return WA_STATE_PAUSE;
	else if(ret == 0)
		return WA_STATE_STOP;

	return -1;
}

BOOL CWinampController::IsPEWindowWinshade()
{
	RECT rcPEWnd;
	
	::GetWindowRect(m_hWndWinampPE, &rcPEWnd);

	if((rcPEWnd.bottom - rcPEWnd.top) < 116)
		return TRUE;
	
	return FALSE;
}

int CWinampController::GetVolume()
{
	return -1;
}

// Only works for Winamp 2.90 upwards
string CWinampController::GetCurrentSongMetadata(char* MetaData)
{
    char                    buffer[1024];
    char                    Filename[MAX_PATH];
    extendedFileInfoStruct  Info;
    LRESULT                 lRet = 0;

    memset(buffer, 0, sizeof(buffer));
	strcpy(Filename, GetCurrentSongFileName().c_str());

    Info.ret = buffer;
    Info.retlen = sizeof(buffer);
    Info.filename = Filename;
    Info.metadata = strdup(MetaData);

    lRet = SendMessage(m_hWndWinamp,WM_WA_IPC, (WPARAM)&Info, IPC_GET_EXTENDED_FILE_INFO);
/*    if ( lRet != 1)
    {
      PRINTF(DEBUG_DETAIL, 
              "CWinampController::GetCurrentSongMetadata", 
              "FAILED - metadata '%s', ret is %d", 
              MetaData,lRet);
    }*/
	//buggy ass code, no?
    return string(buffer);
}

BOOL CWinampController::GetCurrentSongInfo(MetaInfo* info)
{
	itemRecord	rec;
	char        Filename[MAX_PATH];
	int			Result = 0;
	BOOL		bRet = FALSE;			
	memset(&rec, 0, sizeof(itemRecord));
	strcpy(Filename,GetCurrentSongFileName().c_str());
	rec.filename = Filename;
	Result = SendMessage(m_hWndWinampML,WM_ML_IPC,(WPARAM)&rec,ML_IPC_GETFILEINFO);
	
	if(Result != 0)
	{

		//fuck these character strings.
		// Do Stuff
		
		
		DebugSpew(L"artist: %s\n",rec.artist);
		mbstowcs(info->wzAlbum, rec.album, META_FIELD_SIZE);
		mbstowcs(info->wzArtist, rec.artist,META_FIELD_SIZE);
		mbstowcs(info->wzTitle, rec.title, META_FIELD_SIZE);
		info->nLength = rec.length;
		info->nTrackNum = rec.track;

		Result = ::SendMessage(m_hWndWinampML,WM_ML_IPC,(WPARAM)&rec,ML_IPC_FREEFILEINFO);
		bRet = TRUE;
	}
	
	return bRet;
}

