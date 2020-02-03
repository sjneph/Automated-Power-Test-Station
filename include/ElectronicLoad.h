// Macro Guard
#ifndef SPTS_ELECTRONICLOAD_H
#define SPTS_ELECTRONICLOAD_H

// Files included
#include "GenericAlgorithms.h"
#include "Instrument.h"
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

// Electronic Load
struct ElectronicLoad 
    : private Instrument<LoadTraits::ModelType::BusType>, private NoCopy {

    //========================
    // START PUBLIC INTERFACE
    //========================
	// Constructor and Destructor
	ElectronicLoad();
    ~ElectronicLoad();

	// Public Enums and Typedefs
	enum { MAXSLEW = -1 };
    typedef ProgramTypes::MType MType;
    typedef ProgramTypes::SetType SetType;

    // Public Member Functions
	bool AddLoadChannel(LoadTraits::Channels chan, const SetType& maxAmps, 
		      const SetType& maxVolts, const SetType& minOhms, const SetType& maxOhms);
	void Concatenate(Switch state);
	Switch CurrentState(LoadTraits::Channels channel);
    ProgramTypes::SetType GetLoadValue(LoadTraits::Channels channel);
	LoadTraits::Modes GetMode(LoadTraits::Channels chan);
    void ImmediateMode();
	bool Initialize();
	bool IsError();
	bool LoadOff(LoadTraits::Channels channel);
	bool LoadOn(LoadTraits::Channels channel);
	SetType MaxAmps(LoadTraits::Channels channel) const;
	SetType MaxVolts(LoadTraits::Channels channel) const;
    MType MeasureCurrent(LoadTraits::Channels channel);
    MType MeasureVoltage(LoadTraits::Channels channel);
    std::pair<SetType, SetType> MinMaxOhms(LoadTraits::Channels channel) const;
	std::string Name();
	bool OpsComplete();
	bool Reset();
	void ResetParallelLoads();
	void ResetXSTStates(LoadTraits::Channels channel);
	void SetLoad(LoadTraits::Channels channel, const SetType& value); 
	bool SetMode(LoadTraits::Channels channel, LoadTraits::Modes mode);
	LoadTraits::Channels 
           SetParallelLoads(LoadTraits::Channels chA, LoadTraits::Channels chB);
	void SetXSTStates(LoadTraits::Channels channel, const SetType& from, 
		               const SetType& to, const SetType& rate = MAXSLEW);
	void TriggerXSTEvent();
	std::string WhatError();
    //======================
    // END PUBLIC INTERFACE
    //======================

private: // Private typedefs
	typedef LoadTraits::ModelType LoadType;
    typedef LoadType::BusType BT;
    typedef LoadType::Language Language;
	typedef ProgramTypes::SetType SetType;
	typedef LoadType::Language Language;
	typedef Switch Switch;
	typedef LoadTraits::Modes Mode;
	typedef LoadTraits::Channels Channels;

private: // Private Helpers
	bool bitprocess(const std::string& errorString, Instrument<BT>::Register toCheck);
	bool command();
	std::string query();

public:
	class ElectronicLoadChannel;

private:
	// Friends and typedefs
	friend class ElectronicLoadChannel;
	typedef std::map<LoadTraits::Channels, ElectronicLoadChannel> LoadMap;
    typedef LoadMap::iterator LoadMapIterator;
    typedef LoadMap::const_iterator ConstLoadMapIterator;

private: 
	// data members
    std::auto_ptr<LoadMap> loadMap_;
	bool concatenate_;
	bool locked_;
	std::string syntax_;
	std::string totalSyntax_;
	long toggle_;
	std::string lastError_;
	long address_;
    std::string name_;
};

} // namespace SPTSInstrument 

#endif  // SPTS_ELECTRONICLOAD_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/



/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
