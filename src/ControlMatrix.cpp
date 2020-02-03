// Files included
#include "Assertion.h"
#include "ControlMatrix.h"
#include "GenericAlgorithms.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


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

namespace {
    typedef StationExceptionTypes::BaseException   StationBaseException;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::BadRtnValue     BadRtnValue;
    typedef StationExceptionTypes::ContainerState  ContainerState;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ControlMatrix <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

namespace SPTSInstrument {

//==============
// bitprocess()
//==============
bool ControlMatrix::bitprocess(const std::string& errorString,
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

//===========
// command()
//===========
bool ControlMatrix::command(const std::string& str, long address) {
	return(Instrument<BT>::commandInstr(address, str));
}

//==============
// Initialize()
//==============
bool ControlMatrix::Initialize() {
    std::set<long>::iterator start = addresses_.begin();
    std::set<long>::iterator stop  = addresses_.end();
    Assert<UnexpectedState>(start != stop, Name());
    bool result = false;
    while ( start != stop ) {
        result = command(Language::Initialize(), *start);
        if ( ! result ) break;
        ++start;
    }
	return(result);
}

//========
// Name()
//========
std::string ControlMatrix::Name() const {
    return("Control Matrix");
}

//=========
// query()
//=========
std::string ControlMatrix::query(const std::string& str, long address) {
	Assert<UnexpectedState>(!str.empty(), Name());
	return(Instrument<BT>::queryInstr(address, str));
}

//==============
// setAddress()
//==============
void ControlMatrix::setAddress(long address) {
    addresses_.insert(address);
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ControlMatrix::RLL <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


//================================
// ControlMatrix::RLL Constructor
//================================
ControlMatrix::RLL::RLL(const RLLContainer& container) 
                 : address_(-1), locked_(true), concatenated_(false), name_(Name())  {
	Assert<BadArg>(! container.empty(), name_);
	RLLContainer::const_iterator start = container.begin(), stop = container.end();
	while ( start != stop ) {        
		knownStates_.insert(std::make_pair(*start, OFF)); 
        ++start;
    }
    InstrumentFile::Types type = InstrumentFile::RLL;
	address_ = ControlMatrix::instance().Instrument<BT>::getAddress(type);
    Assert<BadRtnValue>(address_ > 0, name_);
    ControlMatrix::instance().setAddress(address_);
}

//================
// ChangedState()
//================
bool ControlMatrix::RLL::ChangedState() {
    return(knownStates_ != customReset_);
}

//=====================
// command() Overload1
//=====================
bool ControlMatrix::RLL::command(const std::string& str) {
	Assert<InstrumentError>(!locked_, name_);
    std::string cmd;
	if ( concatenated_ ) {
		if ( ! str.empty() ) {
			if ( ! syntax_.empty() )
			    syntax_ += Language::Concatenate();
			syntax_ += str;
		}
		return(true);
	}
	else if ( ! syntax_.empty() ) {
		syntax_ += Language::Concatenate();
        syntax_ += str;
		cmd = syntax_;
		syntax_ = "";
	}
    else
        cmd = str;

	if ( ! cmd.empty() )
	    return(ControlMatrix::instance().command(cmd, address_));
	return(true);
}

//=====================
// command() Overload2
//=====================
bool ControlMatrix::RLL::command() {
	Assert<InstrumentError>(!(concatenated_ || locked_), name_);
	bool result = true;
	if ( ! syntax_.empty() ) 
        result = ControlMatrix::instance().command(syntax_, address_);
	syntax_ = "";
	return(result);
}

//===============
// Concatenate() 
//===============
void ControlMatrix::RLL::Concatenate(Switch state) {
	switch(state) {
		case OFF:
			concatenated_ = false;
			Assert<UnexpectedState>(command(), name_);
			break;
		default: // ON
			concatenated_ = true;
	}; // Switch
}

//================
// CurrentState()
//================
Switch ControlMatrix::RLL::CurrentState(RLLs line) {
    RLLMap::iterator find = knownStates_.find(line);
	Assert<ContainerState>(find != knownStates_.end());
	return(find->second);
}

//===============
// CustomReset()
//===============
bool ControlMatrix::RLL::CustomReset() {
	RLLContainer toOpen, toClose;
	RLLMap::iterator start = customReset_.begin(), stop = customReset_.end();

	Assert<ContainerState>(start != stop);
	while ( start != stop ) {
		if ( start->second == ON )
            toClose.insert(start->first);			
		else // OFF
			toOpen.insert(start->first);
		++start;
	}
    bool result1 = TurnOff(toOpen);
    bool result2 = TurnOn(toClose);
    return(result1 && result2);
}

//=================
// ImmediateMode()
//=================
void ControlMatrix::RLL::ImmediateMode() {
    concatenated_ = false;
    syntax_       = "";
}

//==============
// Initialize()
//==============
void ControlMatrix::RLL::Initialize() {
    RLLMap::iterator start = knownStates_.begin(), stop = knownStates_.end();
	std::string syntax = Language::Open(start->first);
	while ( ++start != stop ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(start->first);
	}
	locked_ = false;
    try {
	    Assert<InstrumentError>(command(syntax), name_);
    } catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }
}

//=================
// IsConcatenate()
//=================
bool ControlMatrix::RLL::IsConcatenate() const {
    return(concatenated_);
}

//===========
// IsError()
//===========
bool ControlMatrix::RLL::IsError() {
	return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsError(), address_), 
            Instrument<BT>::ERROR));
}

