// Files included
#include "Assertion.h"
#include "ControlMatrixTraits.h"
#include "DialogBox.h"
#include "FilterSelects.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "MeasurementFunctions.h"
#include "OscopeParameters.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "StandardFiles.h"
#include "StationAlgorithms.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
	
	=============
	12/08/08, reb
	=============
		Begin variation on Cross Regulation tests to not include Full Load in worst case calculation.
		Testing needs to only include the specified load values.
		(MeasurementFunctions.cpp, MeasurementFunctions.h, MeasurementRegistration.cpp)
		

	=============
	04/02/08, mrb
	=============
		Modified Load Transient Response().  Changed scope scale function to slowly zoom in to accurately read signals

	=============
	9/02/07, mrb
	=============
		Modified Load Transient Response().  Added second chance to rescale scope.

	=============
	08/10/07, mrb
	=============
		Added the test InhibitCycleTest()

	=============
	03/28/07, MRB
	=============
		Redid what Frank and Hieu had done last year.  Seems we had two copies
		of the software and the changes did not get added to both copies.  I have also 
		incorporated changes that were made on the official version of the software to 
		allow for specific Pard measurements to be chosen.

   ==============
   03/24/06, hqp,fac
   ==============
     Modified TurnOnDelay::performTest() to force the measurement
	 channel to be the trigger channel.
	 Both measurement slopes must be the same when type = SHORT
	 One caveat is when we ever trigger this event using an external source.
	 We may have to undo all these changes.

   ==============
   10/26/05, sjn,
   ==============
     Modified LowLineDropout() slightly --> when changing the input voltage, may use a
       DMM to verify we are at steady state Vin instead of the power supply's reading.
       This is to circumvent issues with older power supplies being unable to give an
       accurate readout during voltage transitions.
     Modified TurnOnDelay::perform() --> set trigger channel to bottom of oscope screen
       --> allow rising trigger that won't clip on oscilloscope even when there is
       drastic overshoot on the primary inhibit line.

     Design Note:  A couple of revisions ago, we got rid of the external trigger on
       oscilloscopes, and instead use a regular oscilloscope channel.  Unlike external
       triggering, the waveforms can now "clip".  This presents a fundamental problem
       to the VinRampDelay() test --> some oscilloscopes do not have (1:1) scale
       factors large enough --> clipping can occur on higher input voltages.  This
       would not have been an issue with external triggering.  Similarly,
       SCReleaseDelay() has this potential problem.  Both of the tests mentioned are
       unofficially deprecated on the SPTS anyway.  These tests should only be used
       in extremely special cases.

   ==============
   05/10/05, sjn,
   ==============
     Added check of APS Primary/Secondary to every implementation of beenDone()
     Added implementation for LoadTransientResponse::performTest(), by copying
       everything over from the previous LoadTransientResponse::operator()
       implementation.  operator() has been modified to call performTest() a second time
       if any scope scaling or scope measurement error occurs.  Since we were unable to
       successfully synchronize the multiple oscilloscopes we must support, the pause
       times defined externally may not be conservative enough in all cases (just
       turned the scope on, etc.).  This new implementation will help reduce the number
       of bad measurement errors thrown by the software.  It will still occur if the s/w
       detects the problem more than once. 
     Similarly, a TurnOnDelay::performTest() method has been implemented for the same
       reasons as LoadTransientResponse::performTest().

   ==============
   03/07/05, sjn,
   ==============
     Modified IinPARD() to reflect current probe design and station layout changes.
     Removed try-catch since you can no longer select auto-rescale with the current
       probe.

   ==============
   12/07/04, sjn,
   ==============
     LoadTransientResponse() --> made default load transition time 10us instead of MAX:
        Loads are getting faster and may one day be too fast for our applications.
     Added comments to Load Transient test.
     Added spts_->WaitOnScope() to the TurnOnDelay func object just before the trigger
        event.  This will affect SC Delay/Overshoot, TOD/Overshoot, Vin Ramp Delay and
        Overshoot.  It makes our code more robust against different scope models with
        different setup times.
     Removed unused local vars from LineRegulation().
     Removed speedup capabilities for VoutPARD.  Removed VoutPARD::beenDone().  The
        station has been re-designed and will no longer be able to perform parallel
        oscilloscope measurements.
     Modified LoadTransientResponse() and TurnOnDelay() --> Remove explicit filter
        path selection type as HUNDREDKHZ is no longer an explicit option with the
        new station layout (the hardware makes it explicit).
     Modified Frequency(), IinPARD() and VoutPARD() --> got rid of HIGHIMPEDANCE
        explicit scope settings --> does not apply to new station layout.
     Modified VoutPARD(), TurnOnDelay(), LoadTransientResponse() --> Changed all calls
        to SPTS::Convert2ScopeChannel() to SPTS::GetScopeChannel().
     Changed the default recovery time marker for TurnOnDelay, SCReleaseDelay and
        VinRampDelay to 95% from 90% (Interpoint Engineering's decision).
     Modified the scope setup files for this station so that the trigger source is
        always defined externally rather than in specific measurement functions,
        which simplifies the software and is now possible due to the re-layout of the
        station.  This resulted in changes to IinPARD(), VoutPARD(), Frequency().
     Modified LLDO, IoutTripPoint and Frequency:  DUT Sanity Checks are now an option
        instead of a requirement.
     Frequency modified to tolerate severe DUT oscillations and invalid signals.
     Modified Efficiency and PowerDissipation: When measuring input current, use the
        currently selected shunt rather than dynamically find the best resolution
        shunt.  We sometimes place 1 ohm in series with a converter at full load at
        present --> causing oscillation problems on some units.  The consequence is
        some reduced resolution, but the overall effect should be minimal.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
  

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    using SpacePowerTestStation::MeasureIinDC;
    using SpacePowerTestStation::MeasureIoutDC;
    using SpacePowerTestStation::MeasureVinDC;
    using SpacePowerTestStation::MeasureVoutDC;
    namespace StationNS = SpacePowerTestStation;


    struct BadConditionTag {
        static std::string Name() {
            return("Bad Test Condition Detected");
        }
    };
    
    struct NoMeasurementTypeTag {
        static std::string Name() {
            return("No Measurement Defined");
        }
    };

    typedef StationExceptionTypes::BaseException      StationBaseException;

    // Station-related exceptions
    typedef StationExceptionTypes::BadArg             BadArg;
    typedef StationExceptionTypes::BadCommand         BadCommand;
    typedef StationExceptionTypes::BadRtnValue        BadRtnValue;
    typedef StationExceptionTypes::ContainerState     ContainerState;
    typedef StationExceptionTypes::InfiniteLoop       InfiniteLoop;
    typedef StationExceptionTypes::MeasurementError   MeasurementError;
    typedef StationExceptionTypes::MeasurementTimeout MeasurementTimeout;
    typedef StationExceptionTypes::RescaleError       RescaleError;
    typedef StationExceptionTypes::ScopeMeasure       ScopeMeasureError;
    typedef StationExceptionTypes::UnexpectedState    UnexpectedState;
    typedef StationExceptionTypes::VinTolerance       VinTolerance;
    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          StationBaseException, 
                                          BadConditionTag
                                         > BadCondition;
    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          StationBaseException, 
                                          NoMeasurementTypeTag
                                         > NoMeasurementType;


    // DUT-related exceptions
    typedef DUTExceptionTypes::BaseException DUTBaseException;

    typedef DUTExceptionTypes::JumperPull    JumperPull;
    typedef DUTExceptionTypes::NoTripPoint   NoTripPoint;
    typedef DUTExceptionTypes::Overshoot2Big Overshoot;
    typedef DUTExceptionTypes::VoutAtFreq    VoutAtFreq;
    typedef DUTExceptionTypes::Undershoot    Undershoot;
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSMeasurement {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed

    //==============================
    // Supporting Functions/Classes 
    //==============================
    
    //=========
    // AbsDiff
    //=========
    template <typename R> 
    struct AbsDiff {
        AbsDiff() { /* */ }
	    R operator()(R r1, R r2) {
            static R negate = -1, zero = 0;
            if ( r1 < zero ) r1 *= negate;
            if ( r2 < zero ) r2 *= negate;
            return(r1 > r2 ? r1-r2 : r2-r1);
	    }
    };

    //===============
    // append2Name()
    //===============
    std::string append2Name(const std::string& name, ConverterOutput::Output out) {
        return(name + convert<std::string>(out));
    }

    //========
    // MaxVal
    //========
    template <typename R> 
    struct MaxVal {
	    R operator() (const R& r1, const R& r2) {
		    return((r1 >= r2) ? r1 : r2);
	    }
    };

    //===============
    // makeRtnType()
    //===============
    Measurement::ReturnType makeRtnType(const Measurement::RTypeFirst& first, 
                                                  const Measurement::MType& second) {
	    Measurement::RTypeSecond container;
	    container.push_back(second);
	    return(std::make_pair(first, container));
    }

} // unnamed namespace




/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===================
// CaseTemperature()
//===================
void CaseTemperature::operator()(ConditionsPtr, const PairMType&) {
    try {
        spts_->SetDMM(SPTSInstrument::DMM::AUTO, SPTSInstrument::DMM::TEMP);
        returnType_ = makeRtnType(Name(), spts_->MeasureDUTTemperature());
        spts_->SetDMM(); // Auto Range, DCV mode
    } catch(...) {
        spts_->SetDMM(); // Auto Range, DCV mode
        throw;
    }
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===================
// CrossRegulation()
//===================
void CrossRegulation::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
	MTypeContainer voutsInit, voutsRef, voutsFinal;

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsInit, ConverterOutput::ALL) :
		MeasureVoutDC(voutsInit, conditions->Channel());

	// Set to full load value(s) for reference
    MTypeContainer miouts = dut_->Iouts();
    SetTypeContainer siouts;
    MTypeContainer::iterator i = miouts.begin(), j = miouts.end();
    while ( i != j ) {
        siouts.push_back(i->Value());
        ++i;
    }
	spts_->SetLoad(siouts);

    // Measure Vout(s)
    conditions->Speedup() ?
        MeasureVoutDC(voutsRef, ConverterOutput::ALL) :
        MeasureVoutDC(voutsRef, conditions->Channel());

	// Set to final load value(s)
	spts_->SetLoad(conditions->IoutsNext());

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsFinal, ConverterOutput::ALL) :
		MeasureVoutDC(voutsFinal, conditions->Channel());
	Assert<ContainerState>(voutsInit.size() == voutsFinal.size(), name());
	Assert<ContainerState>(voutsInit.size() == voutsRef.size(), name());
	
	// Calculate cross regulation(s) - choose worst case regulation(s) 
	RTypeSecond holdDifference;
	std::transform(voutsInit.begin(), voutsInit.end(), voutsRef.begin(), 
		           voutsInit.begin(), AbsDiff<MType>());
	std::transform(voutsFinal.begin(), voutsFinal.end(), voutsRef.begin(),
		           voutsFinal.begin(), AbsDiff<MType>());
	std::transform(voutsInit.begin(), voutsInit.end(), voutsFinal.begin(),
		           std::back_inserter(holdDifference), MaxVal<MType>());
	returnType_ = std::make_pair(name(), holdDifference);
}

