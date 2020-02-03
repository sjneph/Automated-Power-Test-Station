// Files included
#include "Assert.h"
#include "DMM.h"
#include "SingletonType.h"
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
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {


// static definitions
const ProgramTypes::SetType DMM::AUTO 
                     = static_cast<ProgramTypes::SetType>(DMM::Model::AUTORANGE);

//=============
// Constructor 
//=============   
DMM::DMM() : locked_(true), configuration_(DCV), rangeDCV_(AUTO), rangeOhm_(AUTO), 
             rangeoC_(AUTO), needReset_(true), name_(Name()) { 
    // not designed for DMM's w/o multiplexers yet
    Assert<InstrumentError>(Model::MULTIPLEXERCARDEXISTS, name_);
    address_ = Instrument<BusType>::getAddress(InstrumentFile::DMM);   
}

//============
// Destructor
//============
DMM::~DMM() 
{ /* */ }

//==============
// bitprocess() 
//==============
bool DMM::bitprocess(const std::string& errorString,
                     Instrument<BusType>::Register toCheck) {
	// Convert eString to binary representation
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(! eString.empty(), name_);
	NumberBase conversion(eString, 
                          static_cast<NumberBase::Base>(Model::BITRETURNTYPES));
	eString = conversion.Value(NumberBase::BINARY);    
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= Model::TOTALREGISTERBITS, name_);
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
bool DMM::command(const std::string& cmd) {
    Assert<UnexpectedState>(!(locked_ || cmd.empty()), name_);
    return(Instrument<BusType>::commandInstr(address_, cmd));
}

//==============
// Initialize() 
//==============
bool DMM::Initialize() {
    locked_ = false;    
    try {
        Assert<InstrumentError>(command(Language::Initialize()), name_);    
        rangeDCV_  = AUTO; 
        rangeOhm_  = AUTO; 
        rangeoC_   = AUTO; 
        needReset_ = true;
        configuration_ = OHMS;
        SetMode(DCV);            
    } catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }
    return(true);
}

//===========
// IsError() 
//===========
bool DMM::IsError() {
    return(bitprocess(query(Language::IsError()), Instrument<BusType>::ERROR)); 
}

//===========
// measure()
//===========
ProgramTypes::MType DMM::measure(Mode nextMode) {
    Assert<UnexpectedState>(configuration_ == nextMode, name_);
    if ( needReset_ )
        setModeRange();    
    std::string result = query(Language::Measure());
    return(convert<ProgramTypes::MType>(result));
}

//==================
// MeasureDCVolts() 
//==================
ProgramTypes::MType DMM::MeasureDCVolts() {
    return(measure(DCV));  
}

//===============
// MeasureOhms() 
//===============
ProgramTypes::MType DMM::MeasureOhms() {
    return(measure(OHMS));
}

//======================
// MeasureTemperature() 
//======================
ProgramTypes::MType DMM::MeasureTemperature() {
    return(measure(TEMP));
}

//========
// Name() 
//========
std::string DMM::Name() {
    return("DMM");
}

//===============
// OpsComplete() 
//===============
bool DMM::OpsComplete() {
    Assert<InstrumentError>(command(Language::SetOpsComplete()), name_);
	return(bitprocess(query(Language::IsDone()), Instrument<BusType>::OPSCOMPLETE));
}

//=========
// query()
//=========
std::string DMM::query(const std::string& str) {
    Assert<UnexpectedState>(!(str.empty() || locked_), name_);
	return(Instrument<BusType>::queryInstr(address_, str));
}

//=========
// Reset() 
//=========
bool DMM::Reset() {
    return(Initialize());
}

//===========
// SetMode()
//===========
void DMM::SetMode(Mode mode) {
    if ( configuration_ == mode ) // already there
        return;
    configuration_ = mode;
    needReset_ = true; // requires call to setModeRange() prior to Measure
                       //  --> ensure range is set appropriately  
}

//================
// setModeRange()
//================
void DMM::setModeRange() {
    switch(configuration_) {
        case OHMS:
            Assert<InstrumentError>(command(
                Language::ConfigureOhms(Model::OHMSRELAYCHANNEL, rangeOhm_)),
                name_
                                   );            
            break; 
        case DCV:
            Assert<InstrumentError>(command(
                Language::ConfigureDCVolts(Model::DCVOLTAGERELAYCHANNEL, rangeDCV_)),
                name_
                                   );                       
            break;
        case TEMP:
            Assert<InstrumentError>(command(
                Language::ConfigureTemperature(Model::TEMPERATURERELAYCHANNEL)), 
                name_
                                   );
    };
    needReset_ = false;
}

//============
// SetRange() 
//============
void DMM::SetRange(const ProgramTypes::SetType& range) {    
    ProgramTypes::SetType r = range;

    // Error check
    typedef SingletonType<InstrumentFile> IF;
    static ProgramTypes::SetType maxV = IF::Instance()->MaxVolts(InstrumentTypes::DMM);    

    if ( (r != AUTO) && (r < static_cast<ProgramTypes::SetType>(0)) )
        r = absolute(r);

    switch(configuration_) {
        case DCV:
            Assert<BadArg>(range <= maxV, name_);
            if ( (rangeDCV_ == r) && (!locked_) ) // already there 
                return; 
            rangeDCV_ = r;
            break;
        case OHMS:
            if ( (rangeOhm_ == r) && (!locked_) ) // already there
                return;
            rangeOhm_ = r;
            break;        
        case TEMP:
            if ( (rangeoC_ == r) && (!locked_) ) // already there
                return;
            rangeoC_ = r;
    };  
    needReset_ = true; // requires call to setModeRange() prior to Measure
                       //  --> ensure range is set appropriately  
}

//=============
// WhatError()
//=============
std::string DMM::WhatError() {
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