//========
// Name()
//========
std::string ControlMatrix::RLL::Name() {
    static std::string name = InstrumentFile::GetName(InstrumentFile::RLL);
	return(name);
}

//===============
// OpsComplete()
//===============
bool ControlMatrix::RLL::OpsComplete() {
	Assert<UnexpectedState>(! concatenated_, name_);
    syntax_ = Language::SetOpsComplete();
    Assert<InstrumentError>(command(), name_);
    return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsDone(), address_), 
		    Instrument<BT>::OPSCOMPLETE));
}

//=========
// Reset()
//=========
bool ControlMatrix::RLL::Reset() {
    Initialize();
    return(true);
}

//==================
// SetCustomReset()
//==================
void ControlMatrix::RLL::SetCustomReset(const RLLMap& relays) {
	customReset_.clear();
	customReset_ = relays;
}

//=========================
// stateChange() Overload1
//=========================
bool ControlMatrix::RLL::stateChange(RLLs line, Switch state) {
    RLLMap::iterator find = knownStates_.find(line);
	Assert<BadArg>(find != knownStates_.end(), name_);

	if ( state == find->second ) // already there
		return(false);
	switch ( state ) {
		case ON:
			command(Language::Close(find->first));
		    break;
		default: //  OFF:
			command(Language::Open(find->first));
	};
	updateStatus(line, state);
	return(true);
}

//=========================
// stateChange() Overload2
//=========================
bool ControlMatrix::RLL::stateChange(const RLLContainer& lines, Switch state) {
	if ( lines.empty() ) 
		return(false);

	std::string syntax;
	RLLContainer::const_iterator start = lines.begin(), stop = lines.end();
	RLLMap tmp;
	bool counter = false;
	while ( start != stop ) {
		RLLMap::iterator find = knownStates_.find(*start);
		Assert<ContainerState>(find != knownStates_.end(), name_);
        
		if ( state == ON ) {
            if ( ! counter ) {
				syntax = Language::Close(*start);
				counter = true;
			}
			else {
				syntax += Language::Concatenate();
				syntax += Language::Close(*start);
			}
		}
		else { // OFF
            if ( ! counter ) {
				syntax = Language::Open(*start);
				counter = true;
			}
			else {
				syntax += Language::Concatenate();
				syntax += Language::Open(*start);
			}
		}
		tmp.insert(std::make_pair(find->first, state));
		++start;
	}
	Assert<UnexpectedState>(command(syntax), name_);
    if ( ! tmp.empty() )
	    updateStatus(tmp);
	return(counter);
}

