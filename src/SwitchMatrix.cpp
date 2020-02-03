// Files included
#include "GenericAlgorithms.h"
#include "StandardStationFiles.h"
#include "SwitchMatrix.h"
#include "Switch.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   05/23/05, sjn,
   ==============
     Modified bitprocess() overload --> made robust against differing size registers.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::ContainerState  ContainerState;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
} // unnamed

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//===============
// Constructor()
//===============
SwitchMatrix::SwitchMatrix(const RFRelayContainer& contRF, 
                           const DCRelayContainer& contDC,
                           const RFFilterContainer& contFilter)
    : addressRF_(-1), 
      addressDC_(-1),
      addressFilt_(-1),
      locked_(true), 
      rf_(new RFRelayMap),
      dc_(new DCRelayMap),
      filter_(new FilterRelayMap),
      customResetDC_(new DCRelayMap),
      customResetRF_(new RFRelayMap),
      customResetFilt_(new FilterRelayMap)  {

    Assert<BadArg>(!contRF.empty(), nameRF());
    Assert<BadArg>(!contDC.empty(), nameDC());
    Assert<BadArg>(!contFilter.empty(), nameFilter());
	RFRelayContainer::const_iterator startRF = contRF.begin(), stopAC = contRF.end();
	while ( startRF != stopAC ) 
		rf_->insert(std::make_pair(*startRF++, OFF));

    DCRelayContainer::const_iterator startDC = contDC.begin(), stopDC = contDC.end();
    while ( startDC != stopDC ) 
		dc_->insert(std::make_pair(*startDC++, OFF));

    RFFilterContainer::const_iterator startFilt = contFilter.begin();
    while ( startFilt != contFilter.end() )
        filter_->insert(std::make_pair(*startFilt++, OFF));

    addressRF_   = Instrument<BT>::getAddress(InstrumentFile::SWITCHMATRIXRF);
    addressDC_   = Instrument<BT>::getAddress(InstrumentFile::SWITCHMATRIXDC);    
    addressFilt_ = Instrument<BT>::getAddress(InstrumentFile::SWITCHMATRIXFILTER);
}

//==============
// Destructor()
//==============
SwitchMatrix::~SwitchMatrix() 
{ /* */ }

//==============
// bitprocess()
//==============
bool SwitchMatrix::bitprocess(const std::string& errorString,
                              Instrument<BT>::Register toCheck) {
	// Convert eString to binary representation
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(!eString.empty(), Name());
	NumberBase conversion(eString, 
                            static_cast<NumberBase::Base>(MatrixType::BitReturnTypes));
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= MatrixType::TotalRegisterBits, Name());
    std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = Language::BITDELIMMITTER;
    std::vector<std::string> eBits, oBits;
    std::set<std::string> eSet, oSet;
    std::vector<std::string> inter;
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
            eBits = SplitString(Language::ErrorBits(), delim);
            eSet.insert(eBits.begin(), eBits.end());            
            std::set_intersection(eSet.begin(), eSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
			break;
		default:   // OPSCOMPLETE --> if done, then return true
            oBits = SplitString(Language::OpCompleteBits(), delim);
            oSet.insert(oBits.begin(), oBits.end());
            std::set_intersection(oSet.begin(), oSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
	}; // Switch
	return(!inter.empty());
}

//================
// ChangedState()
//================
bool SwitchMatrix::ChangedState(RelayType type) {
    if ( type == RF )
        return((*customResetRF_) != (*rf_));
    if ( type == FILTER )
        return((*customResetFilt_) != (*filter_));
    if ( type == DC )
        return((*customResetDC_) != (*dc_));
    throw(BadArg(Name()));
}

//===================
// Close() Overload1
//===================
bool SwitchMatrix::Close(RFRelay relay) {
    RFRelayMap::iterator find = rf_->find(relay);
    Assert<ContainerState>(find != rf_->end(), nameRF());
    if ( find->second == ON ) // already there
        return(false);
    std::string syntax = Language::Close(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::RF), nameRF());
    find->second = ON;
    return(true);
}

//===================
// Close() Overload2
//===================
bool SwitchMatrix::Close(const RFRelayContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameRF());
    RFRelayContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    RFRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = rf_->find(*start);
        start++;
        Assert<ContainerState>(find != rf_->end(), nameRF());
        if ( firstLoop && (find->second != ON) ) {
            syntax = Language::Close(find->first);
            firstLoop = false;
        }
        else if ( find->second != ON ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Close(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::RF), nameRF());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = rf_->find(*start);
            find->second = ON;
            ++start;
        }        
    }
    return(actionDone);
}

