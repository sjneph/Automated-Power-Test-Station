// Files included
#include "Assertion.h"
#include "Functions.h"
#include "InstrumentTypes.h"
#include "Measurement.h"
#include "OScopeParameters.h"
#include "SPTSException.h"
#include "StationAlgorithms.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   06/23/05, sjn,
   ==============
   Modified pre/post measurement functions.  We used to set the input file to 0V when
    a test step's APS's were not used.  0V took on new meaning with the changes from
    05/10/05.  We now check against TestStepInfo::UNDEFINEDSETTYPE to make everything
    robust to tests which actually want to take an APS to 0V from some sequence wide
    value.

   ==============
   05/10/05, sjn,
   ==============
   Added implementations for: IsDUTError(), onException() and WhatDUTError().
   Added errorCode_ to constructor.
   Modified Measure() to use BadMeasurement if DUTErrorDetected().  Removed local
     variables since returnType_ is no longer static.  Modified
     MeasureWithoutPrePostConditions() to use BadMeasurement if DUTErrorDetected().
     All extra measurments made will also equate to BadMeasure following a 
     DUTErrorDetected().
   Relaxed constraints of APS settings in preMeasurement().  Previously, an APS had
     to be set sequence wide or it was allowed to be set for a particular test step
     only.  Now, we will allow an APS to be set sequence wide and varied for any
     particular test step.  At the end of such a step, the supply will be set back to
     its sequence wide setting.

   ==============
   01/20/05, sjn,
   ==============
   Added explicit vertical scope scaling calls to end of postMeasurement() for
     aesthetic reasons (noise on 2mV/div is very noticeable).
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
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::OutOfRange      OutOfRange;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSMeasurement {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//===========================
// Static member definitions
//===========================
SpacePowerTestStation::SPTS* Measurement::spts_ = 0;
Converter* Measurement::dut_ = 0;
bool Measurement::initialized_ = false;
const Measurement::MType Measurement::BadMeasurement = 9.99E37;
const std::string Measurement::BadMeasurementStr = "9.99E37";

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
Measurement::Measurement() : extraMeasures_(new ReturnTypeContainer),
                             errorCode_(TestStepInfo::TestStep::NODUTERROR)
{ /* */ }

//============
// Destructor
//============
Measurement::~Measurement() 
{ /* */ }

//============
// beenDone()
//============
bool Measurement::beenDone(ConditionsPtr, ConditionsPtr) const { 
    // virtual which may be overridden
    return(false); 
}

//=========================
// dealWithRampVinInBase()
//=========================
bool Measurement::dealWithRampVinInBase() const {
    // virtual which may be overridden
    return(true);
}

//===============
// DoneAlready()
//===============
bool Measurement::DoneAlready(ConditionsPtr current, ConditionsPtr previous) { 
    return(beenDone(current, previous));
}

//=====================
// ExtraMeasurements()
//=====================
Measurement::ReturnTypeContainer Measurement::ExtraMeasurements() {
    return(*extraMeasures_);    
}

//===========
// GetName()
//===========
std::string Measurement::GetName() const { 
    return(name());
}

//==============
// initialize()
//==============
void Measurement::initialize() {
    spts_ = SingletonType<SpacePowerTestStation::SPTS>::Instance();
    dut_ = SingletonType<Converter>::Instance();
    initialized_ = true;
}

//==============
// IsDUTError()
//==============
bool Measurement::IsDUTError() const {
    return(errorCode_ != TestStepInfo::TestStep::NODUTERROR);
}

//===========
// Measure()
//===========
Measurement::ReturnType Measurement::Measure(ConditionsPtr conditions, 
                                             const ProgramTypes::PairMType& limits) {
    if ( !initialized_ )
        initialize();

    // Establish preconditions
    preMeasurement(conditions);

    // Make measurement
    try {
        operator()(conditions, limits);
    } catch(SPTSExceptions::DUTCriticalBase& dcb) {
        errorCode_ = dcb.GetExceptionID();
        postMeasurement(conditions);
        onException();
        throw(dcb);
    } catch(SPTSExceptions::DUTBase& db) {
        errorCode_ = db.GetExceptionID();
    }

    // Establish post conditions
    postMeasurement(conditions);
    onException();
    return(returnType_);
}

//===================================
// MeasureWithoutPrePostConditions()
//===================================
Measurement::ReturnType 
            Measurement::MeasureWithoutPrePostConditions(ConditionsPtr conditions, 
                                            const ProgramTypes::PairMType& limits) {
    if ( !initialized_ )
        initialize();

    // Make measurement
    try {
        operator()(conditions, limits);
    } catch(SPTSExceptions::DUTCriticalBase& dcb) {
        errorCode_ = dcb.GetExceptionID();
        throw(dcb);
    } catch(SPTSExceptions::DUTBase& db) {
        errorCode_ = db.GetExceptionID();
    }

    onException();
    return(returnType_);
}

//===============
// onException()
//===============
void Measurement::onException() {
    if ( IsDUTError() ) { // measurement-related DUT diagnostic failure
        std::size_t sz = returnType_.second.size();
        if ( sz > 0 ) { 
            for ( std::size_t i = 0; i < sz; ++i )
                returnType_.second[i] = BadMeasurement;
        }
        else
            returnType_.second.push_back(BadMeasurement);
        sz = extraMeasures_->size();
        for ( std::size_t i = 0; i < sz; ++i ) {
            std::size_t contSize = (*extraMeasures_)[i].second.size();
            RTypeSecond second;
            for ( std::size_t j = 0; j < contSize; ++j )
                second.push_back(BadMeasurement);            
            (*extraMeasures_)[i] = std::make_pair((*extraMeasures_)[i].first, second);
        } // for
    }
}

//===================
// postMeasurement()
//===================
void Measurement::postMeasurement(ConditionsPtr conditions) {

    // Deal with abnormal input line condition
    ProgramTypes::SetType s = spts_->GetVin();
    if ( (s < dut_->LowLine()) || (s > dut_->HighLine()) ) {
        spts_->SafeInhibit(ON);
        spts_->SetVin(dut_->NominalLine());
        spts_->SafeInhibit(OFF);
    }

    // See if any load current is greater than full load
    //   If so, set all load currents to full
    typedef MTypeContainer MC;
    typedef std::vector< std::pair<Switch, SetType> > VP;
    SetTypeContainer fullLoads;
    bool toChange = false;
    MC iouts = dut_->Iouts();
    VP v = spts_->GetLoadValues();
    Assert<UnexpectedState>(v.size() == iouts.size(), GetName());
    MC::iterator i = iouts.begin();
    VP::iterator a = v.begin();    
    while ( i != iouts.end() ) {
        if ( i->Value() < a->second.Value() ) {
            if ( a->first != OFF )
                toChange = true;
        }        
        fullLoads.push_back(i->Value());
        ++i;
        ++a;
    }
    if ( toChange ) { // set all loads to full
        spts_->SetLoad(LoadTraits::ALL, OFF);
        spts_->SetLoad(fullLoads);
    }

    // Deal with sync input
    if ( conditions->SyncIn() ) 
        spts_->ResetSync();

    // Deal with primary aux supply
    std::pair<bool, ProgramTypes::SetType> setSupply;
    setSupply = VarFile::Instance()->PrimaryAuxSupply();
    if ( conditions->APSPrimary() != TestStepInfo::UNDEFINEDSETTYPE ) { // reset supply
        if ( setSupply.first ) { // set back to sequence-wide value
            spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY, ON);
            spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY,
                          SpacePowerTestStation::APS::VOLTS,
                          setSupply.second);
        }
        else { // back to zero
            spts_->SafeInhibit(ON);
            spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY, OFF);
            spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY, 
                          SpacePowerTestStation::APS::VOLTS,
                          0);
            spts_->SafeInhibit(OFF);
        }
    }

    // Deal with secondary aux supply
    setSupply = VarFile::Instance()->SecondaryAuxSupply();
    if ( conditions->APSSecondary() != TestStepInfo::UNDEFINEDSETTYPE ) { // reset
        if ( setSupply.first ) { // set back to sequence-wide value
            spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY, ON);
            spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY,
                          SpacePowerTestStation::APS::VOLTS,
                          setSupply.second);
        }
        else { // back to zero
            spts_->SafeInhibit(ON);
            spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY, OFF);
            spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY,
                          SpacePowerTestStation::APS::VOLTS,
                          0);            
            spts_->SafeInhibit(OFF);
        }
    }

    // Deal with a primary inhibit condition
    if ( conditions->PrimaryInhibited() )
        spts_->SafeInhibit(OFF);

    // Deal with a secondary inhibit condition
    if ( conditions->SecondaryInhibited() ) {
        spts_->SafeInhibit(ON);
        spts_->ResetPath(ControlMatrixTraits::RelayTypes::SECONDARYINHIBIT);
        spts_->SafeInhibit(OFF);
    }

    // Custom reset all relays/controls
    spts_->ResetPath();

    // Set scope channels to explicit vertical scaling for aesthetic reasons
    SpacePowerTestStation::ExplicitScope::ExplicitParms ep =
                                       SpacePowerTestStation::ExplicitScope::VERTSCALE;
    spts_->SetScopeExplicit(OScopeChannels::ONE, ep, 100e-3);
    spts_->SetScopeExplicit(OScopeChannels::TWO, ep, 100e-3);
    spts_->SetScopeExplicit(OScopeChannels::THREE, ep, 100e-3);
    spts_->SetScopeExplicit(OScopeChannels::TRIGGER, ep, 100e-3);
}