//=====================
// TurnOff() Overload1
//=====================
bool ControlMatrix::RLL::TurnOff(RLLs line) {
	return(stateChange(line, OFF));
}

//=====================
// TurnOff() Overload2
//=====================
bool ControlMatrix::RLL::TurnOff(const RLLContainer& lines) { 
	return(stateChange(lines, OFF));
}

//====================
// TurnOn() Overload1
//====================
bool ControlMatrix::RLL::TurnOn(RLLs line) {
	return(stateChange(line, ON));
}

//====================
// TurnOn() Overload2
//====================
bool ControlMatrix::RLL::TurnOn(const RLLContainer& lines) {
	return(stateChange(lines, ON));
}

//==============
// Underlying()
//==============
ControlMatrix* ControlMatrix::RLL::Underlying() {
	return(&ControlMatrix::instance());
}

//==========================
// updateStatus() Overload1
//==========================
void ControlMatrix::RLL::updateStatus(const RLLMap& relays) {
	RLLMap::const_iterator start = relays.begin(), stop = relays.end();
	while ( start != stop ) {
		RLLMap::iterator find = knownStates_.find(start->first);
		Assert<ContainerState>(find != knownStates_.end(), name_);
		find->second = start->second;                
		++start;
	}
}

//==========================
// updateStatus() Overload2
//==========================
void ControlMatrix::RLL::updateStatus(RLLs relay, Switch sw) {
    RLLMap::iterator find = knownStates_.find(relay);
	Assert<ContainerState>(find != knownStates_.end()); 
	find->second = sw;    
}

//=============
// WhatError()
//=============
std::string ControlMatrix::RLL::WhatError() const {
	return(ControlMatrix::instance().query(Language::WhatError(), address_));
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>> ControlMatrix::InputRelayControl <<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//==============================================
// ControlMatrix::InputRelayControl Constructor
//==============================================
ControlMatrix::InputRelayControl::InputRelayControl(const RelayContainer& container) 
                                        : address_(-1), locked_(true), name_(Name()) {
	Assert<BadArg>(!container.empty(), name_);
	RelayContainer::const_iterator start = container.begin(), stop = container.end();
	while ( start != stop ) {
		knownStates_.insert(std::make_pair(*start, OFF));
        ++start;
    }
    InstrumentFile::Types type = InstrumentFile::INPUTRELAYCONTROL;
    address_ = ControlMatrix::instance().Instrument<BT>::getAddress(type);
    Assert<BadRtnValue>(address_ > 0, name_);
    ControlMatrix::instance().setAddress(address_);
}

//================
// ChangedState()
//================
bool ControlMatrix::InputRelayControl::ChangedState(const RelayContainer& relays) {
    if ( knownStates_.size() != customReset_.size() )
        return(true);

	RelayStateMap::const_iterator s = knownStates_.begin();
    RelayStateMap::const_iterator t = customReset_.begin();
    while ( s != knownStates_.end() ) {
        if ( (s->first != t->first) || (s->second != t->second) ) { 
            if ( std::find(relays.begin(), relays.end(), s->first) == relays.end() )
                return(true);
        }                        
        ++s; ++t;
    }
    return(false);
}

//===========
// command()
//===========
bool ControlMatrix::InputRelayControl::command(const std::string& str) {
	Assert<InstrumentError>(!locked_, name_);
	return(ControlMatrix::instance().command(str, address_));
}

//================
// CurrentState()
//================
Switch ControlMatrix::InputRelayControl::CurrentState(InputRelay line) {
    RelayStateMap::iterator find = knownStates_.find(line);
	Assert<ContainerState>(find != knownStates_.end());
	return(find->second);
}

//===============
// CustomReset()
//===============
bool ControlMatrix::InputRelayControl::CustomReset() {
	RelayContainer toOpen, toClose;
	RelayStateMap::iterator start = customReset_.begin(), stop = customReset_.end();
	Assert<ContainerState>(start != stop);
	while ( start != stop ) {
		if ( start->second == ON )
			toClose.push_back(start->first);
		else // OFF
            toOpen.push_back(start->first);			
		++start;
	}
    bool result1 = TurnOff(toOpen);
    bool result2 = TurnOn(toClose);
    return(result1 && result2);
}

//==============
// Initialize()
//==============
void ControlMatrix::InputRelayControl::Initialize() {
    RelayStateMap::iterator start = knownStates_.begin(), stop = knownStates_.end();
	std::string syntax = Language::Open(start->first);
	while ( ++start != stop ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(start->first);
	}
	locked_ = false;
    try {
	    Assert<InstrumentError>(command(syntax), name_);
    } catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }    
}

