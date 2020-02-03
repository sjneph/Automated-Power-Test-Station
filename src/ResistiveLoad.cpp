// Files included
#include "Assert.h"
#include "ConfigureRelays.h"
#include "Converter.h"
#include "ResistiveLoad.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::ContainerState  ContainerState;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//=============================
// ResistiveLoad Constructor() 
//=============================
ResistiveLoad::ResistiveLoad() 
    : loadControl_(0), loadValues_(new LoadValueMap), loadStates_(new LoadStateMap),
	  loadTrans_(new LoadTransMap), activeTrans_(new XSTMap), rllMap_(new RLLMap), 
	  isXST_(false), nextXST_(false), locked_(true) 
{ /* */ }


//==================
// addLoadChannel()
//==================
bool ResistiveLoad::addLoadChannel(ConverterOutput::Output chan, const InnerRLLMap& r) {
	LoadTraits::Channels c;
	SetType zero = static_cast<SetType>(0);

	switch(chan) {
		case ConverterOutput::ONE:
			c = LoadTraits::ONE;
			break;
		case ConverterOutput::TWO:
			c = LoadTraits::TWO;
			break;
		case ConverterOutput::THREE:
			c = LoadTraits::THREE;
			break;
		case ConverterOutput::FOUR:
			c = LoadTraits::FOUR;
			break;
		case ConverterOutput::FIVE:
			c = LoadTraits::FIVE;
			break;
		default:
			throw BadArg();
	};
    rllMap_->insert(std::make_pair(c, r));
	loadValues_->insert(std::make_pair(c, zero));
	loadStates_->insert(std::make_pair(c, OFF));
	loadTrans_->insert(std::make_pair(c, std::make_pair(zero, zero)));
	activeTrans_->insert(std::make_pair(c, false));
    return(true);
}

//=============
// checkMaps()
//=============
void ResistiveLoad::checkMaps() {
    typedef ContainerState CS;
	Assert<CS>(rllMap_->size() == loadValues_->size(), Name());
    Assert<CS>(rllMap_->size() == loadStates_->size(), Name());
    Assert<CS>(rllMap_->size() == loadTrans_->size(), Name());
    Assert<CS>(rllMap_->size() == activeTrans_->size(), Name());

	RLLMap::iterator s = rllMap_->begin(), t = rllMap_->end();
	Assert<CS>(s != t, Name());
	while ( s != t ) {
		Assert<CS>((loadValues_->find(s->first) != loadValues_->end()), Name());
        Assert<CS>((loadStates_->find(s->first) != loadStates_->end()), Name());
		Assert<CS>((loadTrans_->find(s->first) != loadTrans_->end()), Name());
		Assert<CS>((activeTrans_->find(s->first) != activeTrans_->end()), Name());
		++s;
	}
}

//===============
// Concatenate()
//===============
void ResistiveLoad::Concatenate(Switch state) {
	switch(state) {
		case ON:
			loadControl_->Concatenate(ON);
			break;
		case OFF:
			loadControl_->Concatenate(OFF);
	};
}

//================
// CurrentState()
//================
Switch ResistiveLoad::CurrentState(LoadTraits::Channels channel) const {
	LoadStateMap::iterator Sfound = loadStates_->find(channel);
	Assert<ContainerState>(Sfound != loadStates_->end(), Name());
	return(Sfound->second);
}

//================
// GetLoadValue()
//================
ProgramTypes::SetType ResistiveLoad::GetLoadValue(LoadTraits::Channels channel) {
    LoadValueMap::iterator found = loadValues_->find(channel);
	Assert<ContainerState>(found != loadValues_->end(), Name());
	return(found->second);
}

//=================
// ImmediateMode()
//=================
void ResistiveLoad::ImmediateMode() {
    loadControl_->ImmediateMode();
}

//==============
// Initialize()
//==============
bool ResistiveLoad::Initialize() {
	Assert<UnexpectedState>(locked_, Name());
	isXST_ = false;
	initializeLoadValues();
	checkMaps();
	locked_ = false;
	loadControl_->Initialize(); // go to no load 
    return(true);
}