//============
// beenDone()
//============
bool CrossRegulation::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Cross Reg and decide if we can speed up the test
	return( 
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
		     (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())       
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===================
// CrossRegulationXX()
//===================
void CrossRegulationXX::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
	MTypeContainer voutsInit, voutsRef, voutsFinal;

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsInit, ConverterOutput::ALL) :
		MeasureVoutDC(voutsInit, conditions->Channel());

	
// remove this section for Cross Regulation Test with only specified load values
	// Set to full load value(s) for reference
    MTypeContainer miouts = dut_->Iouts();
    SetTypeContainer siouts;
    MTypeContainer::iterator i = miouts.begin(), j = miouts.end();
    while ( i != j ) {
        siouts.push_back(i->Value());
        ++i;
    }
	/*spts_->SetLoad(siouts);
	don't set to full load */

    // Measure Vout(s)
    conditions->Speedup() ?
        MeasureVoutDC(voutsRef, ConverterOutput::ALL) :
        MeasureVoutDC(voutsRef, conditions->Channel());

/**/
	// Set to final load value(s)
	spts_->SetLoad(conditions->IoutsNext());

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsFinal, ConverterOutput::ALL) :
		MeasureVoutDC(voutsFinal, conditions->Channel());
	Assert<ContainerState>(voutsInit.size() == voutsFinal.size(), name());
	Assert<ContainerState>(voutsInit.size() == voutsRef.size(), name());
	
	// Calculate cross regulation(s) - choose worst case regulation(s) 
	RTypeSecond holdDifference;
	std::transform(voutsInit.begin(), voutsInit.end(), voutsFinal.begin(), 
		           voutsInit.begin(), AbsDiff<MType>());
/*	std::transform(voutsInit.begin(), voutsInit.end(), voutsRef.begin(), 
		           voutsInit.begin(), AbsDiff<MType>());
	std::transform(voutsFinal.begin(), voutsFinal.end(), voutsRef.begin(),
		           voutsFinal.begin(), AbsDiff<MType>());
	std::transform(voutsInit.begin(), voutsInit.end(), voutsFinal.begin(),
		           std::back_inserter(holdDifference), MaxVal<MType>());
*/
	std::transform(voutsInit.begin(), voutsInit.end(), voutsInit.begin(),
		           std::back_inserter(holdDifference), MaxVal<MType>());
	returnType_ = std::make_pair(name(), holdDifference);
}