//===========
// IsError()
//===========
bool ControlMatrix::InputRelayControl::IsError() {
    return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsError(), address_), 
		    Instrument<BT>::ERROR));
}

//========
// Name()
//========
std::string ControlMatrix::InputRelayControl::Name() {
    static std::string name = InstrumentFile::GetName(InstrumentFile::INPUTRELAYCONTROL);
	return(name);
}

//===============
// OpsComplete()
//===============
bool ControlMatrix::InputRelayControl::OpsComplete() {
    Assert<InstrumentError>(command(Language::SetOpsComplete()), name_);
    return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsDone(), address_), 
		    Instrument<BT>::OPSCOMPLETE));
}

//=========
// Reset()
//=========
bool ControlMatrix::InputRelayControl::Reset() {
    Initialize();
    return(true);
}

//==================
// SetCustomReset()
//==================
void ControlMatrix::InputRelayControl::SetCustomReset(const RelayStateMap& relays) {
	customReset_.clear();
	customReset_ = relays;
}

//=========================
// stateChange() Overload1
//=========================
bool ControlMatrix::InputRelayControl::stateChange(InputRelay relay, Switch state) {
    RelayStateMap::iterator find = knownStates_.find(relay);
	Assert<BadArg>(find != knownStates_.end(), name_);

	if ( state == find->second ) // already there
		return(false);
	switch ( state ) {
		case ON:
			command(Language::Close(find->first));
		    break;
		default: //  OFF:
			command(Language::Open(find->first));
	};
	updateStatus(relay, state);
	return(true);
}

//=========================
// stateChange() Overload2
//=========================
bool ControlMatrix::InputRelayControl::stateChange(const RelayContainer& relays, 
                                                   Switch state) {
	if ( relays.empty() ) 
		throw(BadArg(name_));

	std::string syntax;
	RelayContainer::const_iterator start = relays.begin(), stop = relays.end();
	RelayStateMap tmp;
	bool counter = false;
	while ( start != stop ) {
		RelayStateMap::iterator find = knownStates_.find(*start);
		Assert<ContainerState>(find != knownStates_.end(), name_);
        if ( find->second != state ) { // Something to do
		    if ( state == ON ) {
                if ( ! counter ) {
				    syntax = Language::Close(*start);
				    counter = true;
			    }
			    else {
				    syntax += Language::Concatenate();
				    syntax += Language::Close(*start);
			    }
		    }
		    else { // OFF
                if ( ! counter ) {
				    syntax = Language::Open(*start);
				    counter = true;
			    }
			    else {
				    syntax += Language::Concatenate();
				    syntax += Language::Open(*start);
			    }
		    }
		    tmp.insert(std::make_pair(find->first, state));
        }
        ++start;
	}
    if ( syntax.size() )
	    Assert<UnexpectedState>(command(syntax), name_);
    if ( ! tmp.empty() )
	    updateStatus(tmp);
	return(counter);
}

//=====================
// TurnOff() Overload1
//=====================
bool ControlMatrix::InputRelayControl::TurnOff(InputRelay relay) {
	return(stateChange(relay, OFF));
}

//=====================
// TurnOff() Overload2
//=====================
bool ControlMatrix::InputRelayControl::TurnOff(const RelayContainer& relays) {
    return(stateChange(relays, OFF));
}

//====================
// TurnOn() Overload1
//====================
bool ControlMatrix::InputRelayControl::TurnOn(InputRelay relay) {
	return(stateChange(relay, ON));
}