//===================
// Close() Overload3
//===================
bool SwitchMatrix::Close(DCRelay relay) {
    DCRelayMap::iterator find = dc_->find(relay);
    Assert<ContainerState>(find != dc_->end(), nameDC());
    if ( find->second == ON ) // already there
        return(false);
    std::string syntax = Language::Close(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::DC), nameDC());
    find->second = ON;    
    return(true);
}

//===================
// Close() Overload4
//===================
bool SwitchMatrix::Close(const DCRelayContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameDC());
    DCRelayContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    DCRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = dc_->find(*start);
        ++start;
        Assert<ContainerState>(find != dc_->end(), nameDC());
        if ( firstLoop && (find->second != ON) ) {
            syntax = Language::Close(find->first);
            firstLoop = false;
        }
        else if ( find->second != ON ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Close(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::DC), nameDC());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = dc_->find(*start);
            find->second = ON;
            ++start;
        }
    }
    return(actionDone);
}

//===================
// Close() Overload5
//===================
bool SwitchMatrix::Close(FilterRelay relay) {
    FilterRelayMap::iterator find = filter_->find(relay);
    Assert<ContainerState>(find != filter_->end(), nameFilter());
    if ( find->second == ON ) // already there
        return(false);
    std::string syntax = Language::Close(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::FILTER), nameFilter());
    find->second = ON;    
    return(true);
}

//===================
// Close() Overload6
//===================
bool SwitchMatrix::Close(const RFFilterContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameFilter());
    RFFilterContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    FilterRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = filter_->find(*start);
        ++start;
        Assert<ContainerState>(find != filter_->end(), nameFilter());
        if ( firstLoop && (find->second != ON) ) {
            syntax = Language::Close(find->first);
            firstLoop = false;
        }
        else if ( find->second != ON ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Close(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::FILTER), nameFilter());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = filter_->find(*start);
            find->second = ON;
            ++start;
        }
    }
    return(actionDone);
}

//===========
// command()
//===========
bool SwitchMatrix::command(const std::string& cmd, RelayType type) {
	Assert<UnexpectedState>(!(locked_ || cmd.empty()), Name());
    switch(type) {
        case RF:
            return(Instrument<BT>::commandInstr(addressRF_, cmd));
            break;
        case DC:
            return(Instrument<BT>::commandInstr(addressDC_, cmd));
            break;
        default: // FILTER
            return(Instrument<BT>::commandInstr(addressFilt_, cmd));
    };
}

//===============
// CustomReset()
//===============
bool SwitchMatrix::CustomReset(RelayType type) {
    RFRelayContainer toOpenRF, toCloseRF;
	RFRelayMap::iterator startRF = customResetRF_->begin();
    RFRelayMap::iterator stopRF = customResetRF_->end();
    RFFilterContainer toOpen, toClose;
    FilterRelayMap::iterator start = customResetFilt_->begin();
    FilterRelayMap::iterator stop = customResetFilt_->end();
    DCRelayContainer toOpenDC, toCloseDC;
	DCRelayMap::iterator startDC = customResetDC_->begin();
    DCRelayMap::iterator stopDC = customResetDC_->end();

    // Perform a user-defined custom reset on the 'type' of instr
    bool check1 = true, check2 = true;
    switch(type) {
        case RF:
            Assert<ContainerState>(startRF != stopRF, nameRF());
            while ( startRF != stopRF ) {
                if ( startRF->second == OFF )
                    toOpenRF.push_back(startRF->first);
                else // ON
                    toCloseRF.push_back(startRF->first);
                ++startRF;
	        }
            if ( ! toOpenRF.empty() )
                check1 = Open(toOpenRF); 
            else
                check1 = false;

            if ( ! toCloseRF.empty() )
                check2 = Close(toCloseRF);
            else
                check2 = false;
            break;
       case FILTER:
            Assert<ContainerState>(start != stop, nameFilter());
            while ( start != stop ) {
                if ( start->second == OFF )
                    toOpen.push_back(start->first);
                else // ON
                    toClose.push_back(start->first);
                ++start;
	        }
            if ( ! toOpen.empty() )
                check1 = Open(toOpen); 
            else
                check1 = false;

            if ( ! toClose.empty() )
                check2 = Close(toClose);
            else
                check2 = false;
            break;
       default: // DC
           Assert<ContainerState>(startDC != stopDC, nameDC());
           while ( startDC != stopDC ) {
               if ( startDC->second == OFF )
                   toOpenDC.push_back(startDC->first);
               else // ON
                   toCloseDC.push_back(startDC->first);
               ++startDC;
	       }           
           if ( ! toOpenDC.empty() )
               check1 = Open(toOpenDC); 
           else
               check1 = false;

           if ( ! toCloseDC.empty() )
               check2 = Close(toCloseDC);
           else
               check2 = false;
    };
    return(check1 || check2);
}

