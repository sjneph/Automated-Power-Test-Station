// Macro Guard
#ifndef SPTS_RESISTIVELOAD_H
#define SPTS_RESISTIVELOAD_H


// Files included
#include "ControlMatrix.h"
#include "ControlMatrixTraits.h"
#include "Converter.h"
#include "LoadTraits.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "Switch.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

class ResistiveLoad : private Instrument<ControlMatrixTraits::ModelType::BusType>, 
                      private NoCopy {

protected:
	typedef ProgramTypes::SetType SetType;
	typedef Switch Switch;
	typedef LoadTraits::Channels Channels;
	typedef std::map<Channels, SetType> LoadValueMap; 
	typedef std::map<Channels, Switch>  LoadStateMap; 
	typedef std::map< Channels, std::pair<SetType, SetType> > LoadTransMap;
	typedef std::map<SetType, ControlMatrix::RLL::RLLContainer> InnerRLLMap;
	typedef std::map<Channels, InnerRLLMap> RLLMap;
	typedef std::map<Channels, bool> XSTMap;
	typedef ControlMatrix::RLL RLLs;

public:
	// Public Typedefs
	typedef ControlMatrix::RLL::RLLContainer RLLContainer;

    // Constructor
	explicit ResistiveLoad();

	// Public Interface
	void Concatenate(Switch state);
	Switch CurrentState(LoadTraits::Channels channel) const;
    ProgramTypes::SetType GetLoadValue(LoadTraits::Channels channel);
    void ImmediateMode();
	bool Initialize();
    void InitializeAllRLLs(const RLLContainer& all);
	bool IsError() const;
	bool LoadOff(LoadTraits::Channels channel);
	bool LoadOn(LoadTraits::Channels channel);
	SetType MaxAmps(LoadTraits::Channels chan) const;
    std::string Name() const;
    bool OpsComplete() const;
	bool Reset();
	void ResetXSTStates(LoadTraits::Channels channel);
	void SetLoad(LoadTraits::Channels channel, const SetType& value); 
	void SetXSTStates(LoadTraits::Channels channel, const SetType& from, 
		               const SetType& to);
	void TriggerXSTEvent();
	std::string WhatError() const;

private:	
    bool addLoadChannel(ConverterOutput::Output chan, const InnerRLLMap& r);
	void checkMaps();
	void initializeLoadValues();

private:
	std::auto_ptr<ControlMatrix::RLL> loadControl_;
	std::auto_ptr<LoadValueMap> loadValues_;
	std::auto_ptr<LoadStateMap> loadStates_;
	std::auto_ptr<LoadTransMap> loadTrans_;
	std::auto_ptr<XSTMap> activeTrans_;
	std::auto_ptr<RLLMap> rllMap_;
	bool isXST_;
	bool nextXST_;
	bool locked_;
};

} // namespace SPTSInstrument

#endif // SPTS_RESISTIVELOAD_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
