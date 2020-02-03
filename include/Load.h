// Macro Guard
#ifndef SPTS_LOADWRAPPER_H
#define SPTS_LOADWRAPPER_H

// Files included
#include "Assert.h"
#include "ControlMatrix.h"
#include "ElectronicLoad.h"
#include "ProgramTypes.h"
#include "ResistiveLoad.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardFiles.h"
#include "Switch.h"



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct Load : private NoCopy {

    //========================
	// Start Public Interface
    //========================
	Load();	
    bool AddLoadChannel(LoadTraits::Channels chan);
	void Concatenate(Switch state);
    std::pair<Switch, ProgramTypes::SetType> 
                                      GetLoadValue(LoadTraits::Channels channel);
	LoadTraits::Modes GetMode(LoadTraits::Channels chan) const; 
    void ImmediateMode();
	bool Initialize();
	bool IsError() const;
	Switch LoadState(LoadTraits::Channels channel) const;
	LoadTraits::Types LoadType() const;     
	ProgramTypes::SetType MaxCurrent(LoadTraits::Channels chan) const;
	ProgramTypes::SetType MaxVolts(LoadTraits::Channels chan) const;
    ProgramTypes::MType MeasureAmps(LoadTraits::Channels chan);
    ProgramTypes::MType MeasureVolts(LoadTraits::Channels chan);
    std::pair<ProgramTypes::SetType, ProgramTypes::SetType> 
                                        MinMaxOhms(LoadTraits::Channels chan) const;
    std::string Name() const;
	void Off(LoadTraits::Channels channel);
	void On(LoadTraits::Channels channel);
	bool OperationComplete() const;
	LoadTraits::Channels ParallelLoads(LoadTraits::Channels ChA, 
		                               LoadTraits::Channels ChB);
	bool Reset();
	void ResetParallelLoads();
	void SetLoadValue(LoadTraits::Channels channel, 
		              const ProgramTypes::SetType& value);
	bool SetMode(LoadTraits::Channels chan, LoadTraits::Modes mode);
	void SetTransient(LoadTraits::Channels chan, 
		              const ProgramTypes::SetType& A,
		              const ProgramTypes::SetType& B, 
					  const ProgramTypes::SetType& Rate);
    void SetTransient(LoadTraits::Channels chan,
                      const ProgramTypes::SetType& A,
                      const ProgramTypes::SetType& B);
	void TransientOff(LoadTraits::Channels chan);
	void TransientOn();
	std::string WhatError() const;
    ~Load();
    //======================
	// End Public Interface
    //======================

private:
    bool isEL() const;
	std::string name() const;
    typedef ProgramTypes::SetType SetType;

private:
	std::auto_ptr<ResistiveLoad> rl_;
	std::auto_ptr<ElectronicLoad> el_;
};

} // namespace SPTSInstrument

#endif // SPTS_LOADWRAPPER_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
