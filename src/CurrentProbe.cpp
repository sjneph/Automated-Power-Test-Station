// Files included
#include "Assertion.h"
#include "CurrentProbe.h"
#include "GenericAlgorithms.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  //===============
  // 03/02/05, sjn,
  //===============
  In preparation of changing the design of the current probe system and consequent
    layout changes:

    Removed Probe enum type
    nameLC() and nameHC_ changed to name()
    Removed WhichProbe(), RescaleUp(), SelectProbe(), RescaleUp(), MaxScale(),
      MinScale() and SetScale().
    Removed CurrentRange argument to MaxAmpsAllowed(); made const
    Changed MaxAmpsAllowed() to a const member function

    Removed activeProbe_    
    Removed addressLC_ and addressHC_
    Removed probeSelected_  
    Removed checkDegauss_    

    // The following is necessary due to new probe: Need to explicitly control time
    //  in between talk/queries to new Tektronix TCPA300 probe or it will crash.
    Added void wait()
    Added waitCalculate();
    Added setAppropriateRangeAndCoupling();
    Added timer_
    Added lastCommand_


   ==============
   01/18/05, sjn,
   ==============
     Removed call to: SingletonType<StationFile>::Instance()->NeedDegauss() in
         Initialize() --> factored out to SPTS::NeedsDegauss()
     Modified RescaleUp() and SetScale() --> use non-static local var for possible
         scales instead of static local --> would have been a problem when switching
         between High Current and Low Current Probes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::BadRtnValue     BadRtnValue;
    typedef StationExceptionTypes::InfiniteLoop    InfiniteLoop;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::OutOfRange      OutOfRange;
    typedef StationExceptionTypes::RescaleError    RescaleError;
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
CurrentProbe::CurrentProbe() : address_(-1), locked_(true), lastError_(""),
                               needDegauss_(false) {
    address_ = Instrument<BT>::getAddress(InstrumentFile::CURRENTPROBE);
}

//============
// Destructor
//============
CurrentProbe::~CurrentProbe() 
{ /* */ }

//=====================
// checkDegaussState()
//=====================
void CurrentProbe::checkDegaussState() {
    // Degaussing needed?
    needDegauss_ = false;
    if ( IsError() ) {
        if ( needDegauss_ )
            lastError_ = "";
        else {
            lastError_ = WhatError();
            throw(InstrumentError(lastError_));
        }
    }    
}

//===========
// command()
//===========
bool CurrentProbe::command(const std::string& cmd) {
    Assert<UnexpectedState>(!(locked_ || cmd.empty()), Name());
    wait();
    bool toRtn = Instrument<BT>::commandInstr(address_, cmd);
    lastCommand_ = cmd;
    return(toRtn);
}

//================
// CurrentScale()
//================
ProgramTypes::SetType CurrentProbe::CurrentScale() {
    return(Language::GetCurrentScale(query(Language::ReadRange())));
}

//==============
// Initialize()
//==============
bool CurrentProbe::Initialize() {
    lastError_ = "";
    try {
        locked_ = false;
        setAppropriateRangeAndCoupling();
    } catch(...) {
        locked_ = true;
        throw;
    }
    return(true);
}

//===========
// IsError()
//===========
bool CurrentProbe::IsError() {
    if ( !lastError_.empty() ) // only cleared by WhatError() and Initialize()
        return(true);

    static std::string degaussDone = Uppercase(Language::DegaussDone());    
    static std::string inNeedOfDegauss = Uppercase(Language::NeedsDegauss());
    std::string error = Uppercase(query(Language::IsError()));
    if ( error == inNeedOfDegauss ) {
        lastError_ = "Probe not degaussed: should be";
        needDegauss_ = true;
    }
    else if ( error == degaussDone )
        return(false);
    else
        lastError_ = "Unknown probe return value";
    return(true);
}

//==================
// MaxAmpsAllowed()
//==================
ProgramTypes::SetType CurrentProbe::MaxAmpsAllowed() const {
    static InstrumentFile::Types type = InstrumentFile::CURRENTPROBE;
    return(SingletonType<InstrumentFile>::Instance()->MaxAmps(type));
}

//========
// Name()
//========
std::string CurrentProbe::Name() const {    
    return("Current Probe");
}

//================
// NeedsDegauss()
//================
bool CurrentProbe::NeedsDegauss() {
    checkDegaussState();
    return(needDegauss_);
}

//=========
// query()
//=========
std::string CurrentProbe::query(const std::string& q) {
    Assert<UnexpectedState>(!(locked_ || q.empty()), Name());
    wait();
    lastCommand_ = q;
    std::string result = Instrument<BT>::queryInstr(address_, q, waitCalculate());
    return(result);
}

//=========
// Reset()
//=========  
bool CurrentProbe::Reset() {
    /*  If you perform a Language::Reset() with Tektronix TCPA300,
         it comes up needing to be deguassed... --> don't do it */
    return(Initialize());
}

//==================================
// setAppropriateRangeAndCoupling()
//==================================
void CurrentProbe::setAppropriateRangeAndCoupling() {
    std::string currentRange = query(Language::ReadRange());
    if ( !Language::IsAppropriateRange(currentRange) )       
        Assert<InstrumentError>(command(Language::ToggleRange()), Name());

    std::string currentCoupling = query(Language::ReadCoupling());
    if ( !Language::IsAppropriateCoupling(currentCoupling) )    
        Assert<InstrumentError>(command(Language::ToggleCoupling()), Name());
}

//=================
// SystemDegauss()
//=================
bool CurrentProbe::SystemDegauss() {
    typedef std::vector<std::string> StrVec;
    command(Language::Degauss());
    checkDegaussState(); // Wait until the probe is actually degaussed before returning
    return(!needDegauss_); // If failed to degauss return false
}

//========
// wait()
//========
void CurrentProbe::wait() {
    try {
        double timeToWait = waitCalculate();
        if ( timeToWait > 0 && timer_.IsTiming() ) {
            static const int MAXLOOPS = 10;
            int cntr = 0;
            while ( timer_.ElapsedTime().Value() < timeToWait ) {
                Assert<InfiniteLoop>(cntr++ < MAXLOOPS, Name());
                Pause(timeToWait / 4);
            } // while
        }
        timer_.Clear();
        timer_.StartTiming();
    } catch(...) {
        timer_.Clear();
        timer_.StartTiming();
        throw;
    }
}

//=================
// waitCalculate()
//=================
double CurrentProbe::waitCalculate() {
    static std::string degaussCommand = Language::Degauss();
    static bool needToWait = 
       static_cast<int>(CurrentProbeTraits::ModelType::REQUIRESSPECIALTIMER);

    double timeToWatch = 0;
    if ( needToWait ) {
        /* degauss treated the same as a total reset */
        if ( lastCommand_.find(degaussCommand) != std::string::npos )
            timeToWatch = CurrentProbeTraits::ModelType::TIMEINSECONDSAFTERDEGAUSS;
        else // non-reset command precedes this one
            timeToWatch = CurrentProbeTraits::ModelType::TIMEINSECONDSAFTERLASTCOMMAND;
    }
    return(timeToWatch);
}

//=============
// WhatError()
//============= 
std::string CurrentProbe::WhatError() {
    std::string toRtn = lastError_;
    lastError_ = "";
    return(toRtn);    
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
