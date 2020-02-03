// Files included
#include "Assert.h"
#include "ConfigureRelays.h"
#include "Converter.h"
#include "ConverterOutput.h"
#include "Functions.h"
#include "InstrumentTypes.h"
#include "MainSupplyTraits.h"
#include "Measurement.h"
#include "LoadTraits.h"
#include "OperatorInterface.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "StandardFiles.h"
#include "StandardStationFiles.h"
#include "StationAlgorithms.h"
#include "Switch.h"
#include "SPTS.h"
#include "SwitchMatrixTraits.h"
#include "TestFixtureFile.h"
#include "VariablesFile.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
	10/27/2009 MRB
		Altered MeasureVoutDC to check if VariablesFile allows use of Load Meter.

	08/09/2006 HQP
		Altered RampVin() to support older (first generation) products.

	05/16/2006 MRB
		Altered MeasureIoutDC to check if VariablesFile allows use of Load Meter.

   ==============
   12/30/05, sjn,
   ==============
     Modified WaitSoakDUT() to try to minimize overall impact of soaking for minutes per
       gen-029.  Soaking places the DUT temperature too close to maximum upper limit,
       when testing at temperature extremes, prior to beginning of actual test.  Changed
       routine so that we are within +/-1 oC only after a temperature soak.

   ==============
   11/20/05, sjn,
   ==============
     Added InstrumentSetup typedef.  Added VerifyPowerConnection() routine.  This
       allows us to determine if the input power pins are making contact with the DUT
       when supported by the test fixture and the VAR file indicates the check.

   ==============
   10/11/05, sjn,
   ==============
     Modified RampVin() to call SPTS::SetVin() with second parameter --> cannot use
       DMM to measure Vin when ramping input voltage because relay already set in the
       station.  The DMM is only used with certain power supplies that cannot measure
       the input voltage reliably themselves (older supplies).

   ==============
   05/10/05, sjn,
   ==============
     Removed CheckVoutsAtInhibit() --> in StationAlgorithms.template
     Modified InitializeStation(): Moved check of degauss needs to before the check for
      any station errors --> technicality with new current probe which cannot tell you
      if it has any errors, only if it needs to be degaussed.  Removed sequence wide
      setup of APS's --> handled via SPTS class now.
     Added PostSequenceReset() to help isolate consecutive test sequences.

   ==============
   03/20/05, sjn,
   ==============
     Added WaitSoakDUT().  Modified EnforceTemperature() to call WaitSoakDUT() when
      required per GEN-029.   

   ==============  
   09/23/04, sjn,
   ==============
     Modified EnumChecker() to reflect that OScopeChannels::FOUR no longer exists - it
      is now OScopeChannels::TRIGGER.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    // Dut Exceptions
    typedef DUTExceptionTypes::HighIin    HighIin;

    // Station Exceptions
    typedef StationExceptionTypes::BadArg              BadArg;
    typedef StationExceptionTypes::BadIinShuntValue    BadIinShuntValue;
    typedef StationExceptionTypes::BadRtnValue         BadRtnValue;
    typedef StationExceptionTypes::BlownFuse           BlownFuse;
    typedef StationExceptionTypes::ContainerState      ContainerState;
    typedef StationExceptionTypes::FileError           FileError;
    typedef StationExceptionTypes::InhibitCurrentHigh  InhibitCurrentHigh;
    typedef StationExceptionTypes::InstrumentError     InstrumentError;
    typedef StationExceptionTypes::InstrumentSetup     InstrumentSetup;
    typedef StationExceptionTypes::ProgramError        ProgramError;
    typedef StationExceptionTypes::TemperatureOverflow TemperatureOverflow;
    typedef StationExceptionTypes::TemperatureTimeout  TemperatureTimeout;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    using SpacePowerTestStation::SPTS;
    using SPTSInstrument::InstrumentTypes;

    // File typedefs
    typedef ProgramTypes::MType   MType;
    typedef ProgramTypes::SetType SetType;

    // File variables
    enum TempType { BASE, DUT };
    SPTS* stationPtr        = 0;
    Converter* dutPtr       = 0;
    InstrumentFile* iPtr    = 0;
    VariablesFile* varPtr   = 0;
    TestFixtureFile* fixPtr = 0;
    StationFile* staFilePtr = 0;

    // name()
    const std::string& name() {
        static std::string name = "Station Algorithms";
        return(name);
    }

    // AbsDiff structure
    template <typename Type>
    struct AbsDiff {
        Type operator()(const Type& t1, const Type& t2) {
            Type toRtn = t1 - t2;
            if ( toRtn < static_cast<Type>(0) )
                toRtn *= static_cast<Type>(-1);
            return(toRtn);
        }
    };

    struct EnumChecker {
        // Basic enumeration checks which ensure the program will
        //  run as expected
        typedef LoadTraits LT;
        typedef ConverterOutput CO;
        typedef OScopeChannels OS;
        EnumChecker() {
            Assert<ProgramError>(
                (static_cast<long>(LT::ONE)     == static_cast<long>(CO::ONE))   &&
                (static_cast<long>(LT::TWO)     == static_cast<long>(CO::TWO))   &&
                (static_cast<long>(LT::THREE)   == static_cast<long>(CO::THREE)) &&
                (static_cast<long>(LT::FOUR)    == static_cast<long>(CO::FOUR))  &&
                (static_cast<long>(LT::FIVE)    == static_cast<long>(CO::FIVE))  &&
                (static_cast<long>(OS::ONE)     == static_cast<long>(CO::ONE))   &&
                (static_cast<long>(OS::TWO)     == static_cast<long>(CO::TWO))   &&
                (static_cast<long>(OS::THREE)   == static_cast<long>(CO::THREE)) &&
                (static_cast<long>(OS::TRIGGER) == static_cast<long>(CO::FOUR)),
                "Program Enumeration Checks " + name()
                                );
        }
    };
    static EnumChecker checkEnums;
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SpacePowerTestStation {

//==================
// DUTOrientation()
//==================
bool DUTOrientation() {

    if ( ! fixPtr->Symmetrical() ) // No need to check
        return(true);
    ProgramTypes::MType maxOhms = 8000; // arbitrary default
    if ( varPtr->GetOrientationOhms().first ) // override default
        maxOhms = varPtr->GetOrientationOhms().second;
    stationPtr->SetDMM(maxOhms.Value(), SPTSInstrument::DMM::OHMS);

    // Switch statement accounts for possibility of converter output1
    //   being mapped to a load channel which is not channel one.
    typedef SwitchMatrixTraits::RelayTypes RT;
    RT::DCRelay voutRelay = RT::VOUTDC1;
    switch(stationPtr->Convert2LoadChannel(ConverterOutput::ONE)) {
        case LoadTraits::ONE:   voutRelay = RT::VOUTDC1; break;
        case LoadTraits::TWO:   voutRelay = RT::VOUTDC2; break;
        case LoadTraits::THREE: voutRelay = RT::VOUTDC3; break;
        case LoadTraits::FOUR:  voutRelay = RT::VOUTDC4; break;
        case LoadTraits::FIVE:  voutRelay = RT::VOUTDC5; break;
    };
     
    // Make measurement and determine orientation
    long counter = -1;
    static const long maxTries = 2;
    try {
        stationPtr->SetPath(voutRelay);    
        while ( ++counter < maxTries ) {
            if ( stationPtr->MeasureOhms() < maxOhms )
                break;
        }
        stationPtr->ResetPath(voutRelay);
        stationPtr->SetDMM(); // Back to default DCV mode   
    } catch(...) {
        stationPtr->ResetPath(voutRelay);
        stationPtr->SetDMM(); // Back to default DCV mode 
        throw;
    }

    return(counter < maxTries); 
}

//======================
// EnforceTemperature()
//======================
void EnforceTemperature(bool initialTemperature) {
    
    // Grab tolerance information
    std::pair<MType, MType> tolerance = 
                                staFilePtr->GetTemperatureTolerance(initialTemperature);
    MType minTolerance = absolute(tolerance.first);                
    MType maxTolerance = absolute(tolerance.second);

    if ( minTolerance > maxTolerance )
        std::swap(minTolerance, maxTolerance);

    // Get target temperature and current temperature info        
    MType targetTemperature = static_cast<MType>(varPtr->GetTemperature());
    MType currentTemperature;
    try {
        stationPtr->SetDMM(SPTSInstrument::DMM::AUTO, SPTSInstrument::DMM::TEMP);
        currentTemperature = stationPtr->MeasureDUTTemperature();
    } catch(...) {
        stationPtr->SetDMM(); // Auto Range, DCV mode
        throw;
    }
    stationPtr->SetDMM(); // Auto Range, DCV mode
    Assert<TemperatureOverflow>((currentTemperature < MType(200)) && 
                                (currentTemperature > MType(-200)), 
                                 name());

    // Locals
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
    bool atHot = false, atCold = false;

    // Get room temperature
    static const MType roomTemperature = GetRoomTemperature();

    // Determine (targetTemperature) +/- tolerances
    MType lowerLimit, upperLimit;
    if ( targetTemperature > roomTemperature ) { // hot
        lowerLimit = targetTemperature - minTolerance;
        upperLimit = targetTemperature + maxTolerance;
        atHot = true;
    }
    else if ( targetTemperature < roomTemperature ) { // cold
        lowerLimit = targetTemperature - maxTolerance;
        upperLimit = targetTemperature + minTolerance;
        atCold = true;
    }
    else { // room
        lowerLimit = targetTemperature - absolute(tolerance.first);
        upperLimit = targetTemperature + absolute(tolerance.second);
    }

    // See if we're within tolerance
    if ( (currentTemperature <= upperLimit) && (currentTemperature >= lowerLimit) ) {
        if ( atHot )
            oi->ShowAtTemperature(HOT);
        else if ( atCold )
            oi->ShowAtTemperature(COLD);
        else
            oi->ShowAtTemperature(ROOM);

        if ( initialTemperature )
            WaitSoakDUT();

        return; // we're within tolerance
    }

    // Not within tolerance --> either heat up or cool down
    bool loadsOn = false;
    typedef std::vector< std::pair<Switch, SetType> > LoadValues;
    LoadValues loadValues;
    if ( currentTemperature < targetTemperature ) { // heat up
        oi->ShowWarmingTemperature();
        loadValues = stationPtr->GetLoadValues();
        ProgramTypes::MTypeContainer iouts = dutPtr->Iouts();
        ProgramTypes::MTypeContainer::iterator i = iouts.begin(), j = iouts.end();
        ProgramTypes::SetTypeContainer fullLoads;
        while ( i != j ) {
            fullLoads.push_back(i->Value());
            ++i;
        } // while
        stationPtr->SetLoad(fullLoads); 
        loadsOn = true;
    }
    else { // cool down
        stationPtr->SafeInhibit(ON);
        oi->ShowCoolingTemperature();
    }
    
    try {
        long counter = -1, maxCounter = 200;
        bool done = false;        
        SetType pauseBetweenMeasures = 3; // in seconds
        stationPtr->SetDMM(SPTSInstrument::DMM::AUTO, SPTSInstrument::DMM::TEMP);
        while ( !done ) { // max time = maxCounter * pauseBetweenMeasures
            Assert<DUTExceptionTypes::TestAborted>(!oi->DidAbort(), name());
            Assert<TemperatureTimeout>(++counter < maxCounter, name());
            Pause(pauseBetweenMeasures);
            currentTemperature = stationPtr->MeasureDUTTemperature();
            if ( currentTemperature <= upperLimit ) {
                if ( currentTemperature >= lowerLimit )
                    break;
            }
        }
    } catch(...) {
        stationPtr->SetDMM(); // Auto Range, DCV mode
        throw;
    }
    stationPtr->SetDMM(); // Auto Range, DCV mode
   
    // Restore system to original state
    if ( loadsOn ) { // restore load states
        LoadValues::iterator i = loadValues.begin();
        SPTS::LoadChannels loadChannels = stationPtr->GetLoadChannels();
        Assert<UnexpectedState>(loadChannels.size() == loadValues.size(), name());
        SPTS::LoadChannels::iterator a = loadChannels.begin();
        while ( i != loadValues.end() ) {
            stationPtr->SetLoad(*a, i->second); // old load value
            if ( !i->first ) // was OFF 
                stationPtr->SetLoad(*a, OFF);          
            ++a;
            ++i;
        } // while
    }
    else // restore inhibit
        stationPtr->SafeInhibit(OFF);

    // Show user we're at temperature
    if ( atHot )
        oi->ShowAtTemperature(HOT);
    else if ( atCold )
        oi->ShowAtTemperature(COLD);
    else
        oi->ShowAtTemperature(ROOM);

    if ( initialTemperature )
        WaitSoakDUT();
}

//==================
// FixtureIDCheck()
//==================
bool FixtureIDCheck() {
    SingletonType<TestFixtureFile>::Instance()->SetFixture(varPtr->Fixture());
    ProgramTypes::MType id = fixPtr->IDResistor();
    ProgramTypes::MType tolerance = fixPtr->IDResistorTolerance();
    stationPtr->SetDMM(SPTSInstrument::DMM::AUTO, SPTSInstrument::DMM::OHMS);
    stationPtr->SetPath(SwitchMatrixTraits::RelayTypes::BOXID);
    long counter = -1, rllCounter = -1;
    while ( ++counter < 2 ) {
        ProgramTypes::MType value = stationPtr->MeasureOhms();
        if ( absolute(value - id) < tolerance )
            break;
    }
    stationPtr->ResetPath(SwitchMatrixTraits::RelayTypes::BOXID);

    // If using RLL feature, check for appropriate RLL fixture
    if ( stationPtr->LoadType() == LoadTraits::PASSIVE ) { 
        id = fixPtr->RLLIDResistor();
        tolerance = fixPtr->RLLIDResistorTolerance();
        stationPtr->SetPath(SwitchMatrixTraits::RelayTypes::RESBOXID);
        while ( ++rllCounter < 2 ) {
            ProgramTypes::MType value = stationPtr->MeasureOhms();
            if ( absolute(value - id) < tolerance )
                break;
        }
        stationPtr->ResetPath(SwitchMatrixTraits::RelayTypes::RESBOXID);
    }
    stationPtr->SetDMM(); // Back to default DCV mode
    return((counter < 2) && (rllCounter < 2));
}

//========================
// InitializeAlgorithms()
//========================
void InitializeAlgorithms() {
    // This order of initialization should be maintained
    stationPtr = SingletonType<SPTS>::Instance();
    iPtr       = SingletonType<InstrumentFile>::Instance();
    dutPtr     = SingletonType<Converter>::Instance();
    varPtr     = SingletonType<VariablesFile>::Instance();
    fixPtr     = SingletonType<TestFixtureFile>::Instance();
    staFilePtr = SingletonType<StationFile>::Instance();
}

//=============================
// InitializeBaseTemperature()
//=============================
void InitializeBaseTemperature(
                                const SetType& toTemp,
                                const SetType& posTolerance,
                                const SetType& negTolerance
                              ) {
    // Check arguments                          
    Assert<BadArg>(posTolerance > 0, name());
    Assert<BadArg>(negTolerance < 0, name());    

    // Determine (tolerance)
    SetType tolerance;
    if ( posTolerance.Value() >= absolute(negTolerance) )
        tolerance = posTolerance;
    else
        tolerance = absolute(negTolerance);
    
    // Measure temperature and calculate (difference)            
    SetType currentTemp = stationPtr->MeasureBaseTemp().Value();
    SetType difference, zero = 0;
    if ( currentTemp >= zero ) {
        if ( toTemp >= zero )
            difference = absolute(toTemp - currentTemp);
        else // toTemp < zero
            difference = absolute(absolute(toTemp) + currentTemp);
    }
    else { // currentTemp < zero
        if ( toTemp >= zero )
            difference = absolute(toTemp + absolute(currentTemp));
        else // toTemp < zero
            difference = absolute(toTemp - currentTemp);
    } // if

    // Determine if to recurse --> only done on flatleaded parts, if applicable
    SetType maxDiff = 25;
    static bool recurse = true;
    if ( (difference > maxDiff) && recurse ) { // Recurse first to a pre-final-temp value
        if ( ! SingletonType<TestFixtureFile>::Instance()->DownLeaded() ) {        
            SetType currentTemp = stationPtr->MeasureBaseTemp().Value(); 
            try {
                recurse = false;
                SetType recurseTemp = 0;
                if ( currentTemp < toTemp ) // heating up      
                    recurseTemp = toTemp - maxDiff;
                else // currentTemp > toTemp --> cooling down
                    recurseTemp = toTemp + maxDiff;
                InitializeBaseTemperature(recurseTemp, posTolerance, negTolerance);
                recurse = true;
            } catch(...) {
                recurse = true;
                throw;
            }
        } // if
    } // if
 
    // Go to temperature
    bool first = true;
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
    long cntr = 0, maxLoop = 200, pauseTime = 3; // timeout value = pauseTime * maxLoop
    stationPtr->SetTemperatureBase(toTemp);
    while ( difference > tolerance ) {       
        Assert<DUTExceptionTypes::TestAborted>(!oi->DidAbort(), name());
        Assert<TemperatureTimeout>(++cntr < maxLoop, name());

        if ( first ) { // Update interface to show ramping temperature
            if ( toTemp > currentTemp ) // heating
                oi->ShowWarmingTemperature();
            else // cooling
                oi->ShowCoolingTemperature();
            first = false;
        }
        currentTemp = stationPtr->MeasureBaseTemp().Value();                                   
        if ( currentTemp >= zero ) {
            if ( toTemp >= zero )
                difference = absolute(toTemp - currentTemp);
            else // toTemp < zero
                difference = absolute(absolute(toTemp) + currentTemp);
        }
        else { // currentTemp < zero
            if ( toTemp >= zero )
                difference = absolute(toTemp + absolute(currentTemp));
            else // toTemp < zero
                difference = absolute(toTemp - currentTemp);
        } // if
        Pause(pauseTime);
    } // Loop while not within tolerance
}

//=====================
// InitializeStation()
//=====================
void InitializeStation(bool resetTemp) {
    // Initialize
    stationPtr->Initialize(resetTemp);

    // Degauss if necessary
    if ( stationPtr->NeedsDegauss() ) {
        stationPtr->ProbeDegauss();
        Assert<InstrumentError>(!stationPtr->IsError(), "Degauss");
    }

    // Any station errors?
    if ( stationPtr->IsError() ) {
        std::pair<InstrumentTypes::Types, std::string> p;
        p = stationPtr->WhatError();
        std::string toReport = iPtr->GetModelType(p.first); 
        toReport += (": " + p.second);
        throw(InstrumentError(toReport));
    }
}

//================
// MeasureIinDC() 
//================
ProgramTypes::MType MeasureIinDC(IinDCType type) {

    MType toRtn;
    StationFile* s = SingletonType<StationFile>::Instance();  
    std::pair<SwitchMatrixTraits::RelayTypes::DCRelay, 
              std::pair<StationFile::IinDCBoard, StationFile::IinShunt> > pair;
    pair = stationPtr->GetInputShuntInfo();
    SwitchMatrixTraits::RelayTypes::DCRelay relay = pair.first;
    StationFile::IinDCBoard board = pair.second.first;
    StationFile::IinShunt shunt = pair.second.second;

    switch(type) {        
        case NOINHIBIT:
            stationPtr->SetPath(relay);
            toRtn = stationPtr->MeasureDCV();
            stationPtr->ResetPath(relay);
            toRtn /= (s->GetShuntValue(board, shunt));
            break;

        case ISINHIBITED: // Get best resolution without inhibiting DUT
            toRtn = MeasureIinDC(NOINHIBIT); // recurse
            // make sure it's safe to select BIGOHM
            Assert<InhibitCurrentHigh>(toRtn < s->MaxCurrentValue(board, 
                                                             StationFile::BIGOHM));
            stationPtr->SetIinShunt(StationFile::BIGOHM);
            toRtn = MeasureIinDC(NOINHIBIT); // recurse
            stationPtr->SetIinShunt(StationFile::SMALLOHM);
            break;

        default: // DYNAMIC             
            // Measure current using shunt already set
            toRtn = MeasureIinDC(NOINHIBIT); // recurse                      
 
            // See if we can get better resolution
            bool toChange = false;
            StationFile::IinShunt nextShunt = StationFile::SMALLOHM;
            if ( toRtn < s->MaxCurrentValue(board, StationFile::BIGOHM) ) {
                if ( shunt == StationFile::BIGOHM )      
                    return(toRtn); // we're done already
                toChange = true;
                nextShunt = StationFile::BIGOHM;       
            }
            else if ( toRtn < s->MaxCurrentValue(board, StationFile::MIDOHM) ) { 
                if ( shunt == StationFile::MIDOHM )
                    return(toRtn); // we're done already  
                else if ( shunt == StationFile::BIGOHM )
                    throw(BadIinShuntValue());
                toChange = true;
                nextShunt = StationFile::MIDOHM;
            }   
            else if ( shunt != StationFile::SMALLOHM )
                throw(BadIinShuntValue());
     
            if ( toChange ) { // Set best resolution
                stationPtr->SafeInhibit(ON);
                stationPtr->SetIinShunt(nextShunt);
                stationPtr->SafeInhibit(OFF);
                toRtn = MeasureIinDC(NOINHIBIT); // recurse
                stationPtr->SafeInhibit(ON); // Now reset back to lowest ohm shunt
                stationPtr->SetIinShunt(StationFile::SMALLOHM);
                stationPtr->SafeInhibit(OFF);
            }                 
    };
    return(toRtn);
}

//=================
// MeasureIoutDC() 
//=================
void MeasureIoutDC(ProgramTypes::MTypeContainer& iouts, 
                   ConverterOutput::Output output, bool loadMeasure) {
    typedef SwitchMatrixTraits::RelayTypes SMR;
    std::vector<ConverterOutput::Output> outputs;
    std::vector<ConverterOutput::Output>::iterator i, j;

	if(!SingletonType<Converter>::Instance()->UseLoadMeter()){
		loadMeasure = SingletonType<Converter>::Instance()->UseLoadMeter();
	}
	SMR::DCRelay relay = SMR::IOUTDC1;
    LoadTraits::Channels fakeChan, realChan;
	switch(output) {
        case ConverterOutput::ALL: // measure all outputs
            outputs = SingletonType<Converter>::Instance()->Outputs();
            i = outputs.begin(); j = outputs.end();
            while ( i != j ) {
                MeasureIoutDC(iouts, *i, loadMeasure); // recurse
                ++i;
            }
			break;		

		default:
            // Take care of the possibility that IoutX of the converter
            //  is not mapped to physical load channel X.  In that case,
            //  we must trick TestFixtureFile::IoutShuntValue() to believe
            //  that it is mapped from X->X to get the correct output
            //  shunt value.  This keeps people from needing to trick
            //  the program through text file manipulation --> Iout1 Shunt
            //  Value can remain Iout1 even if mapped to load channel 3,
            //  for example.  As a side note, InitializeAlgorithms()'s 
            //  enumeration check ensures the following cast is valid.
            fakeChan = static_cast<LoadTraits::Channels>(output);
            realChan = stationPtr->Convert2LoadChannel(output);
            TestFixtureFile* tf = SingletonType<TestFixtureFile>::Instance();
			switch(realChan) {
				case LoadTraits::ONE:   relay = SMR::IOUTDC1; break;
				case LoadTraits::TWO:   relay = SMR::IOUTDC2; break;
				case LoadTraits::THREE: relay = SMR::IOUTDC3; break;
				case LoadTraits::FOUR:  relay = SMR::IOUTDC4; break;
				case LoadTraits::FIVE:  relay = SMR::IOUTDC5; break;
			}; // Inner switch

        // Pause
        typedef SingletonType<PauseStates> PS;
        Pause(PS::Instance()->GetPauseValue(PauseStates::IOUTDC));

        // Use load for measurement if loadMeasure is set and using ELoad -
        //   otherwise, use DMM
        if ( loadMeasure && (stationPtr->LoadType() == LoadTraits::ELECTRONIC) ) {
            LoadTraits::Channels c = stationPtr->Convert2LoadChannel(output);        
            iouts.push_back(stationPtr->MeasureLoadCurrent(c));
        } 
        else { // Use DMM
            stationPtr->SetPath(relay);
            stationPtr->SetDMM(); // Auto range, DCV mode by default
			iouts.push_back(stationPtr->MeasureDCV() / tf->IoutShuntValue(fakeChan));
            stationPtr->ResetPath(relay);        
        }
	}; // Outer switch
}

//================
// MeasureVinDC() 
//================
ProgramTypes::MType MeasureVinDC() {
    ProgramTypes::MType mt = SingletonType<TestFixtureFile>::Instance()->VinMultiplier();
    stationPtr->SetPath(SwitchMatrixTraits::RelayTypes::INPUTVOLTAGE);
    stationPtr->SetDMM(dutPtr->HighestVinSeen() / mt.Value());
    ProgramTypes::MType measured = stationPtr->MeasureDCV();
    measured *= mt;
    stationPtr->ResetPath(SwitchMatrixTraits::RelayTypes::INPUTVOLTAGE);	
    stationPtr->SetDMM(); // To auto range
	return(measured);
}

//=================
// MeasureVoutDC() 
//=================
void MeasureVoutDC(ProgramTypes::MTypeContainer& vouts, 
                   ConverterOutput::Output output, bool loadMeasure) {    
    typedef SwitchMatrixTraits::RelayTypes SMR;
    typedef std::vector<LoadTraits::Channels> LoadChannels;
    SMR::DCRelay relay = SMR::VOUTDC1;
    std::vector<ConverterOutput::Output> outputs;
    std::vector<ConverterOutput::Output>::iterator i, j;

	if(!SingletonType<Converter>::Instance()->UseLoadMeter()){
		loadMeasure = SingletonType<Converter>::Instance()->UseLoadMeter();
	}

	switch(output) {
		case ConverterOutput::ALL:  // Recursively call function for each output
            outputs = SingletonType<Converter>::Instance()->Outputs();
            i = outputs.begin(); j = outputs.end();
            while ( i != j ) {
                MeasureVoutDC(vouts, *i, loadMeasure);
                ++i;
            }
			break;

		default:  // Any specific channel
            LoadTraits::Channels c = stationPtr->Convert2LoadChannel(output);
            switch(c) {
                case LoadTraits::ONE:   relay = SMR::VOUTDC1; break;
                case LoadTraits::TWO:   relay = SMR::VOUTDC2; break;
                case LoadTraits::THREE: relay = SMR::VOUTDC3; break;
                case LoadTraits::FOUR:  relay = SMR::VOUTDC4; break;
                case LoadTraits::FIVE:  relay = SMR::VOUTDC5; break;
            }; // Inner switch

            // Pause
            typedef SingletonType<PauseStates> PS;
            Pause(PS::Instance()->GetPauseValue(PauseStates::VOUTDC));

            // Use Load unless loadMeasure is false or not using electronic loading
            if ( loadMeasure && (stationPtr->LoadType() == LoadTraits::ELECTRONIC) ) {
                ProgramTypes::MType mult = 1;
                if ( SingletonType<Converter>::Instance()->Vout(output) < 0 )
                    mult *= -1;
                vouts.push_back(stationPtr->MeasureLoadVolts(c) * mult);
            }
            else { // Use DMM
                stationPtr->SetPath(relay);
                stationPtr->SetDMM(); // auto by default
			    vouts.push_back(stationPtr->MeasureDCV());
                stationPtr->ResetPath(relay);
            } // if-else
	}; // Outer switch
}

//=====================
// PostSequenceReset()
//=====================
void PostSequenceReset() {
    // Reset Primary/Secondary APS's if used in last test sequence
    stationPtr->SetAPS(APS::PRIMARY, OFF);
    stationPtr->SetAPS(APS::PRIMARY, APS::VOLTS, 0);
    stationPtr->SetAPS(APS::SECONDARY, OFF);
    stationPtr->SetAPS(APS::SECONDARY, APS::VOLTS, 0);

    // Reset all misc io lines
    stationPtr->ResetAllMiscIO();
}

//===========
// RampVin()
//===========
void RampVin() {
    // Locals
    std::string name = "Ramp Vin";
    SetType lowLine = dutPtr->LowLine(), nomLine = dutPtr->NominalLine();
    SetType numSteps = 5, counter = 0;
    SetType inSteps  = lowLine / numSteps;
    SetType iinMax   = dutPtr->MaxIinNoLoad();

    // Ensure safe limits using arbitrary multiples
    Assert<BadRtnValue>(iinMax > SetType(0) && (iinMax < SetType(1.5)), name); // 08/09/2006 HQP
    SetType psLimit = iinMax * SetType(3.0);// 08/09/2006 HQP
    iinMax *= 2;
    stationPtr->SetDMM(psLimit);

    // Make sure the station does not have any errors
    if ( stationPtr->IsError() ) {
        std::pair<InstrumentTypes::Types, std::string> p;
        p = stationPtr->WhatError();
        std::string toReport = iPtr->GetModelType(p.first); 
        toReport += (" " + p.second);
        throw(InstrumentError(toReport));
    }

    // Limit power supply current and select smallest ohm shunt
    bool worked = true;
    stationPtr->SetIinLimit(psLimit);
    stationPtr->SetIinLimitProtection(ON);
    stationPtr->SetIinShunt(StationFile::SMALLOHM); 

    // Grab shunt information
    MType iinMeasured;
    StationFile* s = SingletonType<StationFile>::Instance();
    std::pair< SwitchMatrixTraits::RelayTypes::DCRelay,
               std::pair<StationFile::IinDCBoard, StationFile::IinShunt> > pair;
    pair = stationPtr->GetInputShuntInfo();
    SwitchMatrixTraits::RelayTypes::DCRelay relay = pair.first;
    StationFile::IinDCBoard board = pair.second.first;
    StationFile::IinShunt shunt = pair.second.second;

    // Set path to measure input current
    stationPtr->SetPath(relay);
    stationPtr->SetLoad(LoadTraits::ALL, OFF);

    bool cannotUseDMM = false, mayUseDMM = true;
    while ( ++counter <= numSteps ) { // continue ramping input voltage
        try {
            // May throw exception if P/S current limits
            stationPtr->SetVin(inSteps * counter, cannotUseDMM);
        } catch(...) {
            throw(HighIin(name));
        }
        iinMeasured = stationPtr->MeasureDCV() / (s->GetShuntValue(board, shunt));
        if ( absolute(iinMeasured) > iinMax ) { // high iin dc
            worked = false; 
            break;
        }
    } // while
   
    // Ensure no overcurrent condition
    stationPtr->ResetPath(relay);
    stationPtr->SetDMM(); // Auto Range
    Assert<HighIin>(worked, name);

    // Set to nominal line and ensure no power supply trips
    stationPtr->SetVin(nomLine, mayUseDMM);
    MType two = 2;
    Assert<HighIin>(nomLine - two < MeasureVinDC(), name);

    // Set BIGOHM explicitly and see if any input fuses are blown
    try {        
        stationPtr->SetIinShunt(StationFile::BIGOHM);
        Assert<BlownFuse>(nomLine - two < MeasureVinDC(), name);
        stationPtr->SetIinShunt(StationFile::SMALLOHM);
    } catch(...) {
        stationPtr->SafeInhibit(ON); // leave inhibited
        stationPtr->SetIinShunt(StationFile::SMALLOHM);
        throw;
    }

    // Set current to max
    stationPtr->SetIinLimitMax();
}

//=========
// Short()
//=========
void Short(const std::vector<ConverterOutput::Output>& v, Switch state) {
    std::vector<ConverterOutput::Output>::const_iterator i = v.begin();
    std::vector<ControlMatrixTraits::RelayTypes::OutputRelay> relays;

    // Push each relay corresponding to each ConverterOutput
    while ( i != v.end() ) {
        LoadTraits::Channels c = stationPtr->Convert2LoadChannel(*i);
        switch(c) {
            case LoadTraits::ONE:
                relays.push_back(ControlMatrixTraits::RelayTypes::SHORT1);
                break;
            case LoadTraits::TWO:
                relays.push_back(ControlMatrixTraits::RelayTypes::SHORT2);
                break;
            case LoadTraits::THREE:
                relays.push_back(ControlMatrixTraits::RelayTypes::SHORT3);
                break;
            case LoadTraits::FOUR:
                relays.push_back(ControlMatrixTraits::RelayTypes::SHORT4);
                break;
            case LoadTraits::FIVE:
                relays.push_back(ControlMatrixTraits::RelayTypes::SHORT5);
        };
        ++i;
    }

    if ( relays.empty() ) // nothing to do
        return;

    if ( state == ON )
        stationPtr->SetPath(relays);
    else
        stationPtr->ResetPath(relays);
}

//=========================
// VerifyPowerConnection()
//=========================
std::pair<bool, ProgramTypes::MType> VerifyPowerConnection(
                                                const ProgramTypes::MType& maxOhms
                                                          ) {
    // The idea is to send a small current through our fixture and verify the
    //  integrity of the power connection.  Threshold values given by Frank Carter,
    //  Senior Test Engineer.
    std::pair<bool, std::string> toCheck = fixPtr->GetPowerConnectionMiscLine();
    if ( !toCheck.first )
        return(std::make_pair(true, 0)); // do not perform check on this DUT
    static const ProgramTypes::MType maxVinMeasure = 1;
    static const ProgramTypes::SetType vinSet = 0.1;
    static const std::string badVin = "Vin too high to check power connection";
    static const std::string badConn = "Bad input power connection detected";
    static const ProgramTypes::SetType psLimit = 500e-3; // just enough current
    static const ProgramTypes::SetType resetPSLimit = 100e-3;
    stationPtr->SetVin(0);
    stationPtr->SetIinLimit(resetPSLimit);
    Assert<InstrumentSetup>(MeasureVinDC() < maxVinMeasure, badVin);
    ControlMatrixTraits::RelayTypes::MiscRelay powerMisc = 
                                                ConvertToMisc(toCheck.second);
    PauseStates* ps = SingletonType<PauseStates>::Instance();        
    ProgramTypes::SetType psPause = 
                   ps->SupplyCCModeChangePause(stationPtr->WhichSupply());
    SwitchMatrixTraits::RelayTypes::DCRelay relay;
    MainSupplyTraits::Supply type = stationPtr->WhichSupply();
    if ( type == MainSupplyTraits::PS1 )
        relay = SwitchMatrixTraits::RelayTypes::PS1VOLTS;
    else if ( type == MainSupplyTraits::PS2 )
        relay = SwitchMatrixTraits::RelayTypes::PS2VOLTS;
    else
        relay = SwitchMatrixTraits::RelayTypes::PS3VOLTS;

    bool result = true;
    const ProgramTypes::MType zero = 0, two = 2;
    ProgramTypes::MType open = 1000;
    open.SetPrecision(0);
    ProgramTypes::MType totalResistance = open;
    try {
        // Measure Vin, Iin, and the voltage at the power supply
        //  terminals --> calculate total path resistance.
        stationPtr->SetPath(powerMisc);
        stationPtr->SetIinLimit(psLimit);
        stationPtr->SetVin(vinSet);
        Pause(psPause);
        ProgramTypes::MType iin = MeasureIinDC(NOINHIBIT);       
        ProgramTypes::MType vin = MeasureVinDC();
        stationPtr->SetPath(relay);
        ProgramTypes::MType vSource = stationPtr->MeasureDCV();
        stationPtr->SetVin(0);
        stationPtr->SetIinLimit(resetPSLimit);
        stationPtr->ResetPath(powerMisc);
        stationPtr->ResetPath(relay);
        Assert<InstrumentSetup>(vSource > zero, badConn);
        Assert<InstrumentSetup>(vin > zero, badConn);
        Assert<InstrumentSetup>(vSource > vin, badConn);
        Assert<InstrumentSetup>(iin > zero, badConn);
        totalResistance = (vSource - vin) / iin;
        totalResistance.SetPrecision(3); // mohm range
        if ( totalResistance >= two ) // iin is very small --> open input
            totalResistance = open;
        Assert<InstrumentSetup>(totalResistance <= maxOhms, badConn);
        Assert<InstrumentSetup>(totalResistance > zero, badConn);
    } catch(InstrumentSetup&) {
        stationPtr->SetVin(0);
        stationPtr->SetIinLimit(resetPSLimit);
        stationPtr->ResetPath(powerMisc);
        stationPtr->ResetPath(relay);
        result = false;
    } catch(...) {
        stationPtr->SetVin(0);
        stationPtr->SetIinLimit(resetPSLimit);
        stationPtr->ResetPath(powerMisc);
        stationPtr->ResetPath(relay);
        throw;
    }
    return(std::make_pair(result, totalResistance));
}

//===============
// WaitSoakDUT()
//===============
void WaitSoakDUT() {
    static const MType DUTSOAKTIME = 2 * 60; // defined in GEN-029, in seconds
    static const MType ABSOLUTEMAXDUTSOAKTIME = DUTSOAKTIME * 1.5;
    static const long PAUSEBETWEENENCHECK = 1; // seconds
    static const SetType room = GetRoomTemperature();
    if ( varPtr->GetTemperature() == room )
        return; // at room temperature --> no soak required
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();

    // Wait until DUTSOAKTIME has elapsed while maintaining temperature
    Clock timer;
    timer.StartTiming();
    const MType targetTemperature = static_cast<MType>(varPtr->GetTemperature());
    const MType upperLimit = targetTemperature + 1;
    const MType lowerLimit = targetTemperature - 1;
    typedef std::vector< std::pair<Switch, SetType> > LoadValues;
    LoadValues loadValues = stationPtr->GetLoadValues(); // initial load values
    oi->ShowSoaking(true);
    try {
        stationPtr->SetDMM(SPTSInstrument::DMM::AUTO, SPTSInstrument::DMM::TEMP);
        ProgramTypes::MTypeContainer iouts = dutPtr->Iouts(); // full load values
        ProgramTypes::MTypeContainer::iterator i, j;
        i = iouts.begin(), j = iouts.end();
        ProgramTypes::SetTypeContainer halfLoads;
        while ( i != j ) {
            halfLoads.push_back(i->Value() / 2);
            ++i;
        } // while
        stationPtr->SetLoad(halfLoads);
        MType currentTemperature = stationPtr->MeasureDUTTemperature();
        bool heatingUp = true;
        while ( /* having soaked long enough or not within upper/lower limits */
                timer.ElapsedTime() < DUTSOAKTIME ||
                currentTemperature < lowerLimit   ||
                currentTemperature > upperLimit
              ) { /* ElapsedTime() will only grow... */

            // Get target temperature and current temperature info
            Assert<DUTExceptionTypes::TestAborted>(!oi->DidAbort(), "DUT Soaking");
            if ( timer.ElapsedTime() > ABSOLUTEMAXDUTSOAKTIME ) // no infinite loop
                break; // unable to get back to within upper/lower limits...            
            currentTemperature = stationPtr->MeasureDUTTemperature();
            
            if ( currentTemperature < lowerLimit ) { // heat up
                oi->ShowWarmingTemperature();
                if ( !heatingUp ) // we're inhibited
                    stationPtr->SafeInhibit(OFF);                
                heatingUp = true;
            }
            else if ( currentTemperature > upperLimit ) { // cool down
                oi->ShowCoolingTemperature();
                if ( heatingUp ) // inhibit DUT
                    stationPtr->SafeInhibit(ON);
                heatingUp = false;
            }
            Pause(PAUSEBETWEENENCHECK);
            currentTemperature = stationPtr->MeasureDUTTemperature();
        } // while
    } catch(...) {
        stationPtr->SetDMM(); // Auto Range, DCV mode
        oi->ShowSoaking(false);
        // Restore system to original state
        stationPtr->SafeInhibit(OFF);
        LoadValues::iterator i = loadValues.begin();
        SPTS::LoadChannels loadChannels = stationPtr->GetLoadChannels();
        Assert<UnexpectedState>(loadChannels.size() == loadValues.size(), name());
        SPTS::LoadChannels::iterator a = loadChannels.begin();
        while ( i != loadValues.end() ) {
            stationPtr->SetLoad(*a, i->second); // old load value
            if ( !i->first ) // was OFF 
                stationPtr->SetLoad(*a, OFF);          
            ++a;
            ++i;
        } // while
        throw;
    }
    stationPtr->SetDMM(); // Auto Range, DCV mode
    oi->ShowSoaking(false);

    // Restore system to original state
    stationPtr->SafeInhibit(OFF);
    LoadValues::iterator i = loadValues.begin();
    SPTS::LoadChannels loadChannels = stationPtr->GetLoadChannels();
    Assert<UnexpectedState>(loadChannels.size() == loadValues.size(), name());
    SPTS::LoadChannels::iterator a = loadChannels.begin();
    while ( i != loadValues.end() ) {
        stationPtr->SetLoad(*a, i->second); // old load value
        if ( !i->first ) // was OFF 
            stationPtr->SetLoad(*a, OFF);          
        ++a;
        ++i;
    } // while
}

} // namespace SpacePowerTestStation

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