//========================
// initializeLoadValues()
//========================
void ResistiveLoad::initializeLoadValues() {
	Assert<UnexpectedState>(locked_, Name());
	ConverterOutput::Output num 
                 = SingletonType<Converter>::Instance()->NumberOutputs();
	RLLContainer hold; // needs to be a unique container like std::set<>
    long numOuts = static_cast<long>(num);
    ConverterOutput::Output aOut; 
	for ( long a = 1; a <= numOuts; ++a ) {
        aOut = static_cast<ConverterOutput::Output>(a);
        VariablesFile::InnerMap tmp = 
                  SingletonType<VariablesFile>::Instance()->RLoads(aOut);
		Assert<ContainerState>(!tmp.empty(), Name());
        VariablesFile::InnerMap::iterator start = tmp.begin();
        InnerRLLMap toStore;
        while ( start != tmp.end() ) {
            RLLContainer holdtmp;
            VariablesFile::RLLSet::iterator i = start->second.begin();
            VariablesFile::RLLSet::iterator j = start->second.end();
            while ( i != j ) {
                holdtmp.insert(ConvertToRLL(*i));
                ++i;
            }
            toStore.insert(std::make_pair(start->first, holdtmp));
            ++start;
        } // while
        
		InnerRLLMap::iterator beg = toStore.begin();
		while ( beg != toStore.end() ) {
			std::copy(beg->second.begin(), 
				      beg->second.end(), 
					  std::inserter(hold, hold.begin()));
			++beg;
		} // while
		addLoadChannel(aOut, toStore);
	} // for-loop
	loadControl_.reset(new ControlMatrix::RLL(hold));
}

//=====================
// InitializeAllRLLs()
//=====================
void ResistiveLoad::InitializeAllRLLs(const RLLContainer& all) {
    // This routine must be called before Initialize() or after Reset()
    Assert<UnexpectedState>(locked_, Name()); 

    loadControl_.reset(new ControlMatrix::RLL(all));
    loadControl_->Initialize(); // ensure all lines are initialized
    loadControl_.reset(0);
}

//===========
// IsError()
//===========
bool ResistiveLoad::IsError() const {
    return(loadControl_->IsError());
}

//===========
// LoadOff()
//===========
bool ResistiveLoad::LoadOff(LoadTraits::Channels channel) {
	LoadStateMap::iterator found = loadStates_->find(channel);
	Assert<ContainerState>(found != loadStates_->end(), Name());
	bool isOff = true;
	if ( found->second != OFF ) {
		RLLMap::iterator check = rllMap_->find(channel);
		InnerRLLMap::iterator start = check->second.begin();
		RLLContainer lines; // must be a unique-type container, like std::set
		while ( start != check->second.end() ) {
            RLLContainer::iterator next = start->second.begin();
            while ( next != start->second.end() ) {
                lines.insert(*next);
                ++next;
            }
			++start;
		}
        isOff = loadControl_->TurnOff(lines);
		if ( isOff ) 
			found->second = OFF;
	} 
	return(isOff);
}

//==========
// LoadOn()
//==========
bool ResistiveLoad::LoadOn(LoadTraits::Channels channel) {
    LoadStateMap::iterator found = loadStates_->find(channel);
	Assert<ContainerState>(found != loadStates_->end(), Name());
	bool isOn = true;
	if ( found->second != ON ) {
        SetType value = loadValues_->find(channel)->second;
        RLLContainer r = rllMap_->find(channel)->second.find(value)->second;
        isOn = loadControl_->TurnOn(r); // loadValues_ already set --> do not update
	    if ( isOn ) 
		    found->second = ON;
	}
    return(isOn);
}

//===========
// MaxAmps()
//===========
ResistiveLoad::SetType ResistiveLoad::MaxAmps(LoadTraits::Channels) const {
	RLLMap::const_iterator it = rllMap_->begin();
	SetType s = it->first;
	while ( ++it != rllMap_->end() ) {
		if ( it->first > s )
			s = it->first;
	}
	return(s);
}

//========
// Name()
//========
std::string ResistiveLoad::Name() const {
	return("Passive Load");
}

//===============
// OpsComplete()
//===============
bool ResistiveLoad::OpsComplete() const {
    return(loadControl_->OpsComplete());
}

//=========
// Reset()
//=========
bool ResistiveLoad::Reset() {
	locked_ = true;
	return(Initialize());
}

//==================
// ResetXSTStates()
//==================
void ResistiveLoad::ResetXSTStates(LoadTraits::Channels channel) {
	LoadTransMap::iterator start = loadTrans_->find(channel);
	if ( start == loadTrans_->end() ) // not set 
        return;
   
    bool notReset = isXST_; 
    isXST_ = false; // need to get passed SetLoad's Assertions
    try {
	    SetLoad(channel, start->second.first);
    } catch(...) {
        isXST_ = notReset; // restore
        throw;
    }
    isXST_ = notReset; // restore
	start->second.second = start->second.first; // to, from are the same now

	activeTrans_->find(channel)->second = false;
	XSTMap::iterator start2 = activeTrans_->begin();
	notReset = false;
	while ( start2 != activeTrans_->end() ) {
         if ( start2->second ) // not all reset
			 notReset = true;
		 ++start2;
	}
	isXST_ = notReset;
}