//==============
// Initialize()
//==============
bool SwitchMatrix::Initialize() {
    RFRelayMap::iterator startRF = rf_->begin(), stopRF = rf_->end();
    DCRelayMap::iterator startDC = dc_->begin(), stopDC = dc_->end();
    FilterRelayMap::iterator startFilt = filter_->begin(), 
                           stopFilter = filter_->end();

    // Initialize SwitchMatrix Instrument
	locked_ = false;
    std::vector<RelayType> addresses;
    addresses.push_back(RF);  
    addresses.push_back(DC);
    addresses.push_back(FILTER);
    std::vector<RelayType>::iterator i = addresses.begin(), j = addresses.end();
    bool result = false;
    while ( i != j ) {
        result = command(Language::Initialize(), *i);
        if ( ! result ) break;
        ++i;
    }
    if ( ! result ) return(result);
    
    // Open RF lines
	std::string syntax = Language::Open(startRF->first);
	while ( ++startRF != stopRF ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(startRF->first);
	}
	Assert<UnexpectedState>(command(syntax, RelayType::RF), Name());

    // Open DC lines
    syntax = Language::Open(startDC->first);
	while ( ++startDC != stopDC ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(startDC->first);
	}
	Assert<UnexpectedState>(command(syntax, RelayType::DC), Name()); 

    // Open RF Filter lines
    syntax = Language::Open(startFilt->first);
	while ( ++startFilt != stopFilter ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(startFilt->first);
	}
	Assert<UnexpectedState>(command(syntax, RelayType::FILTER), Name());
    return(true);
}

//===========
// IsError()
//===========
bool SwitchMatrix::IsError(RelayType r) {
    std::string syntax = Language::IsError();
    return(bitprocess(query(syntax, r), Instrument<BT>::ERROR)); 
}

//========
// Name() 
//========
std::string SwitchMatrix::Name() {
    return("DC-RF-FILTER Switch Matrix");
}

//==========
// nameDC() 
//==========
std::string SwitchMatrix::nameDC() const {
    return("DC Switch Matrix");
}

//==============
// nameFilter() 
//==============
std::string SwitchMatrix::nameFilter() const {
    return("Filter Switch Matrix");
}

//==========
// nameRF() 
//==========
std::string SwitchMatrix::nameRF() const {
    return("RF Switch Matrix");
}

//==================
// Open() Overload1
//==================
bool SwitchMatrix::Open(RFRelay relay) {
    RFRelayMap::iterator find = rf_->find(relay);
    Assert<ContainerState>(find != rf_->end(), nameRF());
    if ( find->second == OFF ) // already there
        return(false);
    std::string syntax = Language::Open(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::RF), nameRF());
    find->second = OFF;
    return(true);
}

//==================
// Open() Overload2
//==================
bool SwitchMatrix::Open(const RFRelayContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameRF());
    RFRelayContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    RFRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = rf_->find(*start);
        ++start;
        Assert<ContainerState>(find != rf_->end(), nameRF());
        if ( firstLoop && (find->second != OFF) ) {
            syntax = Language::Open(find->first);
            firstLoop = false;
        }
        else if ( find->second != OFF ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Open(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::RF), nameRF());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = rf_->find(*start);
            find->second = OFF;
            ++start;
        }        
    }
    return(actionDone);
}

//==================
// Open() Overload3
//==================
bool SwitchMatrix::Open(DCRelay relay) {
    DCRelayMap::iterator find = dc_->find(relay);
    Assert<ContainerState>(find != dc_->end(), nameDC());
    if ( find->second == OFF ) // already there
        return(false);
    std::string syntax = Language::Open(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::DC), nameDC());
    find->second = OFF;
    return(true);
}