//====================
// TurnOn() Overload2
//====================
bool ControlMatrix::InputRelayControl::TurnOn(const RelayContainer& relays) {
	return(stateChange(relays, ON));
}

//==============
// Underlying()
//==============
ControlMatrix* ControlMatrix::InputRelayControl::Underlying() {
	return(&ControlMatrix::instance());
}

//==========================
// updateStatus() Overload1
//==========================
void ControlMatrix::InputRelayControl::updateStatus(const RelayStateMap& relays) {
	RelayStateMap::const_iterator start = relays.begin(), stop = relays.end();
	while ( start != stop ) {
		RelayStateMap::iterator find = knownStates_.find(start->first);
		Assert<ContainerState>(find != knownStates_.end(), name_);
		find->second = start->second;                
		++start;
	}
}

//==========================
// updateStatus() Overload2
//==========================
void ControlMatrix::InputRelayControl::updateStatus(InputRelay relay, Switch sw) {
    RelayStateMap::iterator find = knownStates_.find(relay);
	Assert<ContainerState>(find != knownStates_.end()); 
	find->second = sw;    
}

//=============
// WhatError()
//=============
std::string ControlMatrix::InputRelayControl::WhatError() const {
    return(ControlMatrix::instance().query(Language::WhatError(), address_));
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>> ControlMatrix::OutputRelayControl <<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


//===============================================
// ControlMatrix::OutputRelayControl Constructor
//===============================================
ControlMatrix::OutputRelayControl::OutputRelayControl(const RelayContainer& container) 
                                        : address_(-1), locked_(true), name_(Name()) {
	Assert<BadArg>(! container.empty(), name_);
	RelayContainer::const_iterator start = container.begin(), stop = container.end();
	while ( start != stop ) {
		knownStates_.insert(std::make_pair(*start, OFF));
        ++start;
    }
    InstrumentFile::Types type = InstrumentFile::OUTPUTRELAYCONTROL;
	address_ = ControlMatrix::instance().Instrument<BT>::getAddress(type);   
    Assert<BadRtnValue>(address_ > 0, name_);
    ControlMatrix::instance().setAddress(address_);
}

//================
// ChangedState()
//================
bool ControlMatrix::OutputRelayControl::ChangedState() {
    return(knownStates_ != customReset_);
}

//===========
// command()
//===========
bool ControlMatrix::OutputRelayControl::command(const std::string& str) {
	Assert<InstrumentError>(!locked_, name_);
	Assert<InstrumentError>(ControlMatrix::instance().command(str, address_));
    return(true);
}

//================
// CurrentState()
//================
Switch ControlMatrix::OutputRelayControl::CurrentState(OutputRelay line) {
    RelayStateMap::iterator find = knownStates_.find(line);
	Assert<ContainerState>(find != knownStates_.end());
	return(find->second);
}

//===============
// CustomReset()
//===============
bool ControlMatrix::OutputRelayControl::CustomReset() {
	RelayContainer toOpen, toClose;
	RelayStateMap::iterator start = customReset_.begin(), stop = customReset_.end();
	Assert<ContainerState>(start != stop);
	while ( start != stop ) {
		if ( start->second == ON )			
            toClose.push_back(start->first);
		else // OFF
			toOpen.push_back(start->first);
		++start;
	}
    bool result1 = TurnOff(toOpen);
    bool result2 = TurnOn(toClose);
    return(result1 && result2);
}

//==============
// Initialize()
//==============
void ControlMatrix::OutputRelayControl::Initialize() {
    RelayStateMap::iterator start = knownStates_.begin(), stop = knownStates_.end();
	std::string syntax = Language::Open(start->first);
	while ( ++start != stop ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(start->first);
	}
	locked_ = false;
    try {
	    Assert<InstrumentError>(command(syntax), name_);
    } catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }
}

//===========
// IsError()
//===========
bool ControlMatrix::OutputRelayControl::IsError() {
	return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsError(), address_), 
		    Instrument<BT>::ERROR));
}