//============
// beenDone()
//============
bool CrossRegulationXX::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Cross Reg and decide if we can speed up the test
	return( 
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
		     (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())       
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==============
// Efficiency()
//==============
void Efficiency::operator()(ConditionsPtr, const PairMType&) { 

	// Measure inputs - calculate input power
    MType pin = (MeasureVinDC() * MeasureIinDC(StationNS::NOINHIBIT));

	// Measure outputs
	MTypeContainer vouts, iouts;
	MeasureVoutDC(vouts, ConverterOutput::ALL);
    MeasureIoutDC(iouts, ConverterOutput::ALL);	

	// Take absolute values of all output measurements
    Assert<ContainerState>(iouts.size() == vouts.size(), name());
    for ( std::size_t i = 0; i < iouts.size(); ++i ) {
        iouts[i] = absolute(iouts[i]); 
        vouts[i] = absolute(vouts[i]);
    }

	// Calculate output power
	MType pout = std::inner_product(iouts.begin(), iouts.end(), 
	                                vouts.begin(), static_cast<MType>(0));  
	
	// Calculate efficiency 
    Assert<MeasurementError>(pin > pout, Name());
	returnType_ = makeRtnType(name(), pout / pin * static_cast<MType>(100));
}
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=============
// Frequency()
//=============
void Frequency::operator()(ConditionsPtr conditions, const PairMType& limits) {
	// typedefs
    typedef StationNS::ACPathTypes ACPaths;
    typedef ACPaths::ExplicitPaths ExplicitPaths;
    typedef ACPaths::ImplicitPaths ImplicitPaths;
    typedef std::pair<OScopeChannels::Channel, ACPaths::ExplicitPaths> Second;
    typedef std::vector< std::pair<std::string, Second> > CType;
    typedef SpacePowerTestStation::SPTS::ImplicitPaths ImplicitPaths;
    typedef std::vector<ConverterOutput::Output> Outs;
    typedef StationNS::SPTS::LoadChannels LoadChannels;

	// Variable declarations
	long minNumberSources = 3;
	CType sources;
	std::string syncOut  = "Sync Out", 
                voutPard = VoutPARD::Name(),
                iinPard  = IinPARD::Name();
    Second secondType;
    ExplicitPaths ep;

	// Determine multiple Frequency sources that can/will be used
    if ( dut_->HasSyncOut() ) {
        ep = ACPaths::SYNCOUT;
        secondType = std::make_pair(spts_->GetScopeChannel(ep), ep);
		sources.push_back(std::make_pair(syncOut, secondType));
    }

    // Push on Iin PARD as a source to use
	if (!dut_->SkipIinPard()){
	    ep = ACPaths::IINPARD;
		secondType = std::make_pair(spts_->GetScopeChannel(ep), ep);
		sources.push_back(std::make_pair(iinPard, secondType));
	}

	// Push on Vout PARD sources
    Outs outs = dut_->Outputs();
    Outs::const_iterator i = outs.begin(), j = outs.end();
    while ( i != j ) {
        ep = spts_->Convert2ExplicitPath(ACPaths::VOUTPARD, *i);
        secondType = std::make_pair(spts_->GetScopeChannel(ep), ep);
        sources.push_back(std::make_pair(append2Name(voutPard, *i), secondType));
        ++i;
    }
    
    // Push on Iin PARD as needed to ensure minNumberSources is met
    while ( static_cast<long>(sources.size()) < minNumberSources ) {
		if(dut_->SkipIinPard()){
		    Outs outs = dut_->Outputs();
		    Outs::const_iterator i = outs.begin(), j = outs.end();
		    while ( i != j ) {
		        ep = spts_->Convert2ExplicitPath(ACPaths::VOUTPARD, *i);
		        secondType = std::make_pair(spts_->GetScopeChannel(ep), ep);
		        sources.push_back(std::make_pair(append2Name(voutPard, *i), secondType));
		        ++i;
		    }
		}else{
			ep = ACPaths::IINPARD;
			secondType = std::make_pair(spts_->GetScopeChannel(ep), ep);
			sources.push_back(std::make_pair(iinPard, secondType));
		}
	}
    // More local variables
    bool splitter = dut_->FrequencySplitter();
    std::set<OScopeSetupFile::Parameters> unsetParms;
    MType minLimit = limits.first, maxLimit = limits.second;
    MType ideal = ((minLimit + maxLimit) / MType(2));
    MType measured = 0, closest = 0;
    bool done = false;
    FilterSelects::FilterType filter = FilterSelects::TWOMHZ;
    FilterSelects::FilterType syncFilter = FilterSelects::PASSTHRU;
    CType::const_iterator start = sources.begin(), stop = sources.end();

    // Set horizontal scale explicitly based upon limits - formula from Frank Carter
    //   0.X, where X is the number of periods on screen
    SetType horzScale = (0.2 / minLimit.Value()); // need greater or equal to this
    SetTypeContainer scales = 
                     SingletonType<InstrumentFile>::Instance()->ScopeHorizontalScales();
    SetTypeContainer::iterator iter = 
               std::find_if(scales.begin(), scales.end(), 
                            std::bind2nd(std::greater_equal<SetType>(), horzScale));
    Assert<ContainerState>(iter != scales.end(), Name());
    spts_->SetScopeExplicit(StationNS::ExplicitScope::HORZSCALE, *iter);

    // Measure
    while ( start != stop ) {        
        // Set path to next source
        OScopeChannels::Channel scopeChan = start->second.first;
        ExplicitPaths path = start->second.second;
        std::string nextSource = start->first;
        std::string scopeSet = name() + " - " + nextSource;

        try {
            FilterSelects::FilterType thisFilter; // filter line for this measurement

            // Sync out requires a midtest miscellaneous line to be set
            if ( path == ACPaths::SYNCOUT ) {
                std::set<ControlMatrixTraits::RelayTypes::MiscRelay> midMisc = 
                                                               conditions->MidtestMisc();
                std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> midTest;
                std::copy(midMisc.begin(), midMisc.end(), std::back_inserter(midTest));
                Assert<BadCondition>(!midTest.empty(), Name());
                spts_->SetPath(midTest);
                thisFilter = syncFilter;
            }
            else // not sync out
                thisFilter = filter;
 
            // Set path to scope
            spts_->SetPath(path, thisFilter);

            // Make sure explicit and non-explicit settings cover everything
            std::size_t numberExplicitTests = 1;
	        unsetParms = spts_->SetScope(scopeSet, scopeChan);
            Assert<FileError>(numberExplicitTests == unsetParms.size(), Name());
            Assert<FileError>(unsetParms.find(OScopeSetupFile::HORSCALE) != 
                              unsetParms.end(), Name());

            // Make measure
            spts_->WaitOnScope();
            try {
                spts_->RescaleScope(scopeChan);
            } catch(DUTExceptionTypes::SevereOscillation&) {
                /* nada */
            }

            // If fails, then must pass twice in a row to pass
            bool failed = false;
            long numTriesPerSignal = 3;
            for ( long idx = 0; idx < numTriesPerSignal; ++idx ) {

                try {
                    bool toPause = false, toRescale = true;
                    measured = spts_->MeasureScope(OScopeMeasurements::FREQUENCY,
                                                   scopeChan, toPause, toRescale);
                } catch(ScopeMeasureError&) { // suppress non-measures
                    measured = -1;
                } catch(RescaleError&) { // suppress clipping problems
                    measured = -1;
                }

                if ( splitter ) { // deal with any frequency splitters
                    if ( voutPard.find(voutPard) != std::string::npos )
                        measured /= MType(2);
                }

                if ( (measured >= minLimit) && (measured <= maxLimit) ) {
                    if ( ! failed ) { // done
                        returnType_ = makeRtnType(name(), measured);
                        done = true;
                    }
                    failed = false;
                }
                else { // must pass once more
                    if ( absolute(measured-ideal) < absolute(closest-ideal) )
                        closest = measured; // mark closest reading to (ideal) on fails
                    failed = true;
                }                                   

                if ( done ) 
                    break;
            } // for
            spts_->ResetPath(path, filter);

            // Sync Out requires a midtest miscellaneous line to be reset
            if ( path == ACPaths::SYNCOUT ) {
                std::set<ControlMatrixTraits::RelayTypes::MiscRelay> midMisc = 
                                                               conditions->MidtestMisc();
                std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> midTest;
                std::copy(midMisc.begin(), midMisc.end(), std::back_inserter(midTest));
                Assert<BadCondition>(! midTest.empty(), Name());
                spts_->ResetPath(midTest); 
            }
        } catch(...) {
            spts_->ResetPath(path, filter);   
            throw;
        }               
        
        if ( done ) // passed 
            break;
        ++start;
    } // while
    
    if ( ! done )
        returnType_ = makeRtnType(name(), closest);

    VariablesFile* varPtr = SingletonType<VariablesFile>::Instance();
	if ( conditions->SyncIn() && varPtr->DoDUTSanityChecks() )
		StationNS::CheckVouts<VoutAtFreq>(PercentType(20)); // arbitrary %
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=========================
// GenericVoltsMeasureDC()
//=========================
void GenericVoltsMeasureDC::operator()(ConditionsPtr conditions, 
                                       const PairMType&) {
    Assert<BadArg>(1 == conditions->MiscDMM().size(), Name());
	spts_->SetPath((conditions->MiscDMM())[0]);
    spts_->SetDMM(); // Auto-mode
	returnType_ = makeRtnType(name(), spts_->MeasureDCV());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=========
// IinDC()
//=========
void IinDC::operator()(ConditionsPtr conditions, const PairMType&) {
    if ( conditions->IsInhibited() ) // use BIGOHM without using inhibit
        returnType_ = makeRtnType(name(), MeasureIinDC(StationNS::ISINHIBITED));
    else if ( ! conditions->Shorted().empty() ) // shorted --> don't use inhibit
        returnType_ = makeRtnType(name(), MeasureIinDC(StationNS::NOINHIBIT));
    else // Use best resolution possible
	    returnType_ = makeRtnType(name(), MeasureIinDC(StationNS::DYNAMIC));
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===========
// IinPARD()
//===========
void IinPARD::operator()(ConditionsPtr conditions, const PairMType&) {
    // Locals
    typedef StationNS::ACPathTypes Path;
    OScopeChannels::Channel scopeChan = spts_->GetScopeChannel(Path::IINPARD);
	
	// Set up path to oscilloscope
    typedef StationNS::ACPathTypes AC;
	spts_->SetPath(AC::IINPARD, conditions->BW());		    
    MType measured;    

    // File scope settings for this test
	std::set<OScopeSetupFile::Parameters> unsetParms = spts_->SetScope(name(), 
                                                                       scopeChan);

    // Scale scope for resolution and make measurement
    spts_->RescaleScope(scopeChan);
    measured = spts_->MeasureScope(OScopeMeasurements::PEAK2PEAK, scopeChan);
    measured *= spts_->GetCurrentProbeScale();    

	// Store measurement
    Assert<MeasurementError>(measured > MType(0), Name());
	returnType_ = makeRtnType(name(), measured);
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==========
// Inhibit Cycle Test()
//==========
void InhibitCycleTest::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
    Assert<BadArg>(conditions->Channel() != ConverterOutput::ALL, Name());
	MTypeContainer voutsPretest, voutsInit, voutsFinal;

	spts_->SetLoadModes(LoadTraits::CR);

	// Pause        
    typedef SingletonType<PauseStates> PS;
	long Cycles = SingletonType<Converter>::Instance()->InhibitLifeCycle();
	int Completed = 0;
    MeasureVoutDC(voutsPretest, conditions->Channel());
	for(int count = 0; count<Cycles; count++){
	    MeasureVoutDC(voutsInit, conditions->Channel());
		spts_->StrongInhibit(ON);
	    Pause(PS::Instance()->GetPauseValue(PauseStates::INHIBITPULSEWIDTH));
		spts_->StrongInhibit(OFF);
		MeasureVoutDC(voutsFinal, conditions->Channel());
	    Pause(PS::Instance()->GetPauseValue(PauseStates::INHIBITPULSEWIDTH));
		try{
			Assert<ContainerState>(voutsInit.size() == voutsFinal.size(), name());
			if(abs(voutsPretest.back()-voutsFinal.back()) >conditions->RefVal().Value()){
				break;
			};
			if(abs(voutsInit.back()-voutsFinal.back()) >conditions->RefVal().Value()){
				break;
			};
			Completed = 1 + count;
		}catch(...){
			break;
		}
	}

	spts_->SetLoadModes(LoadTraits::CC);

	returnType_ = makeRtnType(name(), Completed);
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==========
// IoutDC()
//==========
void IoutDC::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
    Assert<BadArg>(conditions->Channel() != ConverterOutput::ALL, Name());
	RTypeSecond iouts;
        
    // Shorted?
    bool useLoadMeter = conditions->Shorted().empty();

    // Measure Iout(s)
    conditions->Speedup() ?
        MeasureIoutDC(iouts, ConverterOutput::ALL, useLoadMeter) :
        MeasureIoutDC(iouts, conditions->Channel(), useLoadMeter);

	// Store Measurement(s)
	returnType_ = std::make_pair(name(), iouts);
}
//============
// beenDone()
//============
bool IoutDC::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Iout DC and decide if we can speed up the test
	return( 
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
		     (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=================
// IoutTripPoint()
//=================
void IoutTripPoint::operator()(ConditionsPtr conditions, const PairMType& limits) {
    // Check arguments and ensure vouts are reasonable
    ConverterOutput::Output output = conditions->Channel();
    Assert<BadArg>(output != ConverterOutput::ALL, Name());
    Assert<MeasurementError>(spts_->LoadType() == LoadTraits::ELECTRONIC, Name());
    VariablesFile* varPtr = SingletonType<VariablesFile>::Instance();
    typedef DUTExceptionTypes::VoutBadBeforeTest<IoutTripPoint>::PreVoutBad PVB;
	if ( varPtr->DoDUTSanityChecks() )
        StationNS::CheckVouts<PVB>(PercentType(20));
    
    // Local variables to be used in loop below
    MTypeContainer vouts;
    MeasureVoutDC(vouts, output);
    MType vInitial = absolute(vouts.at(0));
    MType vTrip = (vInitial * MType(0.01));
    if ( conditions->RefVal() != TestStepInfo::UNDEFINEDSETTYPE )
        vTrip = conditions->RefVal().Value();
    MType vTarget = vInitial - vTrip;
    MType upperLimit = (limits.second + limits.second / MType(5));
    LoadTraits::Channels loadChannel = spts_->Convert2LoadChannel(output);
    InstrumentFile* iFile = SingletonType<InstrumentFile>::Instance();
    SetType stepSize = iFile->LoadResolution(loadChannel, upperLimit.Value());
    stepSize *= 2;
    MType minAccuracy = iFile->LoadAccuracy(loadChannel, upperLimit.Value());    
    SetType fullLoad = dut_->Iout(output);
    MType tmp = fullLoad.Value();

    // Assert that the limits are within the capability of the load
    Assert<FileError>(tmp < limits.first, Name());
    Assert<MeasurementError>((limits.first - tmp) > minAccuracy, Name()); 

    // Perform a binary search for the trip point
    SetType iNoTrip = fullLoad, iTrip = upperLimit, nextLoad = fullLoad;
    Assert<BadArg>(iNoTrip < iTrip, Name());
    SetType actualTripPoint(-1), two(2);
    typedef PauseStates PS;
    typedef SingletonType<PS> File;
    SetType ioutTripPointPause = File::Instance()->GetPauseValue(PS::TRIPPOINT);
    bool tripped = false;
    long counter = -1, maxCounter = 50;
    bool done = false;
    while ( !done ) {
        spts_->SetLoad(loadChannel, nextLoad);
        vouts.erase(vouts.begin(), vouts.end());
        Assert<InfiniteLoop>(++counter < maxCounter, Name());
        Pause(ioutTripPointPause);
        MeasureVoutDC(vouts, output);
        MType vout = absolute(vouts.at(0));
        
        if ( absolute(iTrip - iNoTrip) < stepSize ) { // found trip point
            if ( vout < vTarget )
                iTrip = nextLoad;
            else
                iNoTrip = nextLoad;
            actualTripPoint = (iNoTrip + iTrip) / two;
            spts_->SetLoad(LoadTraits::ALL, OFF);
            spts_->SetLoad(loadChannel, fullLoad);
            spts_->SetLoad(LoadTraits::ALL, ON);
            break;            
        }

        if ( vout < vTarget ) { // tripped
            tripped = true;
            iTrip = nextLoad;
            spts_->SetLoad(LoadTraits::ALL, OFF);
            spts_->SetLoad(loadChannel, fullLoad);
            spts_->SetLoad(LoadTraits::ALL, ON);
        }
        else // didn't trip
            iNoTrip = nextLoad;
        nextLoad = (iNoTrip + iTrip) / two;
    } // while
    Assert<NoTripPoint>(tripped, Name()); // ensure we tripped at least once

    // Account for load accuracy problems and store trip point
    MTypeContainer iouts;
    MeasureIoutDC(iouts, output);
    SetType lastMeasured = iouts.at(0);
    SetType difference = lastMeasured - fullLoad;
    Assert<MeasurementError>(difference.Value() <= 2 * minAccuracy.Value(), Name());
    actualTripPoint += difference;

    returnType_ = makeRtnType(Name(), actualTripPoint.Value());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===================
// IoutTripPretrim()
//===================
void IoutTripPretrim::operator()(ConditionsPtr conditions, const PairMType& limits) {
    // Locals
    DialogBox& dbox = (*SingletonType<DialogBox>::Instance());
    
    // Measure Iout Trip Point
    IoutTripPoint::operator()(conditions, limits);
    Assert<UnexpectedState>(std::size_t(1) == returnType_.second.size(), Name());
    MType measure = returnType_.second[0];

    // Get jumper pull info and display if any wirebond(s) should be pulled
    bool found = false;
    ConverterOutput::Output output = conditions->Channel();
    VariablesFile* varPtr = SingletonType<VariablesFile>::Instance();
    VariablesFile::JumperPullTable toPull = varPtr->GetJumperPullIout(output);
    Assert<FileError>(toPull.begin() != toPull.end(), Name());
    VariablesFile::JumperPullTable::iterator i = toPull.begin();
    while ( i != toPull.end() ) { // another jumper pull range to check
        std::vector<std::string> pullRow = SplitString(i->second, ',');
        Assert<FileError>(3 == pullRow.size(), Name());
        Assert<FileError>(IsFloating(pullRow[0]), Name());
        Assert<FileError>(IsFloating(pullRow[1]), Name());

        // Set precision of measured value to match data pulled in from 
        //   pull table --> ensure we can find a match if measured value 
        //   is equal to a file boundary value.
        std::vector<std::string> split1 = SplitString(pullRow[0], '.');
        std::vector<std::string> split2 = SplitString(pullRow[1], '.');
        long prec1 = 0, prec2 = 0;
        if ( 2 == split1.size() )
            prec1 = static_cast<long>(split1[1].size());
        else
            Assert<FileError>(1 == split1.size(), Name());

        if ( 2 == split2.size() )
            prec2 = static_cast<long>(split2[1].size());
        else
            Assert<FileError>(1 == split2.size(), Name()); 

        bool equal = false;
        MType mTemp = measure;
        mTemp.SetPrecision(prec1);
        if ( mTemp.ValueStr() == pullRow[0] )
            equal = true;
        mTemp = measure;
        mTemp.SetPrecision(prec2);
        if ( mTemp.ValueStr() == pullRow[1] )
            equal = true;

        // See if (measure) falls within current jumper pull boundaries
        MType low = convert<MType>(pullRow[0]), high = convert<MType>(pullRow[1]);
        if ( ((measure >= low) && (measure <= high)) || equal ) { // found correct range
            if ( Uppercase(pullRow[2]) != varPtr->NoWirebondPull() ) {
                spts_->SafeInhibit(ON);
                spts_->PowerDown();
                dbox << "Pull Wirebond: " << pullRow[2] << DialogBox::endl;
                dbox.DisplayInteractive();
                spts_->PowerUp();
                spts_->SafeInhibit(OFF);
            }
            returnType_ = makeRtnType(name(), measure);
            found = true;
            break;
        } // if
        ++i;
    } // while
    Assert<JumperPull>(i != toPull.end(), Name());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==================
// LineRegulation()
//==================
void LineRegulation::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
	MTypeContainer voutsInit, voutsFinal;

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsInit, ConverterOutput::ALL) :
		MeasureVoutDC(voutsInit, conditions->Channel());

	// Set to final line value
	spts_->SetVin(conditions->VinNext());

	// Measure Vout(s)
	conditions->Speedup() ?
		MeasureVoutDC(voutsFinal, ConverterOutput::ALL) :
		MeasureVoutDC(voutsFinal, conditions->Channel());
	Assert<ContainerState>(voutsInit.size() == voutsFinal.size(), name());

	// Calculate line regulation(s) - absolute values
	RTypeSecond holdDifference;
	std::transform(voutsInit.begin(), voutsInit.end(), voutsFinal.begin(), 
		           std::back_inserter(holdDifference), AbsDiff<MType>());
	returnType_ = std::make_pair(name(), holdDifference);
}

//============
// beenDone()
//============
bool LineRegulation::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Line Reg and decide if we can speed up the test
	return( 
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
		     (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==================
// LoadRegulation()
//==================
void LoadRegulation::operator()(ConditionsPtr conditions, const PairMType&) { 
	// Local Variables
	MTypeContainer voutsInit, voutsFinal;

	// Measure Vout(s)
    conditions->Speedup() ?
        MeasureVoutDC(voutsInit, ConverterOutput::ALL) :
        MeasureVoutDC(voutsInit, conditions->Channel());

	// Set to final load value(s)
	spts_->SetLoad(conditions->IoutsNext());

    // Measure Vout(s)
    conditions->Speedup() ?
        MeasureVoutDC(voutsFinal, ConverterOutput::ALL) :
        MeasureVoutDC(voutsFinal, conditions->Channel());
	Assert<ContainerState>(voutsInit.size() == voutsFinal.size(), name());

	// Calculate load regulation(s) - absolute values 
	RTypeSecond holdDifference;
	std::transform(voutsInit.begin(), voutsInit.end(), voutsFinal.begin(), 
		           std::back_inserter(holdDifference), AbsDiff<MType>());
	returnType_ = std::make_pair(name(), holdDifference);
}

//============
// beenDone()
//============
bool LoadRegulation::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Load Reg and decide if we can speed up the test
	return( 
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=========================
// LoadTransientRecovery()
//=========================
void LoadTransientRecovery::operator()(ConditionsPtr, const PairMType&) {
    // Actual measure done during LoadTransientResponse, not here
    throw(NoMeasurementType(Name())); 
}

//============
// beenDone()
//============
bool LoadTransientRecovery::beenDone(ConditionsPtr current, 
                                     ConditionsPtr previous) const {
	// Compare what makes sense for LoadTransRec and decide if we can speed up test
	return(
             (current->APSPrimary()  == previous->APSPrimary())  &&
             (current->APSSecondary()== previous->APSSecondary())&&
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited()) &&
             (current->RefVal()      == previous->RefVal())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=========================
// LoadTransientResponse()
//=========================
void LoadTransientResponse::operator()(ConditionsPtr conditions,
                                       const PairMType& limits) {
    // Idea is to give this measurement MAXTRIES fair shots.
    // This gives a detected problem time to correct.  If still uncorrected after
    //  MAXTRIES, then a station problem is assumed.  An example can be an oscilloscope
    //  setup time when it is re-calibrating itself after changing scales.
    static const int MAXTRIES = 2;
    bool first = true;    
    for ( int i = 1; i <= MAXTRIES; ++i ) {
        if ( !first ) { // must ensure initial conditions are re-established
            Measurement::postMeasurement(conditions); // ensure station is safe
            Measurement::preMeasurement(conditions);  // re-establish conditions
            typedef SingletonType<PauseStates> PS;    // wait a bit before retry
            Pause(PS::Instance()->GetPauseValue(PauseStates::TRANSIENTTRIGGER));
        }
        first = false;

        try {
            // Must clear return values for each iteration of loop
            returnType_.second.clear();
            extraMeasures_->clear();
            performTest(conditions, limits);
            return;
        } catch(Overshoot& o) {            
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(),
                                                      BadMeasurement));
                throw(o);
            }
        } catch(RescaleError& r) {
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(),
                                                      BadMeasurement));
                throw(r);
            }
        } catch(StationExceptionTypes::ScopeMeasure& s) {
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(),
                                                      BadMeasurement));
                throw(s);
            }
        } catch(...) { // other exceptions --> no next chance
            returnType_ = makeRtnType(Name(), BadMeasurement);
            extraMeasures_->clear();
            extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(),
                                                  BadMeasurement));
            throw;
        }  
    } // for
}

