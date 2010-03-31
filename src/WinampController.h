/*##############################################
##
## Module				: CWinampController
## Description	: This class takes total control
##								of Winamp 1.x - 2.x
##
## Author(s)		: Spib
##
## Modifications
##
##############################################*/
#pragma once

#include "Gen.h"
#include "wa_ipc.h"
#include "WinampCmd.h"
#include "ml.h"



#define WINAMP_MENU_OPTIONS_POS		11
#define WINAMP_MENU_REPEAT			40022
#define WINAMP_MENU_SHUFFLE			40023
#define WINAMP_MAIN_WINDOW          40258

#define WINAMP_MV_PE_HLT_UP			105825
#define WINAMP_MV_PE_HLT_DOWN		105826

#define WINAMP_DEL_FRM_PE			66570
#define WINAMP_MV_PE_ITEM_DOWN		105752
#define WINAMP_MV_PE_ITEM_UP		105751

#define WINAMP_PE_SORT_TITLE		40209
#define WINAMP_PE_SORT_FNAME		40210
#define WINAMP_PE_SORT_FNAME_PATH	40211

#define WA_STATE_PLAY	100
#define WA_STATE_PAUSE	101
#define WA_STATE_STOP	102

//PE Stuff
#define IPC_PE_GETCURINDEX			100 
#define IPC_PE_GETINDEXTOTAL		101 
#define IPC_PE_GETINDEXINFO			102 
#define IPC_PE_GETINDEXINFORESULT	103 
#define IPC_PE_DELETEINDEX			104 
#define IPC_PE_SWAPINDEX			105 
#define IPC_PE_INSERTFILENAME		106 
#define IPC_PE_GETDIRTY				107 
#define IPC_PE_SETCLEAN				108 
/*
typedef struct
{
	string	strArtist;
	string	strAlbum;
	string	strComment;
	string	strGenre;
	string	strTitle;
	string	strFileName;
	int			nYear;
	int			nLength;
	int			nTrackNo;
}SongInfo;
*/
/*
#define META_FIELD_SIZE 1024

typedef struct MetaInfo {
private:
	//album name
	wchar_t m_szAlbum[META_FIELD_SIZE];
	//artist name
	wchar_t m_szArtist[META_FIELD_SIZE];
	//track title
	wchar_t m_szTrack[META_FIELD_SIZE];
	//track number in album
	int			m_trackNum;
	// track len in seconds 
	int			m_length;
	//unix time that song was played
	time_t				m_playTime;
	//song id, nmakes sure the song is valid
	unsigned long m_id;


} METAINFO, *MetaInfo*;
*/

/*
typedef struct
{
	int nWinampRuns;			// How many times Winamp has been run.
	CComDATE tRunning;			// How long Winamp has been running. 
	CComDATE tRunningMB;			// How long Winamp has been running with the Minibrowser window open. 
	CComDATE tRunningNoneMin;		// How long Winamp has been running in a non minimized state. 
	CComDATE tRunningPlay;			// How long Winamp has been running and playing audio files. 
	CComDATE tRunningPlayNonMin;	// How long Winamp has been running, playing, in a non minimized state.
}WA_STATS;
*/
class CWinampController  
{
public:
	CWinampController();
	virtual ~CWinampController();

	BOOL	Init(BOOL bStartWinamp, HWND Winamp = NULL, HWND WinampML = NULL);

	HWND	GetWinampWnd(){return m_hWndWinamp;}
	HWND    GetPEWnd(){return m_hWndWinampPE;}
	//BOOL	GetWinampStats(WA_STATS& stats);

	//Main Window - Operation
	BOOL	Play(string strFile = "", BOOL bKeepList = FALSE);
	BOOL	Enqueue(string strFile);
	void	Stop();
	void	Pause();
	void	FastForward();
	void	NextTrack();
	void	PrevTrack();
	void	Rewind();
	void	FastFwd5Secs();
	void	Rewind5Secs();
	void	CloseWinamp();
	void	RestartWinamp();
	void	IncreaseVolume();
	void	DecreaseVolume();