//========
// Name()
//========
std::string ControlMatrix::OutputRelayControl::Name() {
    static std::string name = InstrumentFile::GetName(InstrumentFile::OUTPUTRELAYCONTROL);
	return(name);
}

//===============
// OpsComplete()
//===============
bool ControlMatrix::OutputRelayControl::OpsComplete() {
    Assert<InstrumentError>(command(Language::SetOpsComplete()), name_);
    return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsDone(), address_), 
		    Instrument<BT>::OPSCOMPLETE));
}

//=========
// Reset()
//=========
bool ControlMatrix::OutputRelayControl::Reset() {
    Initialize();
    return(true);
}

//==================
// SetCustomReset()
//==================
void ControlMatrix::OutputRelayControl::SetCustomReset(const RelayStateMap& relays) {
	customReset_.clear();
	customReset_ = relays;
}

//=========================
// stateChange() Overload1
//=========================
bool ControlMatrix::OutputRelayControl::stateChange(OutputRelay relay, Switch state) {
    RelayStateMap::iterator find = knownStates_.find(relay);
	Assert<BadArg>(find != knownStates_.end(), name_);

	if ( state == find->second ) // already there
		return(false);
	switch ( state ) {
		case ON:
			command(Language::Close(find->first));
		    break;
		default: //  OFF:
			command(Language::Open(find->first));
	};
	updateStatus(relay, state);
	return(true);
}

//=========================
// stateChange() Overload2
//=========================
bool ControlMatrix::OutputRelayControl::stateChange(const RelayContainer& relays, 
								                    Switch state) {
	if ( relays.empty() ) 
		return(false);

	std::string syntax;
	RelayContainer::const_iterator start = relays.begin(), stop = relays.end();
	RelayStateMap tmp;
	bool counter = false;
	while ( start != stop ) {
		RelayStateMap::iterator find = knownStates_.find(*start);
		Assert<ContainerState>(find != knownStates_.end(), name_);
        if ( find->second != state ) { // Something to do        
		    if ( state == ON ) {
                if ( ! counter ) {
				    syntax = Language::Close(*start);
				    counter = true;
			    }
			    else {
				    syntax += Language::Concatenate();
				    syntax += Language::Close(*start);
			    }
		    }
		    else { // OFF
                if ( ! counter ) {
				    syntax = Language::Open(*start);
				    counter = true;
			    }
			    else {
				    syntax += Language::Concatenate();
				    syntax += Language::Open(*start);
			    }
		    }
		    tmp.insert(std::make_pair(find->first, state));
        }
		++start;        
	}
    if ( syntax.size() )
	    Assert<UnexpectedState>(command(syntax), name_);
    if ( ! tmp.empty() )
	    updateStatus(tmp);
	return(counter);
}

//=====================
// TurnOff() Overload1
//=====================
bool ControlMatrix::OutputRelayControl::TurnOff(OutputRelay relay) {
	return(stateChange(relay, OFF));
}

//=====================
// TurnOff() Overload2
//=====================
bool ControlMatrix::OutputRelayControl::TurnOff(const RelayContainer& relays) {
    return(stateChange(relays, OFF));
}

//====================
// TurnOn() Overload1
//====================
bool ControlMatrix::OutputRelayControl::TurnOn(OutputRelay relay) {
	return(stateChange(relay, ON));
}

//====================
// TurnOn() Overload2
//====================
bool ControlMatrix::OutputRelayControl::TurnOn(const RelayContainer& relays) {
	return(stateChange(relays, ON));
}

//==============
// Underlying()
//==============
ControlMatrix* ControlMatrix::OutputRelayControl::Underlying() {
	return(&ControlMatrix::instance());
}

//==========================
// updateStatus() Overload1
//==========================
void ControlMatrix::OutputRelayControl::updateStatus(const RelayStateMap& relays) {
	RelayStateMap::const_iterator start = relays.begin(), stop = relays.end();
	while ( start != stop ) {
		RelayStateMap::iterator find = knownStates_.find(start->first);
		Assert<ContainerState>(find != knownStates_.end(), name_);
		find->second = start->second;                
		++start;
	}
}

