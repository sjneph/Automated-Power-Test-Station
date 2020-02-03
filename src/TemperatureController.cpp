// Files included
#include "Assertion.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"
#include "TemperatureController.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  07/16/03, sjn,
  ==============
      Added GetTemperatureSetting() and private variables setting_, setTemp_.
      Changed Assert<InstrumentError> to Assert<BadCommand> in command().
      Modified Initialize() and SetTemperature() to support new private member vars.
      Modified Initialize() --> changed logic of isOff_ so that it is set to false
        following instrument initialization.
      Changed return value of IsOn() to include setTemp_ in addition to isOff_ -->
        a call to both Initialize() and SetTemperature() must be done in order for
        IsOn() to return true.  Previously, only Initialize() was required.
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
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//=============
// Constructor
//=============
TemperatureController::TemperatureController() 
                              : isOff_(true), locked_(true), limitsSpecified_(false),
                                min_(0), max_(-1), setting_(0), setTemp_(false) {
    address_ = Instrument<BT>::getAddress(InstrumentFile::TEMPCONTROLLER);
}
  
//============
// Destructor
//============  
TemperatureController::~TemperatureController()
{ /* */ }

//==============
// bitprocess()
//==============
bool TemperatureController::bitprocess
                   (const std::string& errorString, Instrument<BT>::Register toCheck) {
    // Convert eString to binary representation
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(! eString.empty(), Name());
	NumberBase conversion(eString, 
                          static_cast<NumberBase::Base>(ModelType::BitReturnTypes));
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= ModelType::TotalRegisterBits, Name());
	std::string bitSet("");
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset ) {
			bitSet += convert<std::string>(idx);
			bitSet += "||"; 
		} // if
	} // for-loop
	bool result = false;
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
			result = 
				(bitSet.find_first_of(Language::ErrorBits()) != std::string::npos);
			break;
		default:   // OPSCOMPLETE --> if done, then return true
			result = 
				(bitSet.find_first_of(Language::OpCompleteBits()) != std::string::npos);
	}; // Switch
	return(result);
}

//===========
// command()
//===========
bool TemperatureController::command(const std::string& cmd) {
    Assert<BadCommand>((!locked_) && limitsSpecified_, Name());
    return(Instrument<BT>::commandInstr(address_, cmd));
}

//======================
// CurrentTemperature()
//======================
ProgramTypes::MType TemperatureController::CurrentTemperature() {
    std::string toRtn = query(Language::WhatTemperature());
    return(convert<ProgramTypes::MType>(toRtn));
}

//=========================
// GetTemperatureSetting()
//=========================
ProgramTypes::SetType TemperatureController::GetTemperatureSetting() const {
    Assert<BadCommand>(!locked_ && IsOn() && limitsSpecified_ && setTemp_, Name());
    return(setting_);
}

//==============
// Initialize()
//==============
bool TemperatureController::Initialize() {        
    isOff_ = false;
    bool change = false;
    setting_ = 0;
    setTemp_ = false;
    try {
        if ( !limitsSpecified_ ) {
           change = true;
           limitsSpecified_ = true;
        }
        locked_  = false;   
        Assert<InstrumentError>(command(Language::Initialize()), Name());
        Assert<InstrumentError>(command(Language::SetControllingProbe()), Name());
        Assert<InstrumentError>(command(Language::GoImmediateMode()), Name());
    } catch(StationBaseException& error) {
        locked_ = true;
        isOff_ = true;
        if ( change )
            limitsSpecified_ = false;
        throw(error);
    } 
    if ( change )
        limitsSpecified_ = false;
    return(true);
}

//===========
// IsError()
//===========
bool TemperatureController::IsError() {
    return(bitprocess(query(Language::IsError()), Instrument<BT>::ERROR));
}

//========
// IsOn()
//========
bool TemperatureController::IsOn() const {
    return(!isOff_ && setTemp_);
}

//========
// Name()
//========
std::string TemperatureController::Name() const {
    return("Temperature Controller");
}

//===============
// OpsComplete()
//===============
bool TemperatureController::OpsComplete() {
    return(bitprocess(query(Language::IsDone()), Instrument<BT>::OPSCOMPLETE));
}

//=========
// query()
//=========
std::string TemperatureController::query(const std::string& q) {
    std::string s = Instrument<BT>::queryInstr(address_, q);
    Language::Clean(s);
    return(s);
}

//=========
// Reset()
//=========
bool TemperatureController::Reset() {
    Assert<InstrumentError>(Initialize(), Name());
    return(true);
}

//==================
// SetTemperature()
//==================
void TemperatureController::SetTemperature
                                (const ProgramTypes::SetType& temperature) {
    setting_ = 0;
    setTemp_ = false;
    Assert<BadArg>((temperature >= min_) && (temperature <= max_), Name());
    Assert<InstrumentError>(command(Language::GoToTemperature(temperature)), Name());
    setting_ = temperature;
    setTemp_ = true;
}

//========================
// SetTemperatureLimits()
//========================
void TemperatureController::SetTemperatureLimits
                                     (const ProgramTypes::SetType& lowLimit, 
                                      const ProgramTypes::SetType& hiLimit) {
    Assert<BadArg>(lowLimit < hiLimit, Name());
    typedef InstrumentError IE;
    limitsSpecified_ = true;
    try {
        Assert<IE>(command(Language::SetTemperatureLimits(lowLimit, hiLimit)), Name());    
    } catch(StationBaseException& error) {     
        limitsSpecified_ = false;
        throw(error);
    }
    min_ = lowLimit;
    max_ = hiLimit;
}

//===========
// TurnOff()
//===========
void TemperatureController::TurnOff() {
    Reset();
}

//=============
// WhatError()
//=============
std::string TemperatureController::WhatError() {
    return(query(Language::WhatError()));
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
