// Files included
#include "Assertion.h"
#include "Converter.h"
#include "GenericAlgorithms.h"
#include "ScaleUnits.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   10/11/04, sjn,
   ==============
    Modified SS::getValue(Int2Type<TRIGSOURCE>) --> "EXTERNAL" or "4" returned as
       OScopeChannels::TRIGGER to reflect station layout changes.
    Modified SS::getValue(Int2Type<TRIGMODE>) --> removed "SINGLE" and "STOP" as
       possible selections for the Trigger Mode.
    Changed name of NewFamilyNumber() to Reload().
    Modified getValue(Int2Type<COUPLING>) --> when DC50 is specified, we will return
       DC1M instead of DC50.  This reflects changes to the station's layout - the 
       software is no longer responsible for selecting 50 ohms.
    Added getValue<> overload implementations for TRIGVERTSCALE and TRIGCHANCOUPLING.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//===================
// Unnamed Namespace
//===================
namespace {
    typedef StationExceptionTypes::FileError FileError;
    typedef OScopeSetupFile SS;
    typedef ProgramTypes::SetType SType;

    std::string noNumbers(const std::string value) {
        std::string toRtn = "";
        long size = static_cast<long>(value.size());
        for ( long idx = 0; idx < size; ++idx ) {
            if ( ! isdigit(value[idx]) )
                toRtn += value[idx];
        }
        return(toRtn);
    }

    typedef StationExceptionTypes::BadArg BadArg;

} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
SS::OScopeSetupFile() 
   : number_(VERTSCALE + 1), value_(""), sf_(0), params_(0), locked_(true),
     masterFile_(new FileTypes::ScopeSetupFileType("Master Scope Setup File"))
{ /* */ }

//============
// Destructor
//============
SS::~OScopeSetupFile() 
{ /* */ }

//======================
// getValue() Overload1
//======================
Switch SS::getValue(Int2Type<BANDWIDTH>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "OFF" )
            return(OFF);
        else if ( masterValue_ == "ON" )
            return(ON);
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "OFF" )
        return(OFF);
    else if ( value_ == "ON" )
        return(ON);
    throw(FileError(name()));
}

//======================
// getValue() Overload2
//======================
SS::CouplingType SS::getValue(Int2Type<COUPLING>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "AC1M" )
            return(AC1M);
        else if ( masterValue_ == "DC1M" )
            return(DC1M);
        else if ( masterValue_ == "DC50" )
            return(DC1M); // no 50-ohm software support any longer
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "AC1M" )
        return(AC1M);
    else if ( value_ == "DC1M" )
        return(DC1M);
    else if ( value_ == "DC50" )
        return(DC1M); // no 50-ohm software support any longer
    throw(FileError(name()));
}

//======================
// getValue() Overload3
//======================
SS::MemorySetting SS::getValue(Int2Type<HORMEMORYSETTING>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "MIN" )
            return(MIN);
        else if ( masterValue_ == "SMALL" )
            return(SMALL);
        else if ( masterValue_ == "MEDIUM" )
            return(MEDIUM);
        else if ( masterValue_ == "BIG" )
            return(BIG);
        else if ( masterValue_ == "MAX" )
            return(MAX);
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "MIN" )
        return(MIN);
    else if ( value_ == "SMALL" )
        return(SMALL);
    else if ( value_ == "MEDIUM" )
        return(MEDIUM);
    else if ( value_ == "BIG" )
        return(BIG); 
    else if ( value_ == "MAX" )
        return(MAX);
    throw(FileError(name()));
}

//======================
// getValue() Overload4
//======================
ProgramTypes::SetType SS::getValue(Int2Type<HORSCALE>) {
    std::pair<SType, ScaleUnits<SType>::Units> p;

    // Master file takes precedence    
    if ( masterSet_ ) {
        p = ScaleUnits<SType>::GetUnits(masterValue_);
        return(ScaleUnits<SType>::ScaleUp(p.first, p.second));    
    }

    // Otherwise, specific file
    p = ScaleUnits<SType>::GetUnits(value_);
    return(ScaleUnits<SType>::ScaleUp(p.first, p.second));
}

//======================
// getValue() Overload5
//======================
ProgramTypes::SetType SS::getValue(Int2Type<OFFSET>) {
    std::pair<SType, ScaleUnits<SType>::Units> p;

    // Master file takes precedence    
    if ( masterSet_ ) {
        p = ScaleUnits<SType>::GetUnits(masterValue_);
        return(ScaleUnits<SType>::ScaleUp(p.first, p.second));    
    }

    // Otherwise, specific file
    p = ScaleUnits<SType>::GetUnits(value_);
    return(ScaleUnits<SType>::ScaleUp(p.first, p.second));
}

//======================
// getValue() Overload6
//======================
SS::CouplingType SS::getValue(Int2Type<TRIGCHANCOUPLING>) {
    return(getValue(Int2Type<COUPLING>())); // call overload
}