//==================
// preMeasurement()
//==================
void Measurement::preMeasurement(ConditionsPtr conditions) {

    // Get pause value for all optional initial conditions
    PauseStates* ps = SingletonType<PauseStates>::Instance();        
    SetType optionPause = ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS);
    SetType mPause = ps->GetPauseValue(PauseStates::MISCELLANEOUSINITIALCONDITIONS);

    // Set vin and iout values
    spts_->SetVin(conditions->Vin());
    spts_->SetLoad(conditions->Iouts());

    // static local --> does not change from converter-2-converter
    static SetType syncPause = ps->GetPauseValue(PauseStates::SYNCINPUT);

    // Deal with a sync input signal
    if ( conditions->SyncIn() ) {
        spts_->SafeInhibit(ON);
        spts_->SetSync(
            conditions->Freq(),
            dut_->SyncAmplitude(),
            dut_->SyncDutyCycle(),
            dut_->SyncOffset()                
                      );     
        Pause(syncPause);
        spts_->SafeInhibit(OFF);
        Pause(syncPause / SetType(2));
    }
    
    // Deal with load shorts
    if ( !conditions->Shorted().empty() ) {
        SpacePowerTestStation::Short(conditions->Shorted(), ON);
        Pause(optionPause);
        // Measure each applicable vout and ensure < 'maxShortVout'
        static const ProgramTypes::MType maxShortVout = 1;
        std::vector<ConverterOutput::Output>::const_iterator i, j;
        i = conditions->Shorted().begin(), j = conditions->Shorted().end();
        const std::string onError = "Vout too high during short circuit.  ";       
        while ( i != j ) {
            std::string ifError = onError;
            ProgramTypes::MTypeContainer vouts;
            SpacePowerTestStation::MeasureVoutDC(vouts, *i);
            ifError += ("Vout" + convert<std::string>(*i));
            ifError += ("  Measured: " + convert<std::string>(absolute(vouts[0])));
            ifError += "V";
            Assert<OutOfRange>(absolute(vouts[0]) <= maxShortVout, name(), ifError);         
            ++i;
        } // while
    }

    // Deal with primary aux supply
    if ( conditions->APSPrimary() != TestStepInfo::UNDEFINEDSETTYPE ) {
        spts_->SafeInhibit(ON);
        spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY, 
                      SpacePowerTestStation::APS::VOLTS,
                      conditions->APSPrimary());
        spts_->SetAPS(SpacePowerTestStation::APS::PRIMARY, ON);
        Pause(optionPause);
        spts_->SafeInhibit(OFF);
    }

    // Deal with secondary aux supply
    if ( conditions->APSSecondary() != TestStepInfo::UNDEFINEDSETTYPE ) {
        spts_->SafeInhibit(ON);
        spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY,
                        SpacePowerTestStation::APS::VOLTS,
                        conditions->APSSecondary());
        spts_->SetAPS(SpacePowerTestStation::APS::SECONDARY, ON);
        Pause(optionPause);
        spts_->SafeInhibit(OFF);
    }

    // Deal with any pre-Misc lines that need to be set
    std::set<ControlMatrixTraits::RelayTypes::MiscRelay> preMisc = 
                                                    conditions->PretestMisc();
    std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> toPass;
    std::copy(preMisc.begin(), preMisc.end(), std::back_inserter(toPass));
    if ( !toPass.empty() ) {
        spts_->SafeInhibit(ON);
        Pause(mPause);
        spts_->SetPath(toPass);
        Pause(mPause);
        spts_->SafeInhibit(OFF);
    }

    // Cannot both primary and secondary inhibit the dut
    Assert<FileError>(!(conditions->PrimaryInhibited() && 
                        conditions->SecondaryInhibited()),
                        GetName());                  

    // Deal with a primary inhibit condition
    if ( conditions->PrimaryInhibited() ) {
        spts_->StrongInhibit(ON);            
        Pause(optionPause);
    }

    // Deal with a secondary inhibit condition
    if ( conditions->SecondaryInhibited() ) {
        spts_->SetPath(ControlMatrixTraits::RelayTypes::SECONDARYINHIBIT);
        Pause(optionPause);
    }

    // Deal with a ramping input voltage condition, if applicable
    if ( conditions->VinRamp() && dealWithRampVinInBase() ) {
        Assert<BadCommand>(conditions->VinNext() != TestStepInfo::UNDEFINEDSETTYPE, 
                           GetName());
        spts_->SetVin(conditions->VinNext());
        Pause(optionPause);
        Pause(optionPause);
    }
}

//================
// WhatDUTError()
//================
long Measurement::WhatDUTError() const {
    return(errorCode_);
}

} // namespace SPTSMeasurement

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