//===============
// performTest()
//===============
void LoadTransientResponse::performTest(ConditionsPtr conditions, 
                                        const PairMType& limits) {

    Assert<BadCommand>(conditions->Channel() != ConverterOutput::ALL, Name());

    // Grab transition time information
    VariablesFile* vf = SingletonType<VariablesFile>::Instance();
    std::pair<bool, SetType> p = vf->LoadTransientTransitionTime();
    SetType undefinedValue = TestStepInfo::UNDEFINEDSETTYPE;
    SetType transitionTime = (p.first ? p.second : undefinedValue);
    
    typedef std::vector<ConverterOutput::Output> Outputs;
    typedef SetTypeContainer Currents;    

    // Define locals
    Outputs outs = dut_->Outputs();
    Currents iouts = conditions->Iouts();
    Currents nextIouts = conditions->IoutsNext();
    Assert<UnexpectedState>(outs.size() == iouts.size(), Name());
    Assert<UnexpectedState>(iouts.size() == nextIouts.size(), Name());   
    OScopeParameters::SlopeType slope1, slope2; // for recovery time measure

    // More locals
    bool selected = false, low2High = false;
    ConverterOutput::Output thisChannel = conditions->Channel();
    Assert<BadArg>(thisChannel != ConverterOutput::ALL, Name());
    OScopeChannels::Channel scopeChan = OScopeChannels::ALL; 
    Outputs::iterator i = outs.begin();
    Currents::iterator c = iouts.begin(), d = nextIouts.begin();
    SetType vScale = -1;

    try {
        while ( i != outs.end() ) { // Set up another load channel
            LoadTraits::Channels loadChannel = spts_->Convert2LoadChannel(*i);        

            if ( thisChannel == *i ) {
                selected = true;
                StationNS::ACPathTypes::ImplicitPaths implicit = 
                                                  StationNS::ACPathTypes::LOADTRANSIENT;
                scopeChan = spts_->GetScopeChannel(implicit, loadChannel);
                if ( *c > *d )
                    low2High = false;
                else
                    low2High = true;
            } // if

            // Set load to higher value to set up appropriate range for test
            if ( *c > *d )
                spts_->SetLoad(loadChannel, *c);
            else
                spts_->SetLoad(loadChannel, *d);

            if ( transitionTime == undefinedValue ) // no user override
                transitionTime = 10e-6; // 10us by default
            SetType currentDelta = absolute(*c - *d);
            SetType slewRate = absolute(currentDelta / transitionTime);
            slewRate.SetPrecision(10); // arbitrary, but sufficient                           
            spts_->SetLoadTransient(loadChannel, *c, *d, slewRate);
         
            ++i;
            ++c;
            ++d;
        } // while
        Assert<UnexpectedState>(selected, Name());
        
        // Set path to oscilloscope
        spts_->SetPath(StationNS::ACPathTypes::LOADTRANSIENT, thisChannel);                                                          
        spts_->SetPath(StationNS::ACPathTypes::LOADTRIGGER);
        
        // File scope settings for this test
        std::set<OScopeSetupFile::Parameters> params;
        Assert<UnexpectedState>(scopeChan != OScopeChannels::ALL, Name());
        params = spts_->SetScope(append2Name(name(), thisChannel), scopeChan);

        // Software defined scope settings for this test
        long numberExpParms = 1;
        typedef OScopeSetupFile OSSF;

        SetType scale = (2 * limits.second.Value());
        scale /= (spts_->NumberScopeVertDvns() / 2);
        spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, scale);

        // Ensure file and software scope settings cover everything
        Assert<FileError>(numberExpParms == static_cast<long>(params.size()), Name());
        Assert<FileError>(params.find(OSSF::VERTSCALE) != params.end(), Name());

        // Define locals based on polarity of output volts/direction of transition
        OScopeMeasurements::MeasurementType toMeasure;
        long numberTrigs = 0;
        bool negativeSlope = true; // refers to waveform as it goes back to 0
        if ( low2High ) {
            if ( dut_->Vout(thisChannel) < 0 ) { // negative vout
                slope2 = OScopeParameters::NEGATIVE;
                toMeasure = OScopeMeasurements::MAXIMUMVALUE;
                numberTrigs = 1;
                negativeSlope = true;
            }
            else { // positive vout
                slope2 = OScopeParameters::POSITIVE;
                toMeasure = OScopeMeasurements::MINIMUMVALUE;
                numberTrigs = 1;
                negativeSlope = false;
            }
        }
        else { // high to low
            if ( dut_->Vout(thisChannel) < 0 ) { // negative vout
                slope2 = OScopeParameters::POSITIVE;
                toMeasure = OScopeMeasurements::MINIMUMVALUE;
                numberTrigs = 1;
                negativeSlope = false;
            }
            else { // positive vout
                slope2 = OScopeParameters::NEGATIVE;
                toMeasure = OScopeMeasurements::MAXIMUMVALUE;
                numberTrigs = 1;
                negativeSlope = true;
            }
        }               

        // Get Pause time
        typedef SingletonType<PauseStates> PS;
        SetType transientPause = 
                  PS::Instance()->GetPauseValue(PauseStates::TRANSIENTTRIGGER);

        // Measure Vout Initial
        MTypeContainer loadReg;
        bool useDMM = false;
        MeasureVoutDC(loadReg, conditions->Channel(), useDMM);
        Assert<UnexpectedState>(loadReg.size() > std::size_t(0), Name());
        MType voutLoadReg = loadReg[0];
        loadReg.erase(loadReg.begin(), loadReg.end());
        
        // Trigger Load
        spts_->WaitOnScope();
        Pause(transientPause);
        spts_->LoadTransientTrigger();
        spts_->StopScope(); // hold waveform for measure; includes spts_->WaitOnScope()

        // Calculate load regulation effects --> used for response time
        MeasureVoutDC(loadReg, conditions->Channel(), useDMM);
        Assert<UnexpectedState>(loadReg.size() > std::size_t(0), Name());
        voutLoadReg -= loadReg[0];
        
        // Make initial measurement and rescale for better resolution
        MType measured = 0;
        bool toPause = true, toRescale = false;
        measured = spts_->MeasureScope(toMeasure, scopeChan, toPause, toRescale);
        spts_->StartScope(); // restart scope

		/* Added this sequence that should allow the station to try 4 times to get the signal*/
		int Tries = 0;
        SetType VPD = scale;
		while(Tries<4){
			Tries++;
			VPD/=5;
			if(absolute(measured).Value()< VPD){
				VPD *= 2.5;
				spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, VPD);
				// Trigger Load
				spts_->WaitOnScope();
				Pause(transientPause);
				spts_->LoadTransientTrigger();
				spts_->StopScope(); // hold waveform for measure; includes spts_->WaitOnScope()

				// Get a closer look at the signal.
				measured = spts_->MeasureScope(toMeasure, scopeChan, toPause, toRescale);
				spts_->StartScope(); // restart scope
			}else{
				Tries = 4;
			}
		}
		vScale = (absolute(measured).Value() / (spts_->NumberScopeVertDvns() / 2 - 1));

		spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, vScale);

        MType Avgmeasure = 0;
		MType maxcycles = 8;
		for(int loopcount; loopcount<maxcycles; loopcount++){
	        // Re-trigger load
			spts_->LoadTransientTrigger();

			// Re-trigger and measure --> actual measurement for this test
			Pause(transientPause); // for converter recovery
			spts_->WaitOnScope();
			spts_->LoadTransientTrigger();
			spts_->StopScope(); // hold waveform for measure; includes spts_->WaitOnScope()

			try{// Added this measurement to correct for rescale failures. 09/05/07
				measured = absolute(spts_->MeasureScope(toMeasure, scopeChan, 
													toPause, toRescale));
			} catch(RescaleError&) { // waveform clipping
				spts_->StartScope(); // restart scope
				vScale = vScale + vScale;
				spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, vScale);

				// Re-trigger load
				spts_->LoadTransientTrigger();

				// Re-trigger and measure --> Final measurement for this test
				Pause(transientPause); // for converter recovery
				spts_->WaitOnScope();
				spts_->LoadTransientTrigger();
				spts_->StopScope(); // hold waveform for measure; includes spts_->WaitOnScope()
				measured = absolute(spts_->MeasureScope(toMeasure, scopeChan, 
														toPause, toRescale));
			}
			Avgmeasure += measured;
		}
		measured = Avgmeasure/maxcycles;

        Assert<MeasurementError>(measured > MType(0), Name());
        returnType_ = makeRtnType(Name(), measured);
        spts_->LoadTransientOff();


        //==========================================
        // Load Transient Recovery Time measurement
        //==========================================

        // Measure recovery time and store in extraMeasures_        
        SetType levelFrom = 2.5, levelTo = conditions->RefVal();
        if ( levelTo == undefinedValue ) // use default value
            levelTo = dut_->Vout(thisChannel) * SetType(0.01);
        levelTo = absolute(levelTo);
        levelTo += (absolute(voutLoadReg)).Value(); // cancel out load reg effects
        MType peakMeasure = measured;

        if ( levelTo >= peakMeasure ) { // no recovery time to measure
            extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(), 0));
            spts_->StartScope(); // restart oscilloscope
            return;
        }

        // Assign (slope1) depending on what the load type is
        OScopeChannels::Channel fromChan;
        if ( spts_->LoadType() == LoadTraits::ELECTRONIC ) {
            slope1 = LoadTraits::ModelType::LoadTriggerSlope;
            fromChan = OScopeChannels::TRIGGER;
        }
        else { // LoadTraits::PASSIVE
            slope1 = (slope2 == OScopeParameters::POSITIVE ?
                                OScopeParameters::NEGATIVE :
                                OScopeParameters::POSITIVE);                
            fromChan = scopeChan;
        }

        // If you do not have a negative-going slope, then waveform is below reference
        if ( ! negativeSlope )
            levelTo *= -1;

        toPause = false;
        try {
            measured = spts_->MeasureScope(OScopeMeasurements::DELAY,
                                           fromChan, scopeChan, levelFrom, levelTo, 
                                           slope1, slope2, toPause);
            Assert<MeasurementError>(measured > MType(0), Name());
        } catch(ScopeMeasureError&) { // no recovery actually occured within time?
            SetType refValue = absolute(levelTo), pMeas = absolute(peakMeasure).Value();
            SetType margin = 0.5; // allow up to half of a vertical division
            if ( absolute(refValue - pMeas) <= vScale * margin ) 
                measured = 0; // ref value too close to peak value                
            else // time too long
                measured = Measurement::BadMeasurement;
        } catch(RescaleError& re) { // bad error; would have been caught at pk measure
            SetType refValue = absolute(levelTo), pMeas = absolute(peakMeasure).Value();
            SetType margin = 0.5; // allow up to half a vertical division
            if ( absolute(refValue - pMeas) <= vScale * margin ) 
                measured = 0; // ref value too close to peak value                
            else // actual bad measurement
                throw(re);
        }
        extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(), measured));
        spts_->StartScope(); // restart oscilloscope
    } catch(RescaleError&) { // waveform clipping
        spts_->LoadTransientOff();
        spts_->StartScope();
        MType bad = Measurement::BadMeasurement;
        if ( extraMeasures_->empty() ) // recovery time is dependent upon this call
            extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(), bad));
        throw(Overshoot(Name()));
    } catch(...) {
        spts_->LoadTransientOff();
        spts_->StartScope();
        MType bad = Measurement::BadMeasurement;
        if ( extraMeasures_->empty() ) // recovery time is dependent upon this call
            extraMeasures_->push_back(makeRtnType(LoadTransientRecovery::Name(), bad));
        throw;
    }
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==================
// LowLineDropout()
//==================
void LowLineDropout::operator()(ConditionsPtr conditions, const PairMType& limits) {
    ConverterOutput::Output output = conditions->Channel();
    Assert<BadArg>(output != ConverterOutput::ALL, Name());

    // ensure starting output voltages are reasonable
    typedef DUTExceptionTypes::VoutBadBeforeTest<LowLineDropout>::PreVoutBad PreVout;
    VariablesFile* varPtr = SingletonType<VariablesFile>::Instance();
	if ( varPtr->DoDUTSanityChecks() )
        StationNS::CheckVouts<PreVout>(PercentType(20));

    // Locals
    MTypeContainer vouts; 
    MeasureVoutDC(vouts, output);
    Assert<UnexpectedState>(std::size_t(1) == vouts.size(), Name());
    MType vout = absolute(vouts[0]);
    MType vTripVal = (MType(0.01) * vout); // default
    if ( conditions->RefVal() != TestStepInfo::UNDEFINEDSETTYPE ) 
        vTripVal = conditions->RefVal().Value(); // use non-default trip value
    MType vTarget = vout - vTripVal;

    // Get pause value
    typedef SingletonType<PauseStates> PS;
    SetType lldoPause = PS::Instance()->GetPauseValue(PauseStates::LLDO);

    // Loop variables
    long counter = -1, maxCounter = 50;
    bool done = false;

    // More variables
    SetType vStep  = 1, margin = 3; // , denominator = 25, vNoTrip;
    SetType vReset = dut_->LowLine() + margin;
    MType lowerLimit = (limits.first - limits.first / MType(5));
    SetType vTrip = lowerLimit, vNoTrip = vReset;
    Assert<BadArg>(vTrip < vNoTrip, Name());
    SetType nextVoltage = dut_->LowLine(), two(2);
    typedef SingletonType<InstrumentFile> IF;
    SetType vResolution = IF::Instance()->VoltageResolution(spts_->WhichSupply());
    SetType stepSize = two * vResolution;
    SetType actualTripPoint;

    // Main loop
    bool tripped = false, canUseDMM = true, noDMM = false, useDMM = canUseDMM;
    while ( ! done ) {
        // Set next vin value
        spts_->SetVin(nextVoltage, useDMM);
        useDMM = noDMM;
        vouts.erase(vouts.begin(), vouts.end());
        Assert<InfiniteLoop>(++counter < maxCounter, Name());

        // Pause and measure vout
        Pause(lldoPause);
        MeasureVoutDC(vouts, output);
        MType vout = absolute(vouts.at(0));
        
        if ( absolute(vNoTrip - vTrip) < stepSize ) { // found trip point
            if ( vout < vTarget )
                vTrip = nextVoltage;
            else
                vNoTrip = nextVoltage;
            actualTripPoint = (vNoTrip + vTrip) / two;
            spts_->SetLoad(LoadTraits::ALL, OFF);
            spts_->SetVin(vReset, canUseDMM);
            spts_->SetLoad(LoadTraits::ALL, ON);
            break; // done
        }

        if ( vout < vTarget ) { // tripped
            tripped = true;
            vTrip = nextVoltage;
            spts_->SetLoad(LoadTraits::ALL, OFF);
            spts_->SetVin(vReset, noDMM);
            spts_->SetVin(dut_->LowLine(), canUseDMM);
            spts_->SetLoad(LoadTraits::ALL, ON);
            useDMM = canUseDMM;
        }
        else // didn't trip
            vNoTrip = nextVoltage;
        nextVoltage = (vNoTrip + vTrip) / two;
    } // while
    Assert<NoTripPoint>(tripped, Name()); // ensure we tripped at least once
 
    // Account for accuracy of power supply
    SetType minAccuracy = IF::Instance()->VoltageAccuracy(spts_->WhichSupply());
    MType vDiff = MeasureVinDC() - vReset;
    Assert<VinTolerance>(absolute(vDiff) <= minAccuracy, Name());
    actualTripPoint += vDiff.Value();

    returnType_ = makeRtnType(Name(), actualTripPoint.Value());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//====================
// PowerDissipation()
//====================
void PowerDissipation::operator()(ConditionsPtr conditions, const PairMType&) { 
	// Measure inputs - calculate input power
    MType pin, iin;
    iin = MeasureIinDC(StationNS::NOINHIBIT);
    pin = (MeasureVinDC() * iin);

	// Measure outputs
	MTypeContainer vouts, iouts; 
	MeasureVoutDC(vouts, ConverterOutput::ALL);
    bool useLoadMeter = conditions->Shorted().empty();
    MeasureIoutDC(iouts, ConverterOutput::ALL, useLoadMeter);
	
	// Take absolute values of all output measurements
    Assert<ContainerState>(iouts.size() == vouts.size(), name());
    for ( std::size_t i = 0; i < iouts.size(); ++i ) {
        iouts[i] = absolute(iouts[i]); 
        vouts[i] = absolute(vouts[i]);
    }

	// Calculate output power
	MType pout = std::inner_product(iouts.begin(), iouts.end(), 
	                                vouts.begin(), static_cast<MType>(0));  
	
	// Calculate Power Dissipation 
    Assert<MeasurementError>(pin > pout, Name());
	returnType_ = makeRtnType(name(), (pin - pout));
}
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==================
// SCReleaseDelay()
//==================
void SCReleaseDelay::operator()(ConditionsPtr conditions, const PairMType& limits)
{
    Assert<BadArg>(!conditions->Shorted().empty(), Name());
    TurnOnDelay::operator()(conditions, limits); 
}

//============
// beenDone()
//============
bool SCReleaseDelay::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
    Assert<BadArg>(!current->Shorted().empty(), Name());
	// Compare what makes sense for SCReleaseDelay and decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//======================
// SCReleaseOvershoot()
//======================
void SCReleaseOvershoot::operator()(ConditionsPtr conditions, const PairMType& limits) {
    Assert<BadArg>(!conditions->Shorted().empty(), Name());
    std::auto_ptr<Measurement> mptr(MFactory::Instance()->CreateObject(
                                                    Uppercase(SCReleaseDelay::Name()))
                                                                      );
    ReturnType rt = mptr->MeasureWithoutPrePostConditions(conditions, limits);
    Measurement::errorCode_ = mptr->WhatDUTError();
    ReturnTypeContainer extras = mptr->ExtraMeasurements();
    Assert<UnexpectedState>(extras.size() == std::size_t(1), Name());
    returnType_ = extras.at(0);
    extraMeasures_->push_back(makeRtnType(TurnOnDelay::Name(), rt.second[0]));
}

//============
// beenDone()
//============
bool SCReleaseOvershoot::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
    Assert<BadArg>(!current->Shorted().empty(), Name());
	// Compare what makes sense for SCReleaseOvershoot; decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//===============
// TurnOnDelay()
//===============
void TurnOnDelay::operator()(ConditionsPtr conditions,const PairMType& limits) 
{
    // Idea is to give this measurement MAXTRIES fair shots.
    // This gives a detected problem time to correct.  If still uncorrected after
    //  MAXTRIES, then a station problem is assumed.  An example can be an oscilloscope
    //  setup time when it is re-calibrating itself after changing scales.
    static const int MAXTRIES = 2;
    bool first = true;
    std::string overshootName;
    if ( type_ == PRIMARYINHIBIT )
        overshootName = TurnOnOvershoot::Name();
    else if ( type_ == VINRAMP )
        overshootName = VinRampOvershoot::Name();
    else if ( type_ == SHORT )
        overshootName = SCReleaseOvershoot::Name();

    for ( int i = 1; i <= MAXTRIES; ++i ) {
        if ( !first ) { // must ensure initial conditions are re-established
            Measurement::postMeasurement(conditions); // ensure station is safe
            Measurement::preMeasurement(conditions);  // re-establish conditions
            typedef SingletonType<PauseStates> PS;    // wait a bit before retry
            Pause(PS::Instance()->GetPauseValue(PauseStates::TOD));
        }
        first = false;

        try {
            // Must clear return values for each iteration of loop
            returnType_.second.clear();
            extraMeasures_->clear();
            performTest(conditions, limits);
            return;
        } catch(Overshoot& o) {            
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(overshootName,
                                                      BadMeasurement));
                throw(o);
            }
        } catch(Undershoot& u) {            
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(overshootName,
                                                      BadMeasurement));
                throw(u);
            }
        } catch(RescaleError& r) {
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(overshootName,
                                                      BadMeasurement));
                throw(r);
            }
        } catch(StationExceptionTypes::ScopeMeasure& s) {
            if ( MAXTRIES == i ) { // lost last chance
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->clear();
                extraMeasures_->push_back(makeRtnType(overshootName,
                                                      BadMeasurement));
                throw(s);
            }
        } catch(...) { // other exceptions --> no next chance
            returnType_ = makeRtnType(Name(), BadMeasurement);
            extraMeasures_->clear();
            extraMeasures_->push_back(makeRtnType(overshootName,
                                                  BadMeasurement));
            throw;
        }        
    } // for
}