//==================
// Open() Overload4
//==================
bool SwitchMatrix::Open(const DCRelayContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameDC());
    DCRelayContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    DCRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = dc_->find(*start);
        ++start;
        Assert<ContainerState>(find != dc_->end(), nameDC());
        if ( firstLoop && (find->second != OFF) ) {
            syntax = Language::Open(find->first);
            firstLoop = false;
        }
        else if ( find->second != OFF ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Open(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::DC), nameDC());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = dc_->find(*start);
            find->second = OFF;
            ++start;
        }
    }
    return(actionDone);
}

//==================
// Open() Overload5
//==================
bool SwitchMatrix::Open(FilterRelay relay) {
    FilterRelayMap::iterator find = filter_->find(relay);
    Assert<ContainerState>(find != filter_->end(), nameFilter());
    if ( find->second == OFF ) // already there
        return(false);
    std::string syntax = Language::Open(relay);
    Assert<UnexpectedState>(command(syntax, RelayType::FILTER), nameFilter());
    find->second = OFF;
    return(true);
}

//==================
// Open() Overload6
//==================
bool SwitchMatrix::Open(const RFFilterContainer& relays) {
    Assert<BadArg>(! relays.empty(), nameFilter());
    RFFilterContainer::const_iterator start = relays.begin();
    bool firstLoop = true;
    std::string syntax = "";
    FilterRelayMap::iterator find;
    while ( start != relays.end() ) {
        find = filter_->find(*start);
        ++start;
        Assert<ContainerState>(find != filter_->end(), nameFilter());
        if ( firstLoop && (find->second != OFF) ) {
            syntax = Language::Open(find->first);
            firstLoop = false;
        }
        else if ( find->second != OFF ) { // not already there
            syntax += Language::Concatenate(); 
            syntax += Language::Open(find->first);
        }
    }
    bool actionDone = false;
    if ( ! syntax.empty() ) { // something to actually do
        actionDone = true;
        Assert<UnexpectedState>(command(syntax, RelayType::FILTER), nameFilter());
        start = relays.begin();
        while ( start != relays.end() ) {
            find = filter_->find(*start);
            find->second = OFF;
            ++start;
        }
    }
    return(actionDone);
}

//===============
// OpsComplete()
//===============
bool SwitchMatrix::OpsComplete(RelayType r) {
    Assert<InstrumentError>(command(Language::SetOpsComplete(), r), Name());
	std::string syntax = Language::IsDone();
	return(bitprocess(query(syntax, r), Instrument<BT>::OPSCOMPLETE)); 
}

//=========
// query() 
//=========
std::string SwitchMatrix::query(const std::string& str, RelayType r) {
    long add = -1;
    switch(r) {
        case RF:
            add = addressRF_;
            break;
        case DC:
            add = addressDC_;
            break;
        default: // case FILTER:
            add = addressFilt_;
    };
	Assert<UnexpectedState>(!(str.empty() || locked_), Name());
	return(Instrument<BT>::queryInstr(add, str));
}

//=========
// Reset()
//=========
bool SwitchMatrix::Reset() {
    return(Initialize());
}

//============================
// SetCustomReset() Overload1
//============================
void SwitchMatrix::SetCustomReset(const RFRelayMap& relays) {
    customResetRF_->erase(customResetRF_->begin(), customResetRF_->end());
    customResetRF_.reset(new RFRelayMap(relays)); 
}

//============================
// SetCustomReset() Overload2
//============================
void SwitchMatrix::SetCustomReset(const DCRelayMap& relays) {
    customResetDC_->erase(customResetDC_->begin(), customResetDC_->end());
    customResetDC_.reset(new DCRelayMap(relays)); 
}

//============================
// SetCustomReset() Overload3
//============================
void SwitchMatrix::SetCustomReset(const FilterRelayMap& relays) { 
    customResetFilt_->erase(customResetFilt_->begin(), customResetFilt_->end());
    customResetFilt_.reset(new FilterRelayMap(relays));
}

//=============
// WhatError() 
//=============
std::string SwitchMatrix::WhatError(RelayType r) {
    return(query(Language::WhatError(), r));
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
