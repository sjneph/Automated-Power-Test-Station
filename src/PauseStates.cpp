// Files included
#include "Assert.h"
#include "Converter.h"
#include "GenericAlgorithms.h"
#include "InstrumentFile.h"
#include "MainSupplyTraits.h"
#include "PauseStates.h"
#include "ScaleUnits.h"
#include "SingletonType.h"
#include "SPTSException.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   11/14/05, sjn,
   ==============
     Added public method: SupplyCCModeChangePause().  This is related to pauses specific
       to particular power supplies that cannot be captured generically.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg      BadArg;
    typedef StationExceptionTypes::FileError   FileError;
    typedef StationExceptionTypes::NoFileFound NoFileFound;
    typedef StationExceptionTypes::OutOfRange  OutOfRange;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/
    
//=============
// Constructor
//=============
PauseStates::PauseStates() 
                  : familyNumber_(""), 
                    defaultFile_(new FileTypes::PauseFileType("Default Pause File")),
                    name_("Pause States File") 
{ /* */ }

//==================
// checkForNewDUT()
//==================
void PauseStates::checkForNewDUT() {
    std::string famNumber = SingletonType<Converter>::Instance()->FamilyNumber();
    if ( famNumber == familyNumber_ ) // no change
        return;
    familyNumber_ = famNumber;

    // Throws NoFileFound if no specific pauses for this (familyNumber_)
    //  use default pauses if that occurs
    try { 
        pf_.reset(new FileTypes::PauseFileType(famNumber));
    } catch(NoFileFound) {
        pf_.reset(0);
        loadType(DEFAULT);
        return;
    }
    loadType(SPECIFIC);
}

//===========================
// GetPauseValue() Overload1
//===========================
ProgramTypes::SetType PauseStates::GetPauseValue(PauseTypes type) {    
    checkForNewDUT();
    MapType::iterator find = map_->find(type);
	Assert<BadArg>(find != map_->end(), name_);
    return(find->second);
}

//===========================
// GetPauseValue() Overload2
//===========================
ProgramTypes::SetType PauseStates::GetPauseValue(StationPauses type) {
    if ( type == POWERSUPPLYCHANGE )
        return(0.01); // Defined by Interpoint engineers
    else if ( type == RELAYSTATECHANGE )
        return(0.05); // Defined through experimentation
    else // SYNCINPUT
        return(1); // Defined through experimentation
}

//===========================
// SupplyCCModeChangePause()
//===========================
ProgramTypes::SetType
       PauseStates::SupplyCCModeChangePause(MainSupplyTraits::Supply whichSupply) {
    InstrumentFile* ifile = SingletonType<InstrumentFile>::Instance();
    return(ifile->SupplyCCModeChangePause(whichSupply));
}

//=============
// getString()
//=============
PauseStates::MapStringType PauseStates::getString() {
    MapStringType toRtn;

    // Must stay in the order of the PauseTypes enumerations
    toRtn.insert(std::make_pair(SAFEINHIBITON, "Safe Inhibit On"));
    toRtn.insert(std::make_pair(SAFEINHIBITOFF, "Safe Inhibit Off"));
    toRtn.insert(std::make_pair(VOUTDC, "Vout DC"));
    toRtn.insert(std::make_pair(IOUTDC, "Iout DC"));
    toRtn.insert(std::make_pair(MEASUREDMM, "DMM Measure"));    
    toRtn.insert(std::make_pair(MEASURESCOPE, "Scope Measure"));
    toRtn.insert(std::make_pair(LLDO, "LLDO"));
    toRtn.insert(std::make_pair(OPTIONALINITIALCONDITIONS, 
                                   "Optional Initial Conditions"));
    toRtn.insert(std::make_pair(MISCELLANEOUSINITIALCONDITIONS, 
                                   "Miscellaneous Initial Conditions"));
    toRtn.insert(std::make_pair(TRANSIENTTRIGGER, "Transient Trigger"));
    toRtn.insert(std::make_pair(TOD, "Turn On Delay"));
    toRtn.insert(std::make_pair(TRIPPOINT, "Iout Trip Point"));
	toRtn.insert(std::make_pair(INHIBITPULSEWIDTH, "Inhibit Pulsewidth"));
    return(toRtn);
}

//============
// loadType()
//============
void PauseStates::loadType(Types t) {
    typedef ProgramTypes::SetType SType;
    map_.reset(new MapType);
    MapStringType v = getString();
    MapStringType::iterator i = v.begin(), j = v.end();
    while ( i != j ) {
        std::string s;
        if ( t == SPECIFIC )
            s = pf_->GetValue(i->second);
        else // DEFAULT
            s = defaultFile_->GetValue(i->second);
		if (i->first == INHIBITPULSEWIDTH){
			try{
		        Assert<FileError>(! s.empty(), name_);
			}catch(...){
				s = "0ms";
			};
		}
        Assert<FileError>(! s.empty(), name_);
        std::pair<SType, ScaleUnits<SType>::Units> p = ScaleUnits<SType>::GetUnits(s);
        SType value = ScaleUnits<SType>::ScaleUp(p.first, p.second);
        map_->insert(std::make_pair(i->first, value));
        ++i;
    }
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
