typedef class WAState WASTATE, *PWASTATE;


__interface MetaRetriever
{
public:
	bool CopyCurrentMetaInfo(MetaInfo& Meta);
};


typedef class WAStateMachine
{
	PWASTATE paused_;
	PWASTATE playing_;
	PWASTATE stopped_;
	PWASTATE theState_;

	MetaRetriever* pMetaRet_;
	Gerpok* pGerpok_;

public:
	void Pause();
	void Play();
	void Stop();
	void setState(const PWASTATE state);
	const PWASTATE getPauseState() const { return paused_; };
	const PWASTATE getStopState() const { return stopped_; };
	const PWASTATE getPlayState() const { return playing_; };
	const Gerpok* getGerpok() const { return pGerpok_; };

	void GerpokPlay(MetaInfo &Meta) { 
		try {
			return pGerpok_->startSong(Meta);
		} catch (gpk::ConnectError)
		{
			DebugSpew(L"GerpokPlay() - caught ConnectError");
		} catch (...)
		{
			DebugSpew(L"GerpokPlay() - caught unknown exception");
		}
	};
	void GerpokStop() { 
		try {
			return pGerpok_->OnStop();
		} catch (gpk::ConnectError)
		{
			DebugSpew(L"GerpokStop() - caught ConnectError");
		} catch (...)
		{
			DebugSpew(L"GerpokStop() - caught unknown exception");
		}
	};
	void GerpokPause() { return pGerpok_->OnPause(); };
	//bool GetMetaInfo(MetaInfo& Meta) { return pMetaRet_->CopyCurrentMetaInfo(pMeta); };
	WAStateMachine();//MetaRetriever* pMetaRet);
	~WAStateMachine();

} WASTATEMACHINE, *PWASTATEMACHINE;




typedef class WAState
{
protected:
	PWASTATEMACHINE pMachine_;
public:
	WAState(const PWASTATEMACHINE pMachine);
	virtual void OnPause() = 0;
	virtual void OnPlay() = 0;
	virtual void OnStop() = 0;
} WASTATE, *PWASTATE;


class WAPaused : public WAState
{
public:
	WAPaused(const PWASTATEMACHINE pMachine) : WAState(pMachine) {};
	void OnPause();
	void OnStop();
	void OnPlay();
};

class WAPlayable : public WAState
{
protected:
	const PWASTATEMACHINE getMachine() const { return pMachine_; };
public:
	WAPlayable(const PWASTATEMACHINE pMachine) : WAState(pMachine) {};
	//virtual void OnPause() = 0;
	//virtual void OnStop() = 0;
	void OnPlay();
	
};

class WAPlay : public WAPlayable
{
public:
	WAPlay(const PWASTATEMACHINE pMachine) : WAPlayable(pMachine) {};
	void OnPause();
	void OnStop();
//	void OnPlay();
};

class WAStop : public WAPlayable
{
public:
	WAStop(const PWASTATEMACHINE pMachine) : WAPlayable(pMachine) {};
	void OnPause();
	void OnStop();
//	void OnPlay();
};
