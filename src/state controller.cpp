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
//perhaps move this in to the library...
#include <stdafx.h>
#include <constants.h>
#include <utilities.h>
#include <errors.h>
#include <gerpok.h>
#include <state controller.h>
#include "main.h"
typedef Loki::SingletonHolder<WAMetaRetriever, Loki::CreateUsingNew, Loki::PhoenixSingleton> WAMetaInfo;
LOKI_SINGLETON_INSTANCE_DEFINITION(WAMetaInfo)

/**
states which are able to change behavior without changing state:
Play:play
Pause:pause
*/
void WAStateMachine::setState(PWASTATE pState)
{
	if (pState)
		this->theState_ = pState;
}

void WAStateMachine::Stop()
{
	theState_->OnStop();
}

void WAStateMachine::Pause()
{
	theState_->OnPause();
}

void WAStateMachine::Play()
{
	theState_->OnPlay();
}

WAStateMachine::WAStateMachine()//MetaRetriever* pMetaRet) : pMetaRet_(pMetaRet)
{
	pGerpok_ = new Gerpok();
	paused_ = new WAPaused(this);
	playing_ = new WAPlay(this);
	stopped_ = new WAStop(this);
	theState_ = stopped_;
	DebugSpew(L"WAStateMachine::WAStateMachine()");
	
}

WAStateMachine::~WAStateMachine()
{
	delete paused_;
	delete playing_;
	delete stopped_;
	delete pGerpok_;
}

//BASE STATE
WAState::WAState(const PWASTATEMACHINE pMachine) : pMachine_(pMachine)
{
	//if (pMachine)
	//	this->pMachine_ = pMachine;
}

//PLAY

void WAPlay::OnPause()
{
	PWASTATEMACHINE pMachine = getMachine();
	DebugSpew(L"WAPlay::OnPause(Paused)");
	pMachine->GerpokPause();
	pMachine->setState(
		pMachine->getPauseState());
}

/*
classes

StateMachine
  |
  v
State

Gerpok
WAController
*/
/*
void WAPlay::OnPlay()
{
	DebugSpew(L"WAPlay::OnPlay(New Song)");
	MetaInfo* pMeta = new MetaInfo();
	
	pMachine_->GetMetaInfo(pMeta);
	try {
		pMachine_->GerpokPlay(pMeta);
	} catch (gpk::MalformedMetaInfo)
	{
		DebugSpew(L"Malformed meta info");
	}
	delete pMeta;
}
*/
void WAPlay::OnStop()
{ 
	DebugSpew(L"WAPlay::OnStop(Stopped)");
	PWASTATEMACHINE pMachine = getMachine();

	pMachine->GerpokStop();
	pMachine->setState(
		pMachine->getStopState());
}

//PAUSE
void WAPaused::OnPause()
{
	DebugSpew(L"WAPaused::OnPause(Unpaused)");
	pMachine_->GerpokPause();
	pMachine_->setState(
		pMachine_->getPlayState());
}

void WAPaused::OnPlay()
{
	DebugSpew(L"WAPaused::OnPlay(Playing Again)");
	
	pMachine_->GerpokPause();
	pMachine_->setState(
		pMachine_->getPlayState());
}

void WAPaused::OnStop()
{
	DebugSpew(L"WAPaused::OnStop(Stopped)");
	
	pMachine_->GerpokStop();
	pMachine_->setState(
		pMachine_->getStopState());
}

//STOP
void WAStop::OnPause()
{
}

void WAPlayable::OnPlay()
{
	//DebugSpew("WAStop::OnPlay()");

	pMachine_->setState(
		pMachine_->getPlayState());
	MetaInfo Meta;
	
	WAMetaInfo::Instance().CopyCurrentMetaInfo(Meta);
//	if (pMeta->nLength <= 0)
//		pMeta->nLength = g_Controller.GetCurrentSongLength();
	try {
		pMachine_->GerpokPlay(Meta);
	} catch (gpk::MalformedMetaInfo &e) {
		DebugSpew(L"Malformed meta info");
	}
}

void WAStop::OnStop()
{
}