//===============
// performTest()
//===============
void TurnOnDelay::performTest(ConditionsPtr conditions, const PairMType&) {

    // Define locals
    ConverterOutput::Output output = conditions->Channel();
    Assert<BadRtnValue>(output != ConverterOutput::ALL, name());
    LoadTraits::Channels channel = spts_->Convert2LoadChannel(output);
    StationNS::ACPathTypes::ImplicitPaths impl = StationNS::ACPathTypes::STARTUPDELAY;
    OScopeChannels::Channel scopeChan = spts_->GetScopeChannel(impl, channel);
	OScopeChannels::Channel trigChan = OScopeChannels::TRIGGER;


	if( type_ == SHORT )       //This is a work-around for our current
	{//Main if
						       //OScope trigger issue with the 11671-XXXX product.
        trigChan = scopeChan;  //Forcing the scopeChan to be
							   //the trigger source (measurement channel = Trigger channel)
	    PauseStates *ps = SingletonType<PauseStates>::Instance();
        // Software defined scope settings for this test
		long numberExpParms = 2;
		typedef OScopeSetupFile OSSF;
		SetType scale = absolute(2 * dut_->Vout(output)), offset;
		scale /= spts_->NumberScopeVertDvns();
		double scopeScreenMargin = 0.5;
		offset = (scale.Value() * (spts_->NumberScopeVertDvns() / 2 - scopeScreenMargin));
		if ( dut_->Vout(output) > 0 ) // positive output
        offset *= -1;
		spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, scale);
		spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::OFFSET, offset);

		if( type_ == PRIMARYINHIBIT )
		{							// set trigger channel offset
			Assert<BadCommand>(scopeChan != trigChan, name());
			SetType trigOff = spts_->GetScopeVertScale(trigChan).Value();
			trigOff *= -(spts_->NumberScopeVertDvns() / 2 - scopeScreenMargin);
			spts_->SetScopeExplicit(trigChan, StationNS::ExplicitScope::OFFSET, trigOff);
		}

		// File scope settings for this test
		std::set<OScopeSetupFile::Parameters> params;
		params = spts_->SetScope(append2Name(name(), output), scopeChan);	//03/24/2006
																		//The vertical scale must
																		// be set first.
    
		// Ensure file and Software scope settings cover everything
		Assert<FileError>(numberExpParms == static_cast<long>(params.size()), Name());
		Assert<FileError>(params.find(OSSF::VERTSCALE) != params.end(), Name());
		Assert<FileError>(params.find(OSSF::OFFSET) != params.end(), Name());
    
		try
		{//Open try
			if ( type_ == SHORT )
			{ // temporarily unshort
				spts_->SafeInhibit(ON);
				SpacePowerTestStation::Short(conditions->Shorted(), OFF);
				spts_->SafeInhibit(OFF);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
			}

			// Set load to CR mode for this test
			spts_->SetLoadModes(LoadTraits::CR);

			// Set path to oscilloscope
			spts_->StrongInhibit(ON);
			spts_->SetPath(StationNS::ACPathTypes::STARTUPDELAY, output);
			if ( type_ == PRIMARYINHIBIT )
			{
				spts_->SetPath(StationNS::ACPathTypes::PRIMARYINHIBIT);
			}
			else
			if( type_ == VINRAMP )
			{
				spts_->SetVin(0);
				spts_->SetPath(StationNS::ACPathTypes::VINRISE);
				spts_->SafeInhibit(OFF); // inhibit not used
			}
			else
			if( type_ == SHORT )
			{
				spts_->SafeInhibit(OFF); // inhibit not used
				StationNS::Short(conditions->Shorted(), ON);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));            
			}
			else
            throw(UnexpectedState(Name()));

			// Pause        
			SetType timeToPause = ps->GetPauseValue(PauseStates::TOD);
			Pause(timeToPause);
			spts_->WaitOnScope();

			// Reset condition, dependent upon (type_), and capture waveform
			if( type_ == PRIMARYINHIBIT )
			{
				spts_->StrongInhibit(OFF);
			}
			else
			if( type_ == VINRAMP )
			{
				spts_->SetVin(conditions->Vin());
			}
			else
			if( type_ == SHORT )
			{
				StationNS::Short(conditions->Shorted(), OFF);
			}
			// Variable declarations
			OScopeMeasurements::MeasurementType overshoot = OScopeMeasurements::MAXIMUMVALUE;
			OScopeParameters::SlopeType slope1 = OScopeParameters::POSITIVE, slope2 = slope1;

			// Make measurement
			bool toPause = true;
			VariablesFile* vf = SingletonType<VariablesFile>::Instance();
			std::pair<bool, PercentType> percent = vf->TODPercentage();
			PlusMinusPercentType perc1 = 10, perc2 = 95; // defaults
			if( percent.first )		// override perc2
			{
				perc2 = percent.second.Value();
			}

			if( dut_->Vout(output) < 0 )
			{ // Negative Output
				slope2 = OScopeParameters::NEGATIVE;
				overshoot = OScopeMeasurements::MINIMUMVALUE;
			}
		
			if( type_ == SHORT ) // Both measurement slopes must be the same
			{
				slope1 = slope2;	 //3/24/2006
			}

			std::string overshootName;
			if( type_ == PRIMARYINHIBIT )
			{
				overshootName = TurnOnOvershoot::Name();
			}
			else
			if( type_ == VINRAMP )
			{
				overshootName = VinRampOvershoot::Name();
			}
			else
			if( type_ == SHORT )
			{
				overshootName = SCReleaseOvershoot::Name();
			}
			Pause(timeToPause);
			spts_->StopScope(); // hold waveform for measurements
			MType measured;
			try
			{
				measured = spts_->MeasureScope(OScopeMeasurements::DELAY, trigChan, scopeChan, perc1, perc2, slope1, slope2, toPause);
			}//close try
			catch(RescaleError&)
			{
				// This error is thrown when a major overshoot is detected
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name()));
			}
			catch(StationExceptionTypes::ScopeMeasure&)
			{
				// This error is thrown when a negative time is measured
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name())); // type that gives another measurement chance
			}
			Assert<MeasurementError>(measured >= MType(0), Name());
			returnType_ = makeRtnType(Name(), measured);

			//===============================
			// Turn On Overshoot Measurement
			//===============================

        // Measure overshoot and store in extraMeasures_
        try
		{
            MType measured2;
            typedef OScopeMeasurements::MeasurementType MT;
            MT overshootType = OScopeMeasurements::MAXIMUMVALUE;
            if ( dut_->Vout(output) < 0 ) // Negative Output
                overshootType = OScopeMeasurements::MINIMUMVALUE;
            toPause = false;
            bool rescale = false;

            // Measure overshoot and ensure DUT output makes it back to near nominal
            measured = spts_->MeasureScope(overshootType, scopeChan, toPause, rescale);
            MType nominal = absolute(dut_->Vout(output));
            measured = absolute(measured);

            if ( nominal - measured > MType(1) )
			{ // DUT didn't recover
                spts_->StartScope();
                returnType_ = makeRtnType(Name(), BadMeasurement);
                extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
                throw(Undershoot(Name()));
            }

            // Measure VoutDC using oscilloscope to calculate overshoot
            spts_->SetScopeExplicit(OScopeParameters::AUTO);
            spts_->StartScope();
            Pause(timeToPause);
            measured2 = spts_->MeasureScope(overshootType, scopeChan, toPause, rescale);
            measured2 = absolute(measured2);
            measured = (measured2 < measured ? absolute(measured-measured2) : 0);
        }//Close try
		catch(RescaleError&)
		{
            spts_->StartScope();
            returnType_ = makeRtnType(Name(), BadMeasurement);
            extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
            throw(Overshoot(Name())); // overshoot detected
        }//Close catch
		catch(StationExceptionTypes::ScopeMeasure&)
		{
            spts_->StartScope();
            returnType_ = makeRtnType(Name(), BadMeasurement);
            extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
            throw(Overshoot(Name())); // type that gives another measurement chance
        }//close catch
        extraMeasures_->push_back(makeRtnType(overshootName, measured));
        if( type_ == SHORT )
		{ // unshort to go to CC mode again
            spts_->SafeInhibit(ON);
            StationNS::Short(conditions->Shorted(), OFF);
            Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
            spts_->SafeInhibit(OFF);
        }
        // Set loads back to CC mode and re-start oscilloscope
        spts_->SetLoadModes(LoadTraits::CC);
        spts_->StartScope();
       }//Close try
	   catch(...)//Any other cases
	   {
		spts_->StartScope();
        if( type_ == SHORT )
		{ // unshort to go to CC mode again
            spts_->SafeInhibit(ON);
            StationNS::Short(conditions->Shorted(), OFF);
            Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
            spts_->SafeInhibit(OFF);
        }
        spts_->SetLoadModes(LoadTraits::CC);
        throw;
	   }//Close catch
    }//close Main if
	else
	{//Main else
		PauseStates* ps = SingletonType<PauseStates>::Instance();
	    // File scope settings for this test
		std::set<OScopeSetupFile::Parameters> params;
		params = spts_->SetScope(append2Name(name(), output), scopeChan);
		// Software defined scope settings for this test
		long numberExpParms = 2;
		typedef OScopeSetupFile OSSF;
		SetType scale = absolute(2 * dut_->Vout(output)), offset;
		scale /= spts_->NumberScopeVertDvns();
		double scopeScreenMargin = 0.5;
		offset = (scale.Value() * (spts_->NumberScopeVertDvns() / 2 - scopeScreenMargin));
		if ( dut_->Vout(output) > 0 ) // positive output
			offset *= -1;
		spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::VERTSCALE, scale);
		spts_->SetScopeExplicit(scopeChan, StationNS::ExplicitScope::OFFSET, offset);
		if( type_ == PRIMARYINHIBIT )
		{	// set trigger channel offset
			Assert<BadCommand>(scopeChan != trigChan, name());
			SetType trigOff = spts_->GetScopeVertScale(trigChan).Value();
			trigOff *= -(spts_->NumberScopeVertDvns() / 2 - scopeScreenMargin);
			spts_->SetScopeExplicit(trigChan, StationNS::ExplicitScope::OFFSET, trigOff);
		}
		// Ensure file and Software scope settings cover everything
		Assert<FileError>(numberExpParms == static_cast<long>(params.size()), Name());
		Assert<FileError>(params.find(OSSF::VERTSCALE) != params.end(), Name());
		Assert<FileError>(params.find(OSSF::OFFSET) != params.end(), Name());
		try
		{//Big Second try
			if( type_ == SHORT )
			{ // temporarily unshort
				spts_->SafeInhibit(ON);
				SpacePowerTestStation::Short(conditions->Shorted(), OFF);
				spts_->SafeInhibit(OFF);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
			}
			// Set load to CR mode for this test
			spts_->SetLoadModes(LoadTraits::CR);
			// Set path to oscilloscope
			spts_->StrongInhibit(ON);
			spts_->SetPath(StationNS::ACPathTypes::STARTUPDELAY, output);
			if( type_ == PRIMARYINHIBIT )
			{
				spts_->SetPath(StationNS::ACPathTypes::PRIMARYINHIBIT);
			}
			else
			if( type_ == VINRAMP )
			{
				spts_->SetVin(0);
				spts_->SetPath(StationNS::ACPathTypes::VINRISE);
				spts_->SafeInhibit(OFF); // inhibit not used
			}
			else
			if( type_ == SHORT )
			{
				spts_->SafeInhibit(OFF); // inhibit not used
				StationNS::Short(conditions->Shorted(), ON);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));            
			}
			else
			throw(UnexpectedState(Name()));
			// Pause        
			SetType timeToPause = ps->GetPauseValue(PauseStates::TOD);
			Pause(timeToPause);
			spts_->WaitOnScope();

			// Reset condition, dependent upon (type_), and capture waveform
			if( type_ == PRIMARYINHIBIT )
			{
				spts_->StrongInhibit(OFF);
			}
			else
			if( type_ == VINRAMP )
			{
				spts_->SetVin(conditions->Vin());
			}
			else
			if( type_ == SHORT )
			{
				StationNS::Short(conditions->Shorted(), OFF);
			}
			// Variable declarations
			OScopeMeasurements::MeasurementType overshoot = OScopeMeasurements::MAXIMUMVALUE;
			OScopeParameters::SlopeType slope1 = OScopeParameters::POSITIVE, slope2 = slope1;
			// Make measurement
			bool toPause = true;
			VariablesFile* vf = SingletonType<VariablesFile>::Instance();
			std::pair<bool, PercentType> percent = vf->TODPercentage();
			PlusMinusPercentType perc1 = 10, perc2 = 95; // defaults
			if( percent.first ) // override perc2
			{
				perc2 = percent.second.Value();
			}
			if( dut_->Vout(output) < 0 )
			{ // Negative Output
				slope2 = OScopeParameters::NEGATIVE;
				overshoot = OScopeMeasurements::MINIMUMVALUE;
			}
			std::string overshootName;
			if( type_ == PRIMARYINHIBIT )
			{
				overshootName = TurnOnOvershoot::Name();
			}
			else
			if( type_ == VINRAMP )
			{
				overshootName = VinRampOvershoot::Name();
			}
			else
			if( type_ == SHORT )
			{
				overshootName = SCReleaseOvershoot::Name();
			}
			Pause(timeToPause);
			spts_->StopScope(); // hold waveform for measurements
			MType measured;
			try
			{//third try
				measured = spts_->MeasureScope(OScopeMeasurements::DELAY, trigChan, scopeChan, perc1, perc2, slope1, slope2, toPause);
			}//Close third try
			catch(RescaleError&)
			{
				// This error is thrown when a major overshoot is detected
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name()));
			}//close catch
			catch(StationExceptionTypes::ScopeMeasure&)
			{
				// This error is thrown when a negative time is measured
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name())); // type that gives another measurement chance
			}//close catch
			Assert<MeasurementError>(measured >= MType(0), Name());
			returnType_ = makeRtnType(Name(), measured);

			//===============================
			// Turn On Overshoot Measurement
			//===============================
			// Measure overshoot and store in extraMeasures_
			try
			{//fourth try
				MType measured2;
				typedef OScopeMeasurements::MeasurementType MT;
				MT overshootType = OScopeMeasurements::MAXIMUMVALUE;
				if( dut_->Vout(output) < 0 ) // Negative Output
				{
					overshootType = OScopeMeasurements::MINIMUMVALUE;
				}//close if
				toPause = false;
				bool rescale = false;
				// Measure overshoot and ensure DUT output makes it back to near nominal
				measured = spts_->MeasureScope(overshootType, scopeChan, toPause, rescale);
				MType nominal = absolute(dut_->Vout(output));
				measured = absolute(measured);
				if( nominal - measured > MType(1) )
				{ // DUT didn't recover
					spts_->StartScope();
					returnType_ = makeRtnType(Name(), BadMeasurement);
					extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
					throw(Undershoot(Name()));
				}//close if
				// Measure VoutDC using oscilloscope to calculate overshoot
				spts_->SetScopeExplicit(OScopeParameters::AUTO);
				spts_->StartScope();
				Pause(timeToPause);
				measured2 = spts_->MeasureScope(overshootType, scopeChan, toPause, rescale);
				measured2 = absolute(measured2);
				measured = (measured2 < measured ? absolute(measured-measured2) : 0);
			}//close fourth try
			catch(RescaleError&)
			{
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name())); // overshoot detected
			}//close catch
			catch(StationExceptionTypes::ScopeMeasure&)
			{
				spts_->StartScope();
				returnType_ = makeRtnType(Name(), BadMeasurement);
				extraMeasures_->push_back(makeRtnType(overshootName, BadMeasurement));
				throw(Overshoot(Name())); // type that gives another measurement chance
			}//close catch
			extraMeasures_->push_back(makeRtnType(overshootName, measured));
			if( type_ == SHORT )
			{ // unshort to go to CC mode again
				spts_->SafeInhibit(ON);
				StationNS::Short(conditions->Shorted(), OFF);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
				spts_->SafeInhibit(OFF);
			}//close if
			// Set loads back to CC mode and re-start oscilloscope
			spts_->SetLoadModes(LoadTraits::CC);
			spts_->StartScope();
		}//Close Big Second try
		catch(...)//All other cases
		{
			spts_->StartScope();
			if( type_ == SHORT )
			{ // unshort to go to CC mode again
				spts_->SafeInhibit(ON);
				StationNS::Short(conditions->Shorted(), OFF);
				Pause(ps->GetPauseValue(PauseStates::OPTIONALINITIALCONDITIONS));
				spts_->SafeInhibit(OFF);
			}
			spts_->SetLoadModes(LoadTraits::CC);
			throw;
		}//Close catch all other cases
	}//close main else
}