	BOOL	GetShuffleStatus();
	void	SetShuffleStatus(BOOL newVal);
	BOOL	GetRepeatStatus();
	void	SetRepeatStatus(BOOL newVal);
	HMENU	GetOptionMenu();
	long	GetVersion();
	long	GetStatus();
	void	SetStatus(long newVal);
	long	GetCurrentPos();
	void	SetCurrentPos(long newVal);
	void	SetVolume(long newVal);
	void	SetPanning(long PanIndex);
	void	ChangeDirectory(string NewDir);
	string GetSkinName();
	void	SetSkinName(string Name);
	void	StartPlugIn(string PlugInName);
	void	ToggleShufflePlay();
	void	ToggleRepeatPlay();
	void	SongChange();
	long	GetCurrentSongLength();
	BOOL	GetWinampVisible();
	void	SetWinampVisible(BOOL newVal);

	//Song Information
	long	GetSampleRate();
	long	GetBitrate();
	long	GetChannels();
	string GetSongFileName(long PlayListPos);
	string GetSongTitle(long PlayListPos);
    string GetSongMetaData(string MetaData, long PlayListPos);
	string GetCurrentSongFileName();
    string GetCurrentSongTitle();
	string GetCurrentSongMetadata(char* MetaData);
	BOOL		GetCurrentSongInfo(MetaInfo* Info);

	//Playlist Functions
	void	ClearPlaylist();
	long	GetPlayListPos();
	void	SetPlayListPos(long newVal);
	long	GetPlayListCount();
	void	DeletePEItem(int Pos);
	void	DeletePECurHighlight();
	void	MovePEHighlight(int Pos);
	bool	MovePEItem(int curPos, int NewPos);
	BOOL	GetPlaylistVisible();
	void	SetPlaylistVisible(BOOL newVal);
	void	JumpToTrack(unsigned int Number);
	void	SortByName(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_TITLE,0);}
	void	SortByFName(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_FNAME,0);}
	void	SortByFNamePath(){SendMessage(m_hWndWinampPE, WM_COMMAND, WINAMP_PE_SORT_FNAME_PATH,0);}
	void	PLInsertTrack(string strFile, int nPos);
	BOOL	IsPEWindowWinshade();
	
	// Equaliser Functions
	long	GetEqPosition(long Band);
	void	SetEqPosition(long Band, long newVal);
	long	GetPreAmpPosition();
	void	SetPreAmpPosition(long newVal);
	BOOL	GetEqualizerVisible();
	void	SetEqualizerVisible(BOOL newVal);
	BOOL	GetEqEnabled();
	void	SetEqEnabled(BOOL newVal);
	BOOL	GetAutoloadEnabled();
	void	SetAutoloadEnabled(BOOL newVal);

	//Minibrowser stuff
	BOOL	GetMiniBrowserVisible();
	void	SetMiniBrowserVisible(BOOL newVal);
	void	SetMiniBrowserURL(string strUrl, BOOL bForceOpen);
	BOOL  IsInetAvailable();

	int		GetTrackLength();
	int		GetPlayingTime();
	string GetCurrTrackPath();
	int		GetWinampState();
	string GetWinampDir();

	BOOL	IsPlaying();
	BOOL	IsPaused();
	string	GetPlayerVersion();
	void	SetVolume(int nValue);
	int     GetVolume();
	string GetFileNameFromPos(int nPos);

protected:
	BOOL	WindowsReady();
	void	SendCommand(long command);
	
	HWND	m_hWndWinamp;
	HWND	m_hWndWinampEQ;
	HWND	m_hWndWinampPE;
	HWND	m_hWndWinampMB;
	HWND	m_hWndWinampML;
	int		m_nVolume;
	string	m_strVersionString;		// Winamp Version String
	string	m_strWinampDir;			// Winamp Directory
};