//==========================
// updateStatus() Overload2
//==========================
void ControlMatrix::OutputRelayControl::updateStatus(OutputRelay relay, Switch sw) {
    RelayStateMap::iterator find = knownStates_.find(relay);
	Assert<ContainerState>(find != knownStates_.end()); 
	find->second = sw;    
}

//=============
// WhatError()
//=============
std::string ControlMatrix::OutputRelayControl::WhatError() const {
	return(ControlMatrix::instance().query(Language::WhatError(), address_));
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ControlMatrix::Misc <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//=================================
// ControlMatrix::Misc Constructor
//=================================
ControlMatrix::Misc::Misc(const MiscContainer& container) 
                                        : address_(-1), locked_(true), name_(Name()) {
	Assert<BadArg>(!container.empty(), name_);
	MiscContainer::const_iterator start = container.begin(), stop = container.end();
	while ( start != stop ) {
		knownStates_.insert(std::make_pair(*start, OFF));
        ++start;
    }
    InstrumentFile::Types type = InstrumentFile::MISC;
	address_ = ControlMatrix::instance().Instrument<BT>::getAddress(type);
    Assert<BadRtnValue>(address_ > 0, name_);
    ControlMatrix::instance().setAddress(address_);
}

//================
// ChangedState()
//================
bool ControlMatrix::Misc::ChangedState() {
    return(knownStates_ != customReset_);
}

//===========
// command()
//===========
bool ControlMatrix::Misc::command(const std::string& str) {
	Assert<InstrumentError>(! locked_, name_);
	return(ControlMatrix::instance().command(str, address_));
}

//================
// CurrentState()
//================
Switch ControlMatrix::Misc::CurrentState(MiscLine line) {
    MiscMap::iterator find = knownStates_.find(line);
	Assert<ContainerState>(find != knownStates_.end());
	return(find->second);
}

//===============
// CustomReset()
//===============
bool ControlMatrix::Misc::CustomReset() {
	MiscContainer toOpen, toClose;
	MiscMap::iterator start = customReset_.begin(), stop = customReset_.end();

	Assert<ContainerState>(start != stop);
	while ( start != stop ) {
		if ( start->second == ON )
            toClose.push_back(start->first);			
		else // OFF
			toOpen.push_back(start->first);
		++start;
	}
    bool result1 = TurnOff(toOpen);
    bool result2 = TurnOn(toClose);
    return(result1 && result2);
}

//=================
// GetKnownLines()
//=================
ControlMatrix::Misc::MiscMap ControlMatrix::Misc::GetKnownLines() {
    return(knownStates_);
}

//==============
// Initialize()
//==============
void ControlMatrix::Misc::Initialize() {
    MiscMap::iterator start = knownStates_.begin(), stop = knownStates_.end();
	std::string syntax = Language::Open(start->first);
	while ( ++start != stop ) {
		syntax += Language::Concatenate();
		syntax += Language::Open(start->first);
	}
	locked_ = false;
    try {
	    Assert<InstrumentError>(command(syntax), name_);
    }
    catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }
}

//===========
// IsError()
//===========
bool ControlMatrix::Misc::IsError() {
	return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsError(), address_), 
		    Instrument<BT>::ERROR));
}

//========
// Name()
//========
std::string ControlMatrix::Misc::Name() {
    static std::string name = InstrumentFile::GetName(InstrumentFile::MISC);
	return(name);
}

//===============
// OpsComplete()
//===============
bool ControlMatrix::Misc::OpsComplete() {
    Assert<InstrumentError>(command(Language::SetOpsComplete()), name_);
    return(ControlMatrix::instance().bitprocess(
            ControlMatrix::instance().query(Language::IsDone(), address_), 
		    Instrument<BT>::OPSCOMPLETE));
}

//=========
// Reset()
//=========
bool ControlMatrix::Misc::Reset() {
    Initialize();
    return(true);
}