//===========
// SetLoad()
//===========
void ResistiveLoad::SetLoad(LoadTraits::Channels channel, const SetType& value) {
	// SetLoad cannot be called if isXSt_ is true for any channel
	Assert<UnexpectedState>((! isXST_) && (value >= 0), Name());
	LoadValueMap::iterator found = loadValues_->find(channel);
	Assert<ContainerState>(found != loadValues_->end(), Name());

	LoadStateMap::iterator state = loadStates_->find(channel);
    if ( value == SetType(0) ) { // turn off
        LoadOff(channel);
        return;
    }

	if ( value == found->second ) {
		if ( state->second == ON ) // already there
		    return;
	}

	// Don't try to turn "load input on" --> concatenation will hurt you
	// It doesn't make sense here --> the code below will work fine without
	// that notion.
    // Not the most efficient implementation, but ResistiveLoads should
    // not be used very often.
	
    // Make sure containers know what channel/value are
    Assert<BadArg>(rllMap_->find(channel) != rllMap_->end());
    Assert<BadArg>(rllMap_->find(channel)->second.find(value) != 
                   rllMap_->find(channel)->second.end(), Name());

    // Initialize variables
    RLLContainer Off;
    InnerRLLMap& map = rllMap_->find(channel)->second;
    RLLContainer& nextLoad = map.find(value)->second;

    if ( loadStates_->find(channel)->second != OFF ) { // something to change, possibly
        typedef BadArg BA;

        Assert<BA>(map.find(loadValues_->find(channel)->second) != map.end(), Name());
        RLLContainer& currentLoad = map.find(loadValues_->find(channel)->second)->second;
        RLLContainer::iterator start = currentLoad.begin();
        RLLContainer::iterator end   = currentLoad.end();

        // If a RLL is 'ON' currently, but not needed for new load value, then
        //  stuff in (Off) and turn it 'OFF'
        while ( start != end ) {        
            if ( nextLoad.find(*start) == nextLoad.end() )
                Off.insert(*start);
            ++start;
        }
        if ( ! Off.empty() )
            loadControl_->TurnOff(Off);
    }

    // Turn all necessary RLL's 'ON' to satisfy new load value
    Assert<UnexpectedState>(loadControl_->TurnOn(nextLoad), Name());
    state->second = ON;
    found->second = value;
}

//================
// SetXSTStates()
//================
void ResistiveLoad::SetXSTStates(LoadTraits::Channels channel, 
								 const SetType& from, const SetType& to) {
	Assert<BadArg>((from >= 0) && (to >= 0), Name());
	LoadTransMap::iterator found = loadTrans_->find(channel);
	Assert<ContainerState>(found != loadTrans_->end(), Name());
	found->second.first  = from;
	found->second.second = to;
	activeTrans_->find(channel)->second = true;
	isXST_ = true;
}

//===================
// TriggerXSTEvent()
//===================
void ResistiveLoad::TriggerXSTEvent() {

	//----------------------------------------------------------------------------
    // You are forced to call SetXSTStates() immediately before TriggerXSTEvent()
	//  Subsequent, consecutive calls to this routine are OK too.
	//----------------------------------------------------------------------------
	Assert<UnexpectedState>(isXST_ && (! loadControl_->IsConcatenate()), Name());
	nextXST_ = (! nextXST_);  // toggle commands

	// Set initial load value(s) --> we may not be there yet.  If so, nothing happens
	Concatenate(ON);
	isXST_ = false; // get by SetLoad() requirements
	LoadTransMap::iterator start = loadTrans_->begin(), stop = loadTrans_->end();
	while ( start != stop ) {
		if ( activeTrans_->find(start->first)->second ) { // do trans on this chan
		    nextXST_ ?
			    SetLoad(start->first, start->second.first) :
		        SetLoad(start->first, start->second.second);
		}
		++start;
	} // while
	Concatenate(OFF);
	start = loadTrans_->begin();
	
	// Go to next load value(s)
	Concatenate(ON);
	while ( start != stop ) {
		if ( activeTrans_->find(start->first)->second ) { // do trans on this chan
		    nextXST_ ? 
			    SetLoad(start->first, start->second.second):
			    SetLoad(start->first, start->second.first) ; 
		}
		++start;
	} // while
	Concatenate(OFF);
	isXST_ = true;  // restore this information
} 

//=============
// WhatError()
//=============
std::string ResistiveLoad::WhatError() const {
	return(loadControl_->WhatError());
}

} // namespace SPTSInstrument

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