//======================
// getValue() Overload7
//======================
SS::TriggerCouplingType SS::getValue(Int2Type<TRIGCOUPLING>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "AC" )
            return(AC);
        else if ( masterValue_ == "DC" )
            return(DC);
        else if ( masterValue_ == "HFR" )
            return(HFR);
        else if ( masterValue_ == "LFR" )
            return(LFR);
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "AC" ) 
        return(AC);
    else if ( value_ == "DC" )
        return(DC);
    else if ( value_ == "HFR" )
        return(HFR);
    else if ( value_ == "LFR" )
        return(LFR);
    throw(FileError(name()));
}

//======================
// getValue() Overload8
//======================
ProgramTypes::SetType SS::getValue(Int2Type<TRIGLEVEL>) {
    std::pair<SType, ScaleUnits<SType>::Units> p;

    // Master file takes precedence    
    if ( masterSet_ ) {
        p = ScaleUnits<SType>::GetUnits(masterValue_);
        return(ScaleUnits<SType>::ScaleUp(p.first, p.second));    
    }

    // Otherwise, specific file
    p = ScaleUnits<SType>::GetUnits(value_);
    return(ScaleUnits<SType>::ScaleUp(p.first, p.second));    
}

//======================
// getValue() Overload9
//======================
SS::TriggerMode SS::getValue(Int2Type<TRIGMODE>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "AUTO" )
            return(AUTO);
        else if ( masterValue_ == "NORMAL" )
            return(NORMAL);
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "AUTO" )
        return(AUTO);
    else if ( value_ == "NORMAL" )
        return(NORMAL);
    throw(FileError(name()));
}

//=======================
// getValue() Overload10
//=======================
SS::SlopeType SS::getValue(Int2Type<TRIGSLOPE>) {
    // Master file takes precedence    
    if ( masterSet_ ) {
        if ( masterValue_ == "POSITIVE" )
            return(POSITIVE);
        else if ( masterValue_ == "NEGATIVE" )
            return(NEGATIVE);
        throw(FileError(name()));
    }

    // Otherwise, specific file
    if ( value_ == "POSITIVE" )
        return(POSITIVE);
    else if ( value_ == "NEGATIVE" )
        return(NEGATIVE);
    throw(FileError(name()));
}

//=======================
// getValue() Overload11
//=======================
OScopeChannels::Channel SS::getValue(Int2Type<TRIGSOURCE>) {
    // Master file takes precedence
    if ( masterSet_ ) {
        if ( masterValue_ == "EXTERNAL" || masterValue_ == "4" )
            return(OScopeChannels::TRIGGER); // CH4 is trigger channel
        Assert<BadArg>(IsInteger(masterValue_), name());
        return(static_cast<OScopeChannels::Channel>(convert<long>(masterValue_)));
    }

    // Otherwise, specific file
    if ( value_ == "EXTERNAL" || value_ == "4" ) // CH4 is trigger channel
        return(OScopeChannels::TRIGGER);
    Assert<BadArg>(IsInteger(value_), name());
    return(static_cast<OScopeChannels::Channel>(convert<long>(value_)));
}

//=======================
// getValue() Overload12
//=======================
ProgramTypes::SetType SS::getValue(Int2Type<TRIGVERTSCALE>) {
    return(getValue(Int2Type<VERTSCALE>())); // call overload
}

//=======================
// getValue() Overload13
//=======================
ProgramTypes::SetType SS::getValue(Int2Type<VERTSCALE>) {
    std::pair<SType, ScaleUnits<SType>::Units> p;

    // Master file takes precedence    
    if ( masterSet_ ) {
        p = ScaleUnits<SType>::GetUnits(masterValue_);
        return(ScaleUnits<SType>::ScaleUp(p.first, p.second));    
    }

    // Otherwise, specific file
    p = ScaleUnits<SType>::GetUnits(value_);
    return(ScaleUnits<SType>::ScaleUp(p.first, p.second)); 
}

//========
// Name()
//========
std::string SS::Name() {
    return(name());
}

//========
// name()
//========
std::string SS::name() {
    return("Scope Setup File");
}

//==========
// Reload()
//==========
void SS::Reload() {
    Converter* cptr = SingletonType<Converter>::Instance();
    std::string familyNumber = cptr->FamilyNumber();
    sf_.reset(new FileTypes::ScopeSetupFileType(familyNumber));
    locked_ = true;  // must call SetTestName()  
}

//===============
// SetTestName()
//===============
void SS::SetTestName(const std::string& testName) {
    params_.reset(new SF::Parameters(sf_->GetParameters(testName)));
    std::string tName = noNumbers(testName);    
    masterParams_.reset(new SF::Parameters(masterFile_->GetParameters(tName)));
    locked_ = false;
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