//============
// beenDone()
//============
bool TurnOnDelay::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for TurnOnDelay and decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===================
// TurnOnOvershoot()
//===================
void TurnOnOvershoot::operator()(ConditionsPtr conditions, const PairMType& limits) {

    std::auto_ptr<Measurement> mptr( 
                    MFactory::Instance()->CreateObject(Uppercase(TurnOnDelay::Name()))
                                   );
    ReturnType rt = mptr->MeasureWithoutPrePostConditions(conditions, limits);
    Measurement::errorCode_ = mptr->WhatDUTError();
    ReturnTypeContainer extras = mptr->ExtraMeasurements();
    Assert<UnexpectedState>(std::size_t(1) == extras.size(), Name());
    returnType_ = extras[0];
    extraMeasures_->push_back(makeRtnType(TurnOnDelay::Name(), rt.second[0]));
}

//============
// beenDone()
//============
bool TurnOnOvershoot::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for TurnOnOvershoot and decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=========
// VinDC()
//=========
void VinDC::operator()(ConditionsPtr, const PairMType&) {
	returnType_ = makeRtnType(name(), MeasureVinDC());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//================
// VinRampDelay()
//================
void VinRampDelay::operator()(ConditionsPtr conditions, const PairMType& limits) {
    TurnOnDelay::operator()(conditions, limits);
}

//============
// beenDone()
//============
bool VinRampDelay::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for VinRampDelay and decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//====================
// VinRampOvershoot()
//====================
void VinRampOvershoot::operator()(ConditionsPtr conditions, const PairMType& limits) {

    std::auto_ptr<Measurement> mptr(MFactory::Instance()->CreateObject(
                                                    Uppercase(VinRampDelay::Name()))
                                                                      );
    ReturnType rt = mptr->MeasureWithoutPrePostConditions(conditions, limits); 
    Measurement::errorCode_ = mptr->WhatDUTError();
    ReturnTypeContainer extras = mptr->ExtraMeasurements();
    Assert<UnexpectedState>(extras.size() == std::size_t(1), Name());
    returnType_ = extras.at(0);
    extraMeasures_->push_back(makeRtnType(TurnOnDelay::Name(), rt.second[0]));
}

//============
// beenDone()
//============
bool VinRampOvershoot::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for VinRampOvershoot and decide if we can speed up test
	return( 
			 (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
             (current->VinNext()     == previous->VinNext())     &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->IoutsNext()   == previous->IoutsNext())   &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//==========
// VoutDC()
//==========
void VoutDC::operator()(ConditionsPtr conditions, const PairMType&) {
	// Local Variables
    Assert<BadArg>(conditions->Channel() != ConverterOutput::ALL, Name());
	RTypeSecond vouts;

    // Measure Vout(s)
    conditions->Speedup() ?
        MeasureVoutDC(vouts, ConverterOutput::ALL) :
        MeasureVoutDC(vouts, conditions->Channel());

	// Store Measurement(s)
	returnType_ = std::make_pair(name(), vouts);
}

//============
// beenDone()
//============
bool VoutDC::beenDone(ConditionsPtr current, ConditionsPtr previous) const {
	// Compare what makes sense for Vout DC and decide if we can speed up the test
	return( 
		     (current->Speedup()     && previous->Speedup())     &&
		     (current->Vin()         == previous->Vin())         &&
			 (current->Iouts()       == previous->Iouts())       &&
			 (current->Freq()        == previous->Freq())        &&
			 (current->Shorted()     == previous->Shorted())     &&
             (current->MidtestMisc() == previous->MidtestMisc()) &&
             (current->PretestMisc() == previous->PretestMisc()) &&
			 (current->IsInhibited() == previous->IsInhibited())
		  );
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=================
// VoutDCPretrim()
//=================
void VoutDCPretrim::operator ()(ConditionsPtr conditions, const PairMType&) {
    // Locals 
    DialogBox& dbox = (*SingletonType<DialogBox>::Instance());
    VariablesFile* varPtr = SingletonType<VariablesFile>::Instance();
    MTypeContainer vouts;

    // Assertions
    Assert<BadArg>(conditions->Channel() != ConverterOutput::ALL, Name());        
    Assert<BadCommand>(varPtr->IsJumperPullVout(), Name());        

    // Measure vout
    MeasureVoutDC(vouts, conditions->Channel());
    MType measure = vouts.at(0);    

    // Get jumper pull info and display if any wirebond(s) should be pulled
    ConverterOutput::Output output = conditions->Channel();
    VariablesFile::JumperPullTable toPull = varPtr->GetJumperPullVout(output);
    VariablesFile::JumperPullTable::iterator i = toPull.begin();
    while ( i != toPull.end() ) {        
        std::vector<std::string> pullRow = SplitString(i->second, ',');
        Assert<FileError>(3 == pullRow.size(), Name());
        Assert<FileError>(IsFloating(pullRow[0]), Name());
        Assert<FileError>(IsFloating(pullRow[1]), Name());

        // Set precision of measured value to match data pulled in from 
        //   pull table --> ensure we can find a match if measured value 
        //   is equal to a file boundary value.
        std::vector<std::string> split1 = SplitString(pullRow[0], '.');
        std::vector<std::string> split2 = SplitString(pullRow[1], '.');
        long prec1 = 0, prec2 = 0;
        if ( 2 == split1.size() )
            prec1 = static_cast<long>(split1[1].size());
        else
            Assert<FileError>(1 == split1.size(), Name());

        if ( 2 == split2.size() )
            prec2 = static_cast<long>(split2[1].size());
        else
            Assert<FileError>(1 == split2.size(), Name()); 

        bool equal = false;
        MType mTemp = measure;
        mTemp.SetPrecision(prec1);
        if ( mTemp.ValueStr() == pullRow[0] )
            equal = true;
        mTemp = measure;
        mTemp.SetPrecision(prec2);
        if ( mTemp.ValueStr() == pullRow[1] )
            equal = true;

        // See if (measure) falls within the current pull boundaries
        MType low = convert<MType>(pullRow[0]), high = convert<MType>(pullRow[1]);
        if ( ((measure >= low) && (measure <= high)) || equal ) {
            if ( Uppercase(pullRow[2]) != varPtr->NoWirebondPull() ) {
                // Cannot use SafeInhibit() if DUT is inhibited
                Assert<UnexpectedState>(!conditions->IsInhibited(), Name());
                spts_->SafeInhibit(ON);
                spts_->PowerDown();
                dbox << "Pull Wirebond: " << pullRow[2] << DialogBox::endl;
                dbox.DisplayInteractive();
                spts_->PowerUp();
                spts_->SafeInhibit(OFF);
            }
            returnType_ = makeRtnType(name(), measure);
            break;
        }
        ++i;
    } // while
    Assert<JumperPull>(i != toPull.end(), Name());
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//============
// VoutPARD()
//============
void VoutPARD::operator()(ConditionsPtr conditions, const PairMType& limits) {

    // Locals
	RTypeSecond mValues;
    OScopeChannels::Channel chan;
    StationNS::ACPathTypes::ImplicitPaths implicit = StationNS::ACPathTypes::VOUTPARD;
    std::size_t numberExplicitTests = 1;
    std::set<OScopeSetupFile::Parameters> unsetParms;

    ConverterOutput::Output outty = conditions->Channel();
    Assert<BadRtnValue>(outty != ConverterOutput::ALL, name());
	LoadTraits::Channels channel = spts_->Convert2LoadChannel(outty);
    chan = spts_->GetScopeChannel(implicit, channel);
	spts_->SetPath(implicit, outty, conditions->BW());

    // Explicit scope settings for this test
    spts_->SetScopeExplicit(chan, StationNS::ExplicitScope::VERTSCALE, 
                           (2*limits.second.Value() / spts_->NumberScopeVertDvns()));

    // Specific, non-explicit scope settings for this test
	unsetParms = spts_->SetScope(append2Name(name(), outty), chan);

    // Make sure explicit and non-explicit settings cover everything
    Assert<FileError>(numberExplicitTests == unsetParms.size(), Name());
    Assert<FileError>(unsetParms.find(OScopeSetupFile::VERTSCALE) !=
                      unsetParms.end(), Name());

    // Make measurement
    try {
        spts_->RescaleScope(chan);
        mValues.push_back(spts_->MeasureScope(OScopeMeasurements::PEAK2PEAK, chan));
    } catch(...) {
        throw;
    }

	// Multiply Pk-Pk measurement by appropriate Xmission line multiplier 
	std::transform(mValues.begin(), mValues.end(), mValues.begin(), 
	               std::bind2nd(std::multiplies<MType>(), 2));

	// Store measured value(s)
	returnType_ = std::make_pair(name(), mValues);
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

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