//==================
// SetCustomReset()
//==================
void ControlMatrix::Misc::SetCustomReset(const MiscMap& lines) {
	customReset_.clear();
	customReset_ = lines;
    MiscMap::iterator i = customReset_.begin(), j = customReset_.end(), f;
    while ( i != j ) { // Make sure we know about all elements in lines
        f = knownStates_.find(i->first);
        Assert<BadArg>(f != knownStates_.end(), name_);
        ++i;
    }
}

//========
// Size() 
//========
std::size_t ControlMatrix::Misc::Size() {
    return(knownStates_.size());
}

//=========================
// stateChange() Overload1
//=========================
bool ControlMatrix::Misc::stateChange(MiscLine line, Switch state) {
    MiscMap::iterator find = knownStates_.find(line);
	Assert<BadArg>(find != knownStates_.end(), name_);

    if ( state == find->second ) // already there
	    return(false);
	switch ( state ) {
		case ON:
			command(Language::Close(find->first));
		    break;
		default: //  OFF:
			command(Language::Open(find->first));
	};
	updateStatus(line, state);
	return(true);
}

//=========================
// stateChange() Overload2
//=========================
bool ControlMatrix::Misc::stateChange(const MiscContainer& lines, Switch state) {

	if ( lines.empty() ) 
		return(false);

	std::string syntax;
	MiscContainer::const_iterator start = lines.begin(), stop = lines.end();
	MiscMap tmp;
	bool counter = false;
	while ( start != stop ) {
		MiscMap::iterator find = knownStates_.find(*start);
		Assert<ContainerState>(find != knownStates_.end(), name_);
        
		if ( state == ON ) {
            if ( ! counter ) {
				syntax = Language::Close(*start);
				counter = true;
			}
			else {
				syntax += Language::Concatenate();
				syntax += Language::Close(*start);
			}
		}
		else { // OFF
            if ( ! counter ) {
				syntax = Language::Open(*start);
				counter = true;
			}
			else {
				syntax += Language::Concatenate();
				syntax += Language::Open(*start);
			}
		}
		tmp.insert(std::make_pair(find->first, state));
		++start;
	}
	Assert<UnexpectedState>(command(syntax), name_);
    if ( ! tmp.empty() )
	    updateStatus(tmp);
	return(counter);
}

//=====================
// TurnOff() Overload1
//=====================
bool ControlMatrix::Misc::TurnOff(MiscLine line) {
	return(stateChange(line, OFF));
}

//=====================
// TurnOff() Overload2
//=====================
bool ControlMatrix::Misc::TurnOff(const MiscContainer& lines) {
    return(stateChange(lines, OFF));
}

//====================
// TurnOn() Overload1
//====================
bool ControlMatrix::Misc::TurnOn(MiscLine line) {
	return(stateChange(line, ON));
}

//====================
// TurnOn() Overload2
//====================
bool ControlMatrix::Misc::TurnOn(const MiscContainer& lines) {
	return(stateChange(lines, ON));
}

//==============
// Underlying()
//==============
ControlMatrix* ControlMatrix::Misc::Underlying() {
    return(&ControlMatrix::instance());
}

//==========================
// updateStatus() Overload1
//==========================
void ControlMatrix::Misc::updateStatus(const MiscMap& lines) {
	MiscMap::const_iterator start = lines.begin(), stop = lines.end();
	while ( start != stop ) {
		MiscMap::iterator find = knownStates_.find(start->first);
		Assert<ContainerState>(find != knownStates_.end(), name_);
		find->second = start->second;                
		++start;
	}
}

//==========================
// updateStatus() Overload2
//==========================
void ControlMatrix::Misc::updateStatus(MiscLine line, Switch sw) {
    MiscMap::iterator find = knownStates_.find(line);
	Assert<ContainerState>(find != knownStates_.end()); 
	find->second = sw;    
}

//=============
// WhatError()
//=============
std::string ControlMatrix::Misc::WhatError() const {
	return(ControlMatrix::instance().query(Language::WhatError(), address_));
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
