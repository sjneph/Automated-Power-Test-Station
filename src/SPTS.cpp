// Files included
#include "Assertion.h"
#include "ConfigureRelays.h"
#include "TestFixtureFile.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "LimitsFile.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"
#include "StationAlgorithms.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   
   
   =================
   03/27/06, HQP,FAC
   =================

   Commented out an unused variable that got commented out on 3/24/2006


   ==============
   11/14/05, sjn,
   ==============
     Added SetIinLimitProtection(Switch = ON/OFF) for the main power supply.  Added
       #include "TestFixtureFile.h", and updated partSpecific() to look for a
       miscellaneous line called out in the fixture file related to power connection
       checking.  We check to ensure such a miscellaneous line is not also called out
       in the var file or lim file anywhere.

   ==============
   10/26/05, sjn,
   ==============
     Modified SetVin() to step to line voltages < Low Line in iterations.  Previously,
       we only did this for Low Line itself.  Also added second parameter which allows
       for power supplies that cannot measure/report their voltage values accurately
       during transitions.  In these cases, we will ensure the voltage is reasonable
       via DMM measurements (assuming you allow it via the second argument).  If false,
       the second argument will never allow a voltage measurement in SetVin() via the
       DMM (can be useful if the DMM is monitoring something else).  This is all
       necessary due to older power supplies (HP6030A and HP6035A) lying to us during
       voltage transitions.
     Modified setLoadMode() to accommodate configurables --> when calculating ohms law
       with >= 1 outputs OFF due to inhibit --> can cause equation blow up.  We will
       set Iout to 0A for that particular output(s) if inhibited.  It is necessary for
       DUT Sanity Checking to be OFF.  Therefore this is almost certainly only
       applicable to configurable special cases.
     Added GetScopeVertScale().
     Modified SetScope() Overload2 --> set trigger source offset to middle of scope
       screen by default when trigger source differs from measure source.

   ==============
   05/10/05, sjn
   ==============
     Added implementation for ResetAllMiscIO().
     Added getAllMiscIO() and modified partSpecific() to reset all Misc IO between each
       test sequence so that there will be no leftover surprises.
     Modified partSpecific() to set/reset primary/secondary APSs for an entire test
       sequence.  Added station APS setups to Initialize().
     Current Probe and Power Path Layout Changes (only single power path now):
     Removed member variables: powerPath_, cpRescale_ and scopeRescale_.
     Modified getRFRelays() to reflect change from IINPARDHIGHCURR and IINPARDLOWCURR
       to simply IINPARD.
     Modified getInputRelays() for same reason as getRFRelays().
     Modified IsError(): Removed powerPath_ specific information.
     Modified MeasureScope() Overload2: Only oscope can auto-rescale now; not current
       probe (fixed scale).
     Modified partSpecific(): definition of powerPath_.
     Modified Constructor: removed initialization of removed member variables.
     Modified customResets(): removed selection of input power path; which current
       probe is in use.
     Modified ProbeDegauss(): no high-current, low-current separation any longer.
       Simplified code by calling SetPath() and ResetPath() rather than explicit 
       calls to instruments.
     Modified resetMemberVariables(): removed references to removed members variables.
     Modified SetPath() Overload2: in reference to IINPARD - no longer separated into
       high current and low current paths.   
     Removed SetProbeScale(): Fixed Scale.
     Removed CurrentProbeAutoRescale(): No longer applicable with fixed scale.
     Removed rescaler() helper: No longer applicable since only scope can rescale.
     Removed OScopeAutoRescale(): Only scope can be selected - no need to explictly
       select it.
     
   ==============
   02/18/05, sjn
   ==============  
     Modified MeasureScope() Overload3 - delay time.  If the first channel to measure
     delay time to is the trigger channel, then we will assume the time to that trigger
     crossing its threshold is zero if we receive a measurement error.  Some oscope
     models cannot reliably make the measurement when the horizontal scaling factor is
     relatively large.

   ==============
   01/18/05, sjn,
   ==============
     Modified NeedsDegauss() to check StationFile in addition to currentProbe_.
     newDUTSetup() was created --> same as previous NewDUTSetup().  NewDUTSetup() was
        changed to call newDUTSetup() and then to ensure cusom resets were used on all
        applicable instruments.  Initialize()'s call to NewDUTSetup() was changed to
        newDUTSetup().  Relays were not being custom-resetted properly between DUT tests.
     partSpecific() was modified --> added call to miscLines_->Initialize() following
        miscLines_.reset(new Misc).  The instrument is locked after construction until
        Initialize() or Reset() is called.
     Made all member function formal parameters start with a lowercase letter.
     Added implementation for new member function: GetTemperatureSetpoint().
     Added a couple #include's that were always needed.
     Removed call to TestFixtureFile->SetTestFixture() from partSpecific (now in main).
     Modified EmergencyShutdown() --> added 'offset' parameter.
     Re-implemented MeasureScope() overloads 2 and 3 for delay time measurements -->
        using TIME2LEVEL now as it is common to oscilloscope vendors while the
        older DELAY method was not.
     Modified SetScope() --> no more external trigger capability on the station.  The
        external files do not need to be changed however.  If External is specified
        as the Trigger Source, that will be interpreted to mean something useful to
        the current station layout.
     Modified the following to reflect station layout changes.  Some of those changes
      include removing the ability to set more than one scope channel/path at once.
      For example, we had the ability to pass in ConverterOutput::ALL in some cases.
      That will now result in an error since parallel scope capabilities are removed.
         SetPath(ACPathTypes::ExplicitPaths exPath, FilterSelects::FilterType bw)
         SetPath(ACPathTypes::ImplicitPaths impPath, ConverterOutput::Output output, 
                                                       FilterSelects::FilterType bw)
         SetScopeExplicit(all overloads)
         GetScopeChannel(ACPathTypes::ExplicitPaths path)
         RescaleScope()
     Removed: ConflictingPaths() --> no longer applicable with new station layout.
     Removed OScopeChannels::Channel Convert2ScopeChannel(LoadTraits::Channels);
     Added implementations for StartScope() and StopScope().
     Modified WaitOnScope(): Ensuring enough time passes for at least 1 full scope
       acquisition prior to returning.
     Modified SetScope(): Added ability to set TRIGVERTSCALE.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    using namespace SPTSInstrument;

    struct DMMTimeoutTag {
        static std::string Name() {
            return("DMM Ready Timeout");
        }
    };
    struct MainSupplyTimeoutTag {
        static std::string Name() {
            return("Main Supply 'VOLTAGE SET' Timeout");
        }
    };
    struct OScopeTimeoutTag {
        static std::string Name() {
            return("OScope Ready Timeout");
        }
    };

    typedef StationExceptionTypes::BaseException StationBaseException;
    typedef DUTExceptionTypes::BaseException DUTBaseException;

    // Homemade Exceptions
    typedef ExceptionTypes::SomeException<
                                          StationExceptionTypes::InstrumentError::ID,
                                          StationBaseException,
                                          DMMTimeoutTag
                                         > DMMTimeout;

    typedef ExceptionTypes::SomeException<
                                          StationExceptionTypes::InstrumentError::ID,
                                          StationBaseException,
                                          MainSupplyTimeoutTag
                                         > MainSupplyTimeout;

    typedef ExceptionTypes::SomeException<
                                          StationExceptionTypes::InstrumentError::ID,
                                          StationBaseException,
                                          OScopeTimeoutTag
                                         > OScopeTimeout;

    // Station Exceptions    
    typedef StationExceptionTypes::BadArg          BadArg;    
    typedef StationExceptionTypes::ContainerState  ContainerState;    
    typedef StationExceptionTypes::DivideByZero    DivideByZero;
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::InfiniteLoop    InfiniteLoop;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::OutOfRange      OutOfRange;
    typedef StationExceptionTypes::RescaleError    RescaleError;
    typedef StationExceptionTypes::ResetError      ResetError;
    typedef StationExceptionTypes::ScopeSet        ScopeSet;
    typedef StationExceptionTypes::Undefined       Undefined;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;

    // DUT Exceptions
    typedef DUTExceptionTypes::SevereOscillation SevereOscillation;
} // unnamed

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // Local functions to assist SPTS implementation


    //================
    // getAllMiscIO()
    //================
    ControlMatrix::Misc::MiscContainer getAllMiscIO() {
        ControlMatrix::Misc::MiscContainer toRtn;
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC1);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC2);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC3);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC4);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC5);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC6);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC7);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC8);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC9);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC10);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC11);
        toRtn.push_back(ControlMatrixTraits::RelayTypes::MISC12);
        return(toRtn);
    }

    //===============
    // getDCRelays()
    //===============
    SwitchMatrix::DCRelayContainer getDCRelays() {    
        // Local Class
        struct LocalClass : SwitchMatrixTraits::RelayTypes {

            SwitchMatrix::DCRelayContainer CreateRelays() {   
                SwitchMatrix::DCRelayContainer dc;
	            dc.push_back(INPUTVOLTAGE);
                dc.push_back(BOXID);
                dc.push_back(RESBOXID);
                dc.push_back(APSPRIMARYVOLTAGE);
                dc.push_back(IINDCONESMALLOHM);
                dc.push_back(IINDCONEMEDIUMOHM);	
                dc.push_back(IINDCONEBIGOHM);
                dc.push_back(IINDCTWOSMALLOHM);
                dc.push_back(IINDCTWOMEDIUMOHM);
                dc.push_back(IINDCTWOBIGOHM);
                dc.push_back(IINDCTHREESMALLOHM);
                dc.push_back(IINDCTHREEMEDIUMOHM);
                dc.push_back(IINDCTHREEBIGOHM);
                dc.push_back(PS1VOLTS);
                dc.push_back(PS2VOLTS);
                dc.push_back(PS3VOLTS);
                dc.push_back(APSSYSTEM5V);
                dc.push_back(APSSYSTEM12V);
                dc.push_back(VOUTDC1);
                dc.push_back(IOUTDC1);
                dc.push_back(VOUTDC2);
                dc.push_back(IOUTDC2);
                dc.push_back(VOUTDC3);
                dc.push_back(IOUTDC3);
                dc.push_back(VOUTDC4);
                dc.push_back(IOUTDC4);
                dc.push_back(VOUTDC5);
                dc.push_back(IOUTDC5);
                dc.push_back(APSSECONDARYVOLTAGE);
                dc.push_back(MISC1);
                dc.push_back(MISC2);
                dc.push_back(MISC3);
                dc.push_back(MISC4);
                dc.push_back(MISC5);
                dc.push_back(MISC6);
                dc.push_back(MISC7);
                dc.push_back(MISC8);
                dc.push_back(MISC9);
                return(dc);
            }
        };  // LocalClass 

        return(LocalClass().CreateRelays());
    }

    //===================
    // getFilterRelays()
    //===================
    SwitchMatrix::RFFilterContainer getFilterRelays() {    
        // Local Class
        struct LocalClass : SwitchMatrixTraits::RelayTypes {

            SwitchMatrix::RFFilterContainer CreateRelays() {   
                SwitchMatrix::RFFilterContainer filt;
                filt.push_back(TWOMHZIN);
                filt.push_back(TWOMHZOUT);
                filt.push_back(TENMHZIN);
                filt.push_back(TENMHZOUT);	
                filt.push_back(PASSTHRUIN);
                filt.push_back(PASSTHRUOUT);
                return(filt);
            }
        };  // LocalClass 

        return(LocalClass().CreateRelays());
    }  

    //==================
    // getInputRelays()
    //==================
    ControlMatrix::InputRelayControl::RelayContainer getInputRelays() {  

        // Local Class
        struct LocalClass : ControlMatrix::RelayTypes {

            ControlMatrix::InputRelayControl::RelayContainer CreateRelays() {   
                ControlMatrix::InputRelayControl::RelayContainer in;
                in.push_back(CONNECTCOMMONS);
                in.push_back(IINDCA);
                in.push_back(IINDCB);
                in.push_back(IINPARD);
                in.push_back(SYNCENABLE);
                in.push_back(SYNCIN);
                in.push_back(SYNCCHECK);
                in.push_back(VINRISE);
                in.push_back(PRIMARYINHIBITRISE);
                in.push_back(PRIMARYINHIBIT); 
                in.push_back(PS1ISOLATION);
                in.push_back(PS2ISOLATION);
                in.push_back(PS3ISOLATION);
                return(in);
            }
        };  // LocalClass 

        return(LocalClass().CreateRelays());
    }

    //===================
    // getOutputRelays()
    //===================
    ControlMatrix::OutputRelayControl::RelayContainer getOutputRelays() {  

        // Local Class
        struct LocalClass : ControlMatrix::RelayTypes {

            ControlMatrix::OutputRelayControl::RelayContainer CreateRelays() {
                ControlMatrix::OutputRelayControl::RelayContainer out;
                out.push_back(SHORT1);
                out.push_back(SHORT2);
                out.push_back(SHORT3);
                out.push_back(SHORT4);
                out.push_back(SHORT5);
                out.push_back(SECONDARYINHIBIT);
                out.push_back(LOADTRIGGER);
                out.push_back(VOUTPARD1);
                out.push_back(LOADTRANSIENT1);
                out.push_back(VOUTPARD2); 
                out.push_back(LOADTRANSIENT2);
                out.push_back(VOUTPARD3);
                out.push_back(LOADTRANSIENT3);
                out.push_back(VOUTPARD4);
                out.push_back(LOADTRANSIENT4);
                out.push_back(VOUTPARD5);
                out.push_back(LOADTRANSIENT5); 
                return(out);
            }
        };  // LocalClass 

        return(LocalClass().CreateRelays());
    }

    //===============
    // getRFRelays()
    //===============
    SwitchMatrix::RFRelayContainer getRFRelays() {  
      
        // Local Class
        struct LocalClass : SwitchMatrixTraits::RelayTypes {

            SwitchMatrix::RFRelayContainer CreateRelays() {   
                SwitchMatrix::RFRelayContainer rf;
                rf.push_back(VOUTPARD1);
                rf.push_back(VOUTPARD2);
                rf.push_back(VOUTPARD3);
                rf.push_back(VOUTPARD4);
                rf.push_back(VOUTPARD5);
                rf.push_back(LOADTRANSIENT1);
                rf.push_back(LOADTRANSIENT2);
                rf.push_back(LOADTRANSIENT3);
                rf.push_back(LOADTRANSIENT4);
                rf.push_back(LOADTRANSIENT5); 
                rf.push_back(IINPARD);
                rf.push_back(SYNCOUT);
                rf.push_back(EXTENDEDTRANSIENT);
                rf.push_back(EXTENDEDPARD);
                rf.push_back(SYNCCHECK);
                rf.push_back(LOADTRIGGER);
                rf.push_back(PRIMARYINHIBITRISE);
                rf.push_back(VINRISE);
                return(rf);
            }
        };  // LocalClass 

        return(LocalClass().CreateRelays());
    }

    //================
    // selectSupply()
    //================
    std::pair<ProgramTypes::SetType, ProgramTypes::SetType> selectSupply() {
        Converter* converter = SingletonType<Converter>::Instance();    
        return(std::make_pair(converter->HighestVinSeen(), 
                              converter->HighestIinSeen()));    
    }

} // end unnamed namespace


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SpacePowerTestStation {

//=============
// Constructor
//=============
SPTS::SPTS() 
	: inputRelays_(0), outputRelays_(0), miscLines_(0), load_(0), switchMatrix_(0), 
      dMM_(0), scope_(0), tempControl_(0), funcGen_(0), currentProbe_(0), 
      mainSupply_(0), auxSupply_(0), pathOpen_(false), setShort_(true), locked_(true), 
      customReset_(false), pSpec_(false), poweredDown_(false), noReset_(false),
      alwaysReset_(false), iinShunt_(StationFile::SMALLOHM), whatError_(""),
      lastVin_(-1), dut_(0), errorInstr_(InstrumentTypes::PS3), name_(Name())
{ /* */ }

//============
// Destructor
//============
SPTS::~SPTS() 
{ /* */ }

//===========================
// Convert2ConverterOutput()
//===========================
ConverterOutput::Output SPTS::Convert2ConverterOutput(LoadTraits::Channels fromChannel) {
	MapDut2Load::const_iterator start = dut2Load_.begin(), stop = dut2Load_.end();
	while ( start->second != fromChannel ) 
		Assert<BadArg>(++start != stop, name_); 
	return(start->first);
}

//========================
// Convert2ExplicitPath()
//========================
ACPathTypes::ExplicitPaths SPTS::Convert2ExplicitPath(ACPathTypes::ImplicitPaths imp, 
                                                      ConverterOutput::Output output) {
    ACPathTypes::ExplicitPaths result = VOUTPARD1;
    LoadTraits::Channels chan = Convert2LoadChannel(output);
	switch(imp) { 
		case VOUTPARD:
			switch(chan) { 
				case LoadTraits::ONE:   result = VOUTPARD1; break;
				case LoadTraits::TWO:   result = VOUTPARD2; break;
				case LoadTraits::THREE: result = VOUTPARD3; break;
				case LoadTraits::FOUR:  result = VOUTPARD4; break;
				case LoadTraits::FIVE:  result = VOUTPARD5; break;
			}; 
            break;
		default: // LoadTransient, StartUpDelay, StartUpOvershoot, ShortCktOvershoot
			switch(chan) {
				case LoadTraits::ONE:   result = LOADTRANS1; break;
				case LoadTraits::TWO:   result = LOADTRANS2; break;
				case LoadTraits::THREE: result = LOADTRANS3; break;
				case LoadTraits::FOUR:  result = LOADTRANS4; break;
				case LoadTraits::FIVE:  result = LOADTRANS5; break;  
			}; 
	}; // switch
    return(result);
}

//=======================
// Convert2LoadChannel()
//=======================
LoadTraits::Channels SPTS::Convert2LoadChannel(ConverterOutput::Output fromChannel) {
	MapDut2Load::const_iterator it = dut2Load_.find(fromChannel);
	Assert<BadArg>(it != dut2Load_.end(), name_); 
	return(it->second);
}

//================
// customResets()
//================
void SPTS::customResets() {
    // Needs to be called before Initialize() and after partSpecific()
    Assert<UnexpectedState>(locked_, name_);
    Assert<UnexpectedState>(pSpec_, name_);
    	
    // Local typedefs and variables
    typedef ControlMatrix::InputRelayControl::RelayStateMap IMap;
    typedef ControlMatrix::OutputRelayControl::RelayStateMap OMap;   
    typedef ControlMatrix::Misc::MiscMap MMap;
    IMap irsm;
    OMap orsm;
    MMap mrsm;
    SwitchMatrix::RFRelayMap rfrsm;
    SwitchMatrix::FilterRelayMap filtrsm;
    SwitchMatrix::DCRelayMap dcrsm;

    // Local variable declarations
    ControlMatrix::InputRelayControl::RelayContainer input, intmp;
    ControlMatrix::OutputRelayControl::RelayContainer output, outtmp;    
    ControlMatrix::Misc::MiscContainer misc;
    SwitchMatrix::RFRelayContainer rftmp;
    SwitchMatrix::RFFilterContainer filttmp;
    SwitchMatrix::DCRelayContainer dctmp;

    // Grab relays to be enabled by default throughout test sequence
    misc   = dut_->EnabledMiscRelays();
    input  = dut_->EnabledInputRelays();
    output = dut_->EnabledOutputRelays();

    // Determine which power supply path to enable
    switch(psIsolation_) {
        typedef ControlMatrixTraits::RelayTypes RT;
        case MainSupplyTraits::PS1:
            input.push_back(RT::PS1ISOLATION);
            break;
        case MainSupplyTraits::PS2:
            input.push_back(RT::PS2ISOLATION);
            break;
        case MainSupplyTraits::PS3:
            input.push_back(RT::PS3ISOLATION);
            break;
        default:
            throw(BadArg(name_));
    };

    // Set up smallest ohm shunt as default always
    input.push_back(ControlMatrixTraits::RelayTypes::IINDCA);
    input.push_back(ControlMatrixTraits::RelayTypes::IINDCB);

    // Push primary inhibit 
    input.push_back(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);

    // Grab relays that are to be unenabled by default
    intmp   = getInputRelays();
    outtmp  = getOutputRelays();
    rftmp   = getRFRelays();
    filttmp = getFilterRelays();
    dctmp   = getDCRelays(); 

    // Set rf relays custom reset
    for ( std::size_t i = 0; i < rftmp.size(); ++i )
        rfrsm.insert(std::make_pair(rftmp[i], OFF));
    
    // Set filt relays custom reset
    for ( std::size_t i = 0; i < filttmp.size(); ++i )
        filtrsm.insert(std::make_pair(filttmp[i], OFF));

    // Set dc relays custom reset
    for ( std::size_t i = 0; i < dctmp.size(); ++i ) 
        dcrsm.insert(std::make_pair(dctmp[i], OFF));

    // Set input relays custom reset
    for ( std::size_t i = 0; i < intmp.size(); ++i ) 
        irsm.insert(std::make_pair(intmp[i], OFF));

    for ( std::size_t i = 0; i < input.size(); ++i ) {
        IMap::iterator found = irsm.find(input[i]);
        Assert<ContainerState>(found != irsm.end(), name_);
        found->second = ON;
    }

    // Set output relays custom reset
    for ( std::size_t i = 0; i < outtmp.size(); ++i ) 
        orsm.insert(std::make_pair(outtmp[i], OFF));

    for ( std::size_t i = 0; i < output.size(); ++i ) {
        OMap::iterator found = orsm.find(output[i]); 
        Assert<ContainerState>(found != orsm.end(), name_);
        found->second = ON;
    }

    // Set misc relays custom reset 
    if ( miscLines_.get() != 0 ) {
        mrsm = miscLines_->GetKnownLines();
        MMap::iterator i = mrsm.begin(), j = mrsm.end();
        while ( i != j ) {
            i->second = OFF;
            ++i;
        }
        ControlMatrix::Misc::MiscContainer::iterator s = misc.begin();
        while ( s != misc.end() ) {
            MMap::iterator found = mrsm.find(*s);
            Assert<ContainerState>(found != mrsm.end(), name_);
            found->second = ON;
            ++s;
        }
        miscLines_->SetCustomReset(mrsm);
    }
    else // Disagreement between misc and miscLines_ ?
        Assert<ContainerState>(misc.empty(), name_);
    
    // Define instrument custom resets 
	inputRelays_->SetCustomReset(irsm);
    outputRelays_->SetCustomReset(orsm);   
    switchMatrix_->SetCustomReset(rfrsm); 
    switchMatrix_->SetCustomReset(filtrsm);
    switchMatrix_->SetCustomReset(dcrsm);
    customReset_ = true;
}

//=========================
// dmmMeasurementCounter()
//=========================
bool SPTS::dmmMeasurementCounter() {
    // Pause
    PauseStates* ps = SingletonType<PauseStates>::Instance();
    Pause(ps->GetPauseValue(PauseStates::MEASUREDMM));

    // Ensure DMM is ready
    static const long dmmCounter = 100;
    long counter = 0;
    while ( ++counter < dmmCounter ) {
        if ( dMM_->OpsComplete() )
            return(true);
    }
    return(false);
}

//=====================
// EmergencyShutdown()
//=====================
void SPTS::EmergencyShutdown(bool resetTemp, const SetType& offset) {    
    Assert<InstrumentError>(Reset(resetTemp), name_);

    if ( resetTemp ) { // go to room temperature
        static const MType roomTemperature = GetRoomTemperature();
        SetType roomTemp = roomTemperature.Value() - offset.Value();
        tempControl_->SetTemperature(roomTemp);
    }
    locked_ = true; // Can call Initialize() now
}

//========================
// GetCurrentProbeScale()
//========================
ProgramTypes::MType SPTS::GetCurrentProbeScale() {
    return(currentProbe_->CurrentScale().Value());
}

//=====================
// GetInputShuntInfo()
//=====================
std::pair< SwitchMatrixTraits::RelayTypes::DCRelay, 
           std::pair<StationFile::IinDCBoard, StationFile::IinShunt> 
         > SPTS::GetInputShuntInfo() {

    SwitchMatrixTraits::RelayTypes::DCRelay relay;       
    StationFile::IinDCBoard brd;
     
    switch(psIsolation_) {                
        case MainSupplyTraits::PS1:
            brd = StationFile::ONE;
            switch(iinShunt_) {
                case StationFile::BIGOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCONEBIGOHM;
                    break;
                case StationFile::MIDOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCONEMEDIUMOHM;
                    break;
                default: // StationFile::SMALLOHM
                    relay = SwitchMatrixTraits::RelayTypes::IINDCONESMALLOHM;
            }; // Inner-Inner switch
            break;
        case MainSupplyTraits::PS2:
            brd = StationFile::TWO; 
            switch(iinShunt_) {
                case StationFile::BIGOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTWOBIGOHM;
                    break;
                case StationFile::MIDOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTWOMEDIUMOHM;
                    break;
                default: // StationFile::SMALLOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTWOSMALLOHM;
            }; // Inner-Inner switch
            break;
        case MainSupplyTraits::PS3:
            brd = StationFile::THREE;
            switch(iinShunt_) {
                case StationFile::BIGOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTHREEBIGOHM;
                    break;
                case StationFile::MIDOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTHREEMEDIUMOHM;
                    break;
                default: // StationFile::SMALLOHM:
                    relay = SwitchMatrixTraits::RelayTypes::IINDCTHREESMALLOHM;
            }; // Inner-Inner switch
            break;
        default:     
            throw(BadArg(name_));
    }; // Inner switch1  
    return(std::make_pair(relay, std::make_pair(brd, iinShunt_)));   
}

//===================
// GetLoadChannels()
//=================== 
const SPTS::LoadChannels& SPTS::GetLoadChannels() const {
    return(activeLoadChannels_);
}

//=================
// GetLoadValues()
//=================
std::vector< std::pair<Switch, ProgramTypes::SetType> > SPTS::GetLoadValues() const {
    std::vector< std::pair<Switch, ProgramTypes::SetType> > toRtn;
    LoadChannels::const_iterator start = activeLoadChannels_.begin();
    LoadChannels::const_iterator stop  = activeLoadChannels_.end();
    while ( start != stop ) {
        toRtn.push_back(load_->GetLoadValue(*start));
        ++start;
    }
    return(toRtn);
}

//===============
// getLoadsOff()
//===============
SPTS::LoadChannels SPTS::getLoads(Switch state) {

    LoadChannels toRtn;
    LoadChannels::const_iterator start = activeLoadChannels_.begin();
    LoadChannels::const_iterator stop  = activeLoadChannels_.end();
    while ( start != stop ) {
        if ( load_->LoadState(*start) == state )
            toRtn.push_back(*start);
        ++start;
    }
    return(toRtn);
}

//=============================
// GetScopeChannel() Overload1
//=============================
OScopeChannels::Channel 
        SPTS::GetScopeChannel(ACPathTypes::ImplicitPaths impPath, 
			                  LoadTraits::Channels chan) const {
	ACPathTypes::ExplicitPaths result = VOUTPARD1;
	switch(impPath) { 
		case VOUTPARD:
			switch(chan) { 
				case LoadTraits::ONE:   result = VOUTPARD1; break;
				case LoadTraits::TWO:   result = VOUTPARD2; break;
				case LoadTraits::THREE: result = VOUTPARD3; break;
				case LoadTraits::FOUR:  result = VOUTPARD4; break;
				case LoadTraits::FIVE:  result = VOUTPARD5; break;
			}; // Inner Switch1
            break;
		case LOADTRANSIENT:    case STARTUPDELAY: 
		case STARTUPOVERSHOOT: case SHORTCKTOVERSHOOT:
			switch(chan) { 
				case LoadTraits::ONE:   result = LOADTRANS1; break;
				case LoadTraits::TWO:   result = LOADTRANS2; break;
				case LoadTraits::THREE: result = LOADTRANS3; break;
				case LoadTraits::FOUR:  result = LOADTRANS4; break;
				case LoadTraits::FIVE:  result = LOADTRANS5; break;  
			}; // Inner-switch2
            break;
		default: 
			throw(BadArg(name_));
	}; // Outer switch
	return(GetScopeChannel(result)); // Call overloaded version
}

//=============================
// GetScopeChannel() Overload2
//=============================
OScopeChannels::Channel 
    SPTS::GetScopeChannel(ACPathTypes::ExplicitPaths path) const {

    OScopeChannels::Channel toRtn;
    switch(path) {            
        case SYNCOUT:          toRtn = OScopeChannels::THREE;   break;
        case SYNCCHECK:        toRtn = OScopeChannels::THREE;   break;
        case LOADTRANS1:       toRtn = OScopeChannels::ONE;     break;
        case LOADTRANS2:       toRtn = OScopeChannels::ONE;     break;
        case LOADTRANS3:       toRtn = OScopeChannels::ONE;    break;
        case LOADTRANS4:       toRtn = OScopeChannels::ONE;     break;
        case LOADTRANS5:       toRtn = OScopeChannels::ONE;     break;
        case VINRISE:          toRtn = OScopeChannels::TRIGGER; break;
        case PRIMARYINHIBIT:   toRtn = OScopeChannels::TRIGGER; break;
        case VOUTPARD1:        toRtn = OScopeChannels::TWO;     break;
        case VOUTPARD2:        toRtn = OScopeChannels::TWO;     break;
        case VOUTPARD3:        toRtn = OScopeChannels::TWO;     break;
        case VOUTPARD4:        toRtn = OScopeChannels::TWO;     break;
        case VOUTPARD5:        toRtn = OScopeChannels::TWO;     break;
        case IINPARD:          toRtn = OScopeChannels::TWO;     break;
        default:
            throw(BadArg(name_));
    }; // Outer switch
    return(toRtn);
}

//=====================
// GetScopeVertScale()
//=====================
SPTS::SetType SPTS::GetScopeVertScale(OScopeChannels::Channel chan) const {
    return(scope_->GetVertScale(chan));
}

//==========================
// GetTemperatureSetpoint()
//==========================
ProgramTypes::SetType SPTS::GetTemperatureSetpoint() const {
    if ( tempControl_->IsOn() )
        return(tempControl_->GetTemperatureSetting());
    return(std::numeric_limits<ProgramTypes::SetType::ValueType>::min());
}

//==========
// GetVin() 
//==========
ProgramTypes::SetType SPTS::GetVin() const {	
    if ( ! mainSupply_->IsOn() )
        return(0);
	return(mainSupply_->GetVolts());
}

//==============
// Initialize()
//==============
void SPTS::Initialize(bool resetTemp) {
    Assert<UnexpectedState>(locked_, name_);
    bool toChange = false;    

    try {
        // Instrument member variable initializations
        dMM_.reset(new DMM);
        scope_.reset(new Oscilloscope);        
        funcGen_.reset(new FunctionGenerator);
        currentProbe_.reset(new CurrentProbe);
        auxSupply_.reset(new AuxSupply);
	    inputRelays_.reset(new ControlMatrix::InputRelayControl(getInputRelays()));
        outputRelays_.reset(new ControlMatrix::OutputRelayControl(getOutputRelays()));
        switchMatrix_.reset(new SwitchMatrix(getRFRelays(), 
                                             getDCRelays(), 
                                             getFilterRelays())
                                             );
        if ( !customReset_ ) { // newDUTSetup() deals with customReset_
            customReset_ = true;
            toChange = true;
        } 
        locked_ = false;
        auxSupply_->Initialize();
        SetAPS(APS::SYSTEM5V, ON);
        SetAPS(APS::SYSTEM12V, ON);
        SetAPS(APS::PRIMARY, OFF);
        SetAPS(APS::SECONDARY, OFF);
        SetAPS(APS::SYSTEM5V, APS::VOLTS, APS::FIVE);   
        SetAPS(APS::SYSTEM5V, APS::CURRENT, APS::MAX); 
        SetAPS(APS::SYSTEM12V, APS::VOLTS, APS::TWELVE);
        SetAPS(APS::SYSTEM12V, APS::CURRENT, APS::MAX);
        SetAPS(APS::PRIMARY, APS::CURRENT, APS::MAX);
        SetAPS(APS::SECONDARY, APS::CURRENT, APS::MAX);
        inputRelays_->Underlying()->Initialize(); 
        newDUTSetup(); // other member vars - instruments - taken care of here              
        scope_->AddScopeChannel(OScopeChannels::ONE);
        scope_->AddScopeChannel(OScopeChannels::TWO);
        scope_->AddScopeChannel(OScopeChannels::THREE);
        scope_->AddScopeChannel(OScopeChannels::TRIGGER);
        scope_->Initialize();
        SetScope(OScopeChannels::ALL, ON);
        dMM_->Initialize();
        if ( resetTemp || (tempControl_.get() == 0) ) {
            tempControl_.reset(new TemperatureController);
            tempControl_->Initialize();
            setTemperatureBaseLimits();            
        }
        funcGen_->Initialize();
        currentProbe_->Initialize();
        inputRelays_->Initialize();
        outputRelays_->Initialize();
        switchMatrix_->Initialize();
        if ( miscLines_.get() != 0 )
            miscLines_->Initialize();
        alwaysReset_ = true;
        ResetPath();
        alwaysReset_ = false;
    } catch(...) {
        locked_ = true;
        if ( toChange )
            customReset_ = false;
        throw; // rethrow exception
    }
} 

//===========
// IsError()
//===========
bool SPTS::IsError() {   
    // whatError_ is not cleared until WhatError() is called 
    if ( ! whatError_.empty() )
        return(true);

    bool result = true;

    // Determine if any instrument has an error condition
    //   if so, record what error and which instrument
    if ( load_->IsError() ) {
        whatError_ = load_->WhatError();
        if ( load_->LoadType() == LoadTraits::ELECTRONIC )
            errorInstr_ = InstrumentTypes::ELECTRONICLOAD;
        else
            errorInstr_ = InstrumentTypes::RLL;
    }
    else if ( mainSupply_->IsError() ) {
        whatError_ = mainSupply_->WhatError();
        switch(mainSupply_->WhichSupply()) {            
            case MainSupplyTraits::PS1:
                errorInstr_ = InstrumentTypes::PS1;
                break;
        case MainSupplyTraits::PS2:
                errorInstr_ = InstrumentTypes::PS2;
                break;
        case MainSupplyTraits::PS3:
                errorInstr_ = InstrumentTypes::PS3;
                break;        
        };
    }
    else if ( switchMatrix_->IsError(SwitchMatrix::RF) ) {
        whatError_ = switchMatrix_->WhatError(SwitchMatrix::RF);
        errorInstr_ = InstrumentTypes::SWITCHMATRIXRF;
    }
    else if ( switchMatrix_->IsError(SwitchMatrix::DC) ) {
        whatError_ = switchMatrix_->WhatError(SwitchMatrix::DC);
        errorInstr_ = InstrumentTypes::SWITCHMATRIXDC;
    }
    else if ( switchMatrix_->IsError(SwitchMatrix::FILTER) ) {
        whatError_ = switchMatrix_->WhatError(SwitchMatrix::FILTER);
        errorInstr_ = InstrumentTypes::SWITCHMATRIXFILTER;
    }
    else if ( inputRelays_->IsError() ) {
        whatError_ = inputRelays_->WhatError();
        errorInstr_ = InstrumentTypes::INPUTRELAYCONTROL;
    }
    else if ( outputRelays_->IsError() ) {
        whatError_ = outputRelays_->WhatError();
        errorInstr_ = InstrumentTypes::OUTPUTRELAYCONTROL;
    }
    else if ( auxSupply_->IsError() ) {
        whatError_ = auxSupply_->WhatError();
        errorInstr_ = InstrumentTypes::APS;
    }
    else if ( dMM_->IsError() ) {
        whatError_ = dMM_->WhatError();
        errorInstr_ = InstrumentTypes::DMM;
    }
    else if ( currentProbe_->IsError() ) {
        whatError_ = currentProbe_->WhatError();
        errorInstr_ = InstrumentTypes::CURRENTPROBE;
    }
    else if ( scope_->IsError() ) {
        whatError_ = scope_->WhatError();
        errorInstr_ = InstrumentTypes::OSCOPE;
    }
    else if ( tempControl_->IsError() ) {
        whatError_ = tempControl_->WhatError();
        errorInstr_ = InstrumentTypes::TEMPCONTROLLER;
    }
    else if ( funcGen_->IsError() ) {
        whatError_ = funcGen_->WhatError();
        errorInstr_ = InstrumentTypes::FUNCTIONGENERATOR;
    }
    else
        result = false;    
	
    if ( miscLines_.get() != 0 ) {
        if ( miscLines_->IsError() ) {
            result = true;
            whatError_ = miscLines_->WhatError();
            errorInstr_ = InstrumentTypes::MISC;
        }        
    }
    return(result);
}

//====================
// LoadTransientOff()
//====================
void SPTS::LoadTransientOff() {
    LoadChannels::iterator i = activeLoadChannels_.begin();
    try {
        load_->Concatenate(ON);
        while ( i != activeLoadChannels_.end() ) {
            load_->TransientOff(*i);
            ++i;
        }
        load_->Concatenate(OFF);
    } catch(...) {
        load_->ImmediateMode();
        throw;
    }
}

//========================
// LoadTransientTrigger()
//========================
void SPTS::LoadTransientTrigger() {
    load_->TransientOn();    
}

//============
// LoadType()
//============
LoadTraits::Types SPTS::LoadType() {
    return(load_->LoadType());
}

//===================
// MeasureAPSVolts()
//===================
SPTS::MType SPTS::MeasureAPSVolts(APS::Channel channel) {
    typedef SwitchMatrixTraits::RelayTypes SMR;
    SMR::DCRelay relay = SMR::APSPRIMARYVOLTAGE;
	switch(channel) {
		case APS::PRIMARY:   relay = SMR::APSPRIMARYVOLTAGE;   break;
		case APS::SECONDARY: relay = SMR::APSSECONDARYVOLTAGE; break;
		case APS::SYSTEM12V: relay = SMR::APSSYSTEM12V;        break;
		case APS::SYSTEM5V:  relay = SMR::APSSYSTEM5V;         break;
	}; // switch
	switchMatrix_->Close(relay);
	dMM_->SetRange(DMM::AUTO);
	MType measured = MeasureDCV();
	switchMatrix_->Open(relay);
	return(measured);
}

//===================
// MeasureBaseTemp()
//===================
SPTS::MType SPTS::MeasureBaseTemp() {
    return(tempControl_->CurrentTemperature());
}

//==============
// MeasureDCV()
//==============
SPTS::MType SPTS::MeasureDCV() {
    Assert<DMMTimeout>(dmmMeasurementCounter(), name_);
    return(dMM_->MeasureDCVolts());
}

//=========================
// MeasureDUTTemperature()
//=========================
SPTS::MType SPTS::MeasureDUTTemperature() {
    Assert<DMMTimeout>(dmmMeasurementCounter(), name_);
    return(dMM_->MeasureTemperature());
}

//======================
// MeasureLoadCurrent()
//======================
SPTS::MType SPTS::MeasureLoadCurrent(LoadTraits::Channels chan) {
    return(load_->MeasureAmps(chan));
}

//====================
// MeasureLoadVolts()
//====================
SPTS::MType SPTS::MeasureLoadVolts(LoadTraits::Channels chan) {
    return(load_->MeasureVolts(chan));
}

//===============
// MeasureOhms()
//===============
SPTS::MType SPTS::MeasureOhms() {
    Assert<DMMTimeout>(dmmMeasurementCounter(), name_); 
    return(dMM_->MeasureOhms());
}

//==========================
// MeasureScope() Overload1
//==========================
SPTS::MType SPTS::MeasureScope(OScopeMeasurements::MeasurementType mType, 
                               OScopeChannels::Channel chan, bool toPause, 
                               bool toRescale) {

    if ( toPause )
        measureScopePause();
    MType toRtn = scope_->Measure(mType, chan);
    bool clipping = scope_->IsClipping();
    if ( clipping && toRescale ) { // rescale waveform                             
        bool done = false; 
        while ( ! done ) {
            scope_->RescaleUp(chan);
            measureScopePause();
            toRtn = scope_->Measure(mType, chan); 
            if ( !scope_->IsClipping() ) break;
        } // while
        clipping = false;
    }
    Assert<RescaleError>(!clipping, Name());
    return(toRtn);
}

//==========================
// MeasureScope() Overload2
//==========================
SPTS::MType SPTS::MeasureScope(OScopeMeasurements::MeasurementType type, 
                               OScopeChannels::Channel chan1, 
                               OScopeChannels::Channel chan2,
                               const ProgramTypes::PlusMinusPercentType& percChan1, 
                               const ProgramTypes::PlusMinusPercentType& percChan2,
                               OScopeParameters::SlopeType slope1,
                               OScopeParameters::SlopeType slope2,
                               bool toPause) {

    //static const std::string msg = "No AUTO trigger mode for delay measurement";
    //this is an unused variable that got commented out on 3/24/2006
    Assert<BadArg>(type == OScopeMeasurements::DELAY, Name() + ": Not Delay Measure?");
    if ( toPause )
        measureScopePause();

    OScopeMeasurements::MeasurementType mType1, mType2;
    if ( slope1 == OScopeParameters::POSITIVE )
        mType1 = OScopeMeasurements::HIGHVALUE;
    else
        mType1 = OScopeMeasurements::LOWVALUE;

    if ( slope2 == OScopeParameters::POSITIVE )
        mType2 = OScopeMeasurements::HIGHVALUE;
    else
        mType2 = OScopeMeasurements::LOWVALUE;

    bool noPause = false, noRescale = false; // paused already if applicable
    MType ref1 = MeasureScope(mType1, chan1, noPause, noRescale);
    Assert<RescaleError>(!scope_->IsClipping(), Name()); // chan1 clipping?
    MType ref2 = MeasureScope(mType2, chan2, noPause, noRescale);
    Assert<RescaleError>(!scope_->IsClipping(), Name()); // chan2 clipping?
    ref1 *= percChan1.Value();
    ref2 *= percChan2.Value();
    ref1 /= 100;
    ref2 /= 100;
    SetType reference1(ref1.Value());
    SetType reference2(ref2.Value());
    return(
           MeasureScope(type, chan1, chan2, reference1, reference2,
                        slope1, slope2, noPause)
          );
}

//==========================
// MeasureScope() Overload3
//==========================
SPTS::MType SPTS::MeasureScope(OScopeMeasurements::MeasurementType type, 
                               OScopeChannels::Channel chan1, 
                               OScopeChannels::Channel chan2, 
                               const ProgramTypes::SetType& refLevel1, 
                               const ProgramTypes::SetType& refLevel2,
                               OScopeParameters::SlopeType slope1,
                               OScopeParameters::SlopeType slope2,
                               bool toPause) {

    static const std::string msg = "No AUTO trigger mode for delay measurement";
    Assert<BadArg>(type == OScopeMeasurements::DELAY, Name() + ": Not Delay Measure?");
    if ( toPause )
        measureScopePause();

    MType time1 = scope_->Measure(OScopeMeasurements::TIME2LEVEL, chan1, 
                                  refLevel1, slope1);
    if ( scope_->IsClipping() )
	{ // not likely a clip: just too small to measure
        // Some oscilloscope models do not have error messages - just 9.9e37
        // If chan1 is TRIGGER, then assume that it is really close to zero
        Assert<RescaleError>(chan1 == OScopeChannels::TRIGGER, Name());
        time1 = 0;
    }
    MType time2 = scope_->Measure(OScopeMeasurements::TIME2LEVEL, chan2,
                                  refLevel2, slope2);
    Assert<RescaleError>(!scope_->IsClipping(), Name()); // chan2 clipping?
    Assert<StationExceptionTypes::ScopeMeasure>(time2 >= time1, Name());
    return(time2-time1);
}

//=====================
// measureScopePause()
//=====================
void SPTS::measureScopePause() {
    PauseStates* ps = SingletonType<PauseStates>::Instance();
    Pause(ps->GetPauseValue(PauseStates::MEASURESCOPE));
}

//========
// Name()
//========
std::string SPTS::Name() const {
    return("SPTS Class");
}

//================
// NeedsDegauss()
//================
bool SPTS::NeedsDegauss() {
    return(
           currentProbe_->NeedsDegauss() || 
           SingletonType<StationFile>::Instance()->NeedDegauss()
          );
}

//===============
// newDUTSetup()
//===============
void SPTS::newDUTSetup() {    
    Assert<UnexpectedState>(!locked_, name_);
    dut_ = SingletonType<Converter>::Instance();

    // Redefine instruments which may change on any test sequence change    
    load_.reset(new Load);
    mainSupply_.reset(new MainSupply(selectSupply()));

    noReset_     = false;
    psIsolation_ = mainSupply_->WhichSupply();          
    pSpec_       = false; // must call partSpecific() first
    customReset_ = false; // must call customResets() second
    locked_      = true;  
    partSpecific();       // maps/others which require specific converter info
    customResets();       // deal with new instrument custom reset info and APS's
    locked_      = false;
    
    load_->Initialize();    
    mainSupply_->Initialize();
    mainSupply_->OutputOn();
}

//===============
// NewDUTSetup()
//===============
void SPTS::NewDUTSetup() {
    // same as newDUTSetup() but custom resets are guaranteed
    newDUTSetup();
    alwaysReset_ = true;
    ResetPath(); // ensure custom resets are set
    alwaysReset_ = false;    
}

//===================
// NumberScopeDvns()
//===================
long SPTS::NumberScopeVertDvns() {
    return(scope_->NumberVerticalDivisions());
}

//================
// partSpecific()
//================
void SPTS::partSpecific() {
    // Needs to be called before Initialize() and customReset()
    Assert<UnexpectedState>(locked_, name_);
    Assert<UnexpectedState>(!customReset_, name_);

    //===========================
    // Start load configuaration
    //===========================
    // Grab load channels actually needed to test DUT  
    long numOuts = static_cast<long>(dut_->NumberOutputs());
    LoadChannels v = SingletonType<TestFixtureFile>::Instance()->GetLoadsWired();
    typedef std::vector< std::pair<ConverterOutput::Output, LoadTraits::Channels> > 
                                                                           PairVec;
    VariablesFile* vf = SingletonType<VariablesFile>::Instance();
    PairVec pv = vf->GetLoadsMap(numOuts);
    LoadChannels w = vf->GetAllLoadsUsed(numOuts);   
    LoadChannels::iterator wstart = w.begin();
    while ( wstart != w.end() ) { // Ensure we are using only wired load channels
        LoadChannels::iterator tmp = std::find(v.begin(), v.end(), *wstart);
        Assert<FileError>(tmp != v.end(), name_);
        ++wstart;
    }
    for ( std::size_t idx = 0; idx < w.size(); ++idx ) 
        Assert<UnexpectedState>(load_->AddLoadChannel(w[idx]), Name());

    // Deal with parallel loading capability
    std::pair<bool, LoadChannels> p;
    std::vector<ConverterOutput::Output> outputs = dut_->Outputs();
    std::vector<ConverterOutput::Output>::iterator outi = outputs.begin();
    while ( outi != outputs.end() ) {
        p = SingletonType<VariablesFile>::Instance()->GetParallelLoads(*outi);
        if ( p.first ) {
            std::size_t size = p.second.size(), counter = 1;
            LoadTraits::Channels chan;
            load_->ResetParallelLoads();
            while ( counter < size ) { // parallel another load channel                 
                chan = load_->ParallelLoads(p.second[0], p.second[counter]);
                LoadChannels::iterator iter;
                iter = std::find(w.begin(), w.end(), p.second[counter]);
                w.erase(iter); // remove paralleled channel from w
                ++counter;
            } // while
        } // if
        ++outi;
    } // for-loop

    // Make sure there is one load channel per converter output
    Assert<FileError>(numOuts == static_cast<long>(w.size()), name_);

    // Define dut2Load_; 
    dut2Load_.erase(dut2Load_.begin(), dut2Load_.end());  
    PairVec::iterator i = pv.begin();
    while ( i != pv.end() ) {
        dut2Load_.insert(*i);
        ++i;
    }
	Assert<FileError>(!dut2Load_.empty(), name_);
	
    // Define activeLoadChannels_
	MapDut2Load::iterator start = dut2Load_.begin();
    activeLoadChannels_.erase(activeLoadChannels_.begin(), 
                              activeLoadChannels_.end());
	while ( start != dut2Load_.end() ) { 
		activeLoadChannels_.push_back(start->second);
		++start;
	}
    //=========================
    // End load configuaration
    //=========================    

    // Ensure all misc io lines are reset from any previous test sequence
    ResetAllMiscIO();

    // Deal with miscellaneous lines --> grab only what we need for this DUT
    LimitsFile* lptr = SingletonType<LimitsFile>::Instance();
    lptr->RestartSameTest(); // ensure we start from the beginning
    std::set<std::string> misc;
    for ( std::size_t idx = 0; ; ) {
        std::vector<std::string> tmp = lptr->GetPretestMisc();
        std::vector<std::string> tmp2 = lptr->GetMidtestMisc(); 
        std::copy(tmp.begin(), tmp.end(), std::inserter(misc, misc.begin()));
        std::copy(tmp2.begin(), tmp2.end(), std::inserter(misc, misc.begin()));
        if ( ++idx >= lptr->NumberTests() ) 
            break;
        ++(*lptr); // just increment to next test
    }
    lptr->RestartSameTest(); // reset test sequence info
    ControlMatrix::Misc::MiscContainer mcont(dut_->EnabledMiscRelays());

    // Ensure sequence wide miscellaneous lines used do not interfere with
    //   any specific test step's lines used
    ControlMatrix::Misc::MiscContainer toCompare, intersect, merger;    
    std::transform(misc.begin(), misc.end(),
                   std::inserter(toCompare, toCompare.begin()), ConvertToMisc);
    std::sort(toCompare.begin(), toCompare.end());
    std::sort(mcont.begin(), mcont.end());

    // Check if power connection miscellaneous line is set
    TestFixtureFile* ff = SingletonType<TestFixtureFile>::Instance();
    std::pair<bool, std::string> powerCheck = ff->GetPowerConnectionMiscLine();
    if ( powerCheck.first ) {
        ControlMatrixTraits::RelayTypes::MiscRelay mr =
                                                   ConvertToMisc(powerCheck.second);
        Assert<FileError>(std::find(toCompare.begin(),
                                    toCompare.end(), mr) == toCompare.end(), name_);
        toCompare.push_back(ConvertToMisc(powerCheck.second));
        std::sort(toCompare.begin(), toCompare.end());
    }

    // Cannot intermingle misc lines used in LIM, FIXTURE and VAR files
    std::set_intersection(mcont.begin(), mcont.end(), 
                          toCompare.begin(), toCompare.end(),
                          std::inserter(intersect, intersect.begin()));
    Assert<FileError>(0 == intersect.size(), name_);
    std::merge(toCompare.begin(), toCompare.end(), mcont.begin(), mcont.end(), 
               std::inserter(merger, merger.begin()));

     
	if ( !merger.empty() ) {
        miscLines_.reset(new ControlMatrix::Misc(merger));
		miscLines_->Initialize();
	}
    else
        miscLines_.reset(0);

    // See if Primary/Secondary Supplies are to be used
    //  for the whole sequence.  If not, reset them.
    std::pair<bool, ProgramTypes::SetType> setSupply;
    setSupply = SingletonType<VariablesFile>::Instance()->PrimaryAuxSupply();
    if ( setSupply.first ) {
        SetAPS(APS::PRIMARY, ON);
        SetAPS(APS::PRIMARY, APS::VOLTS, setSupply.second);
    }
    else {
        SetAPS(APS::PRIMARY, OFF);
        SetAPS(APS::PRIMARY, APS::VOLTS, 0);
    }
        
    setSupply = SingletonType<VariablesFile>::Instance()->SecondaryAuxSupply();
    if ( setSupply.first ) {
        SetAPS(APS::SECONDARY, ON);
        SetAPS(APS::SECONDARY, APS::VOLTS, setSupply.second);
    }
    else {
        SetAPS(APS::SECONDARY, OFF);
        SetAPS(APS::SECONDARY, APS::VOLTS, 0);
    }

    pSpec_ = true;
}

//=============
// PowerDown() 
//=============
void SPTS::PowerDown() {
    // Load, then Power Supply
    SetLoad(LoadTraits::ALL, OFF);
    lastVin_ = mainSupply_->GetVolts();
    SetVin(static_cast<SetType>(0));
    poweredDown_ = true;
}

//===========
// PowerUp() 
//===========
void SPTS::PowerUp() {  
    // Power Supply, then Load
    Assert<UnexpectedState>(poweredDown_, name_);
    SetVin(lastVin_);
    SetLoad(LoadTraits::ALL, ON);
    poweredDown_ = false;
}

//================
// ProbeDegauss()
//================
void SPTS::ProbeDegauss() {
    Assert<UnexpectedState>(!locked_, name_);
    try {
        SetPath(IINPARD, FilterSelects::PASSTHRU);
        Assert<InstrumentError>(currentProbe_->SystemDegauss(), name_);
        ResetPath(IINPARD, FilterSelects::PASSTHRU);
    } catch(...) { // clean up and rethrow exception    
        ResetPath(IINPARD, FilterSelects::PASSTHRU);
        throw;
    }  
    SingletonType<StationFile>::Instance()->UpdateDegauss();
}

//================
// RescaleScope() 
//================
void SPTS::RescaleScope(OScopeChannels::Channel chan) {
    // This routine is designed for AUTO-mode waveforms with offsets
    //   at the 0 (ground) level.  Do not use this routine otherwise.
    Assert<BadArg>(chan != OScopeChannels::ALL, name_);
    ProgramTypes::MType m = MeasureScope(OScopeMeasurements::PEAK2PEAK, chan);
    m /= (NumberScopeVertDvns() - 4); // minus for margin
    SetScopeExplicit(chan, ExplicitScope::VERTSCALE, m.Value());
  
    // Deal with resolution problems if original vertical scale was very large
    //  relative to the signal we measured --> the following will add vertical 
    //  gain to the waveform measured until it is at least (xDivisions) scope 
    //  divisions.
    bool done = false;
    long cntr = 0;
    ProgramTypes::SetType xDivisions = 1;
    ProgramTypes::SetType n;
    std::pair<ProgramTypes::SetType, ProgramTypes::SetType> vRange = 
                                                           scope_->GetVertRange(chan);

    bool toPause = true;
    bool rescale = true;
    while ( ! done ) {
        Assert<SevereOscillation>(++cntr < 10, name_);
        n = MeasureScope(OScopeMeasurements::PEAK2PEAK, chan, toPause, rescale).Value();
        if ( n >= xDivisions * scope_->GetVertScale(chan) )
            break;
        else if ( scope_->IsClipping() )
            break;
        else if ( vRange.first == scope_->GetVertScale(chan) ) // at minimum scale
            break;
        SetScopeExplicit(chan, ExplicitScope::VERTSCALE, (m /= 2).Value());
    }
}

//=========
// Reset()
//=========
bool SPTS::Reset(bool resetController) {
    // Real instrument resets, not custom resets

    // Inhibit the DUT first
    StrongInhibit(ON);

	ControlMatrix* cm = inputRelays_->Underlying();    

    // Make sure Concatenate() is OFF
    load_->ImmediateMode();
    scope_->ImmediateMode();

    // Reset instruments --> each is contained within its own try-catch
    //   to temporarily suppress exceptions --> to try to reset as much
    //   of the station as possible even in the event that an instrument
    //   isn't working.
    std::string stationError = "";
    bool toAssert = true;
    try {
        try {
            Assert<InstrumentError>(load_->Reset(), load_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        } 
        try {
            Assert<InstrumentError>(mainSupply_->Reset(), mainSupply_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(switchMatrix_->Reset(), switchMatrix_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(cm->Initialize(), cm->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(inputRelays_->Reset(), inputRelays_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(outputRelays_->Reset(), outputRelays_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(auxSupply_->Reset(), auxSupply_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(dMM_->Reset(), dMM_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(currentProbe_->Reset(), currentProbe_->Name());     
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(scope_->Reset(), scope_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }        
        try {
            if ( resetController )
                Assert<InstrumentError>(tempControl_->Reset(), tempControl_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {
            Assert<InstrumentError>(funcGen_->Reset(), funcGen_->Name());
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
            stationError += " ";
        }
        try {    
            if ( miscLines_.get() != 0 )
                Assert<InstrumentError>(miscLines_->Reset());         
        } catch(StationBaseException& error) {
            toAssert = false;
            stationError += error.GetExceptionInfo();
        }
    } catch(...) { // Unknown exception type was thrown
        resetMemberVariables();
        throw(ResetError(name_ + " " + stationError));
    }

    resetMemberVariables();
    Assert<ResetError>(toAssert, stationError);
	return(true);
}

//==================
// ResetAllMiscIO()
//==================
void SPTS::ResetAllMiscIO() {
    try {
        miscLines_.reset(new ControlMatrix::Misc(getAllMiscIO()));
        miscLines_->Initialize();
        miscLines_.reset(0);
    } catch(...) {
        miscLines_.reset(0);
    }
}

//========================
// resetMemberVariables()
//========================
void SPTS::resetMemberVariables() {
    // Only to be called from Reset()
    locked_       = false;
    whatError_    = "";
    pathOpen_     = false;
    setShort_     = true;
    poweredDown_  = false;
    lastVin_      = -1;
    iinShunt_     = StationFile::BIGOHM;
}

//=======================
// ResetPath() Overload1
//=======================
void SPTS::ResetPath(ACPathTypes::ImplicitPaths impPath,
                     ConverterOutput::Output output, FilterSelects::FilterType bw) {
	pathOpen_ = true;
    try {
	    SetPath(impPath, output, bw);
    } catch(...) {        
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//=======================
// ResetPath() Overload2
//=======================
void SPTS::ResetPath(ACPathTypes::ExplicitPaths exPath, 
							      FilterSelects::FilterType bw) {
	pathOpen_ = true;
    try {
	    SetPath(exPath, bw);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//=======================
// ResetPath() Overload3
//=======================
void SPTS::ResetPath(ControlMatrixTraits::RelayTypes::MiscRelay relay) {
    Assert<UnexpectedState>(miscLines_.get() != 0, name_);
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//=======================
// ResetPath() Overload4
//=======================
void SPTS::ResetPath(ControlMatrixTraits::RelayTypes::InputRelay relay) {
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//=======================
// ResetPath() Overload5
//=======================
void SPTS::ResetPath(ControlMatrixTraits::RelayTypes::OutputRelay relay) {
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//=======================
// ResetPath() Overload6
//=======================
void SPTS::ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                                                          relays) {
    pathOpen_ = true;
    try {
        SetPath(relays);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
    pathOpen_ = false;
}

//=======================
// ResetPath() Overload7
//=======================
void SPTS::ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::InputRelay>& 
                                                                           relays) {
    pathOpen_ = true;
    try {
        SetPath(relays);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
    pathOpen_ = false;
}

//=======================
// ResetPath() Overload8
//=======================
void SPTS::ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::OutputRelay>& 
                                                                            relays) {
    pathOpen_ = true;
    try {
        SetPath(relays);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
    pathOpen_ = false;
}

//=======================
// ResetPath() Overload9
//=======================
void SPTS::ResetPath(SwitchMatrixTraits::RelayTypes::DCRelay relay) {
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//========================
// ResetPath() Overload10
//========================
void SPTS::ResetPath(SwitchMatrixTraits::RelayTypes::FilterRelay relay) {
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;         
}

//========================
// ResetPath() Overload11
//========================
void SPTS::ResetPath(SwitchMatrixTraits::RelayTypes::RFRelay relay) {
    pathOpen_ = true;
    try {
        SetPath(relay);
    } catch(...) {
        pathOpen_ = false;
        throw;
    }
	pathOpen_ = false;
}

//========================
// ResetPath() Overload12
//========================
void SPTS::ResetPath() {

    // We only SafeInhibit and CustomReset if a change is detected
    //   with any of our relay control instruments
    bool miscChange = false, inhSet = false;
    if ( miscLines_.get() != 0 )
        miscChange = miscLines_->ChangedState();

    // InputRelayControl --> if Primary Inhibit is currently set, then
    //   custom reset everything to ensure it gets unset
    ControlMatrix::InputRelayControl::RelayContainer v;
    ControlMatrixTraits::RelayTypes::InputRelay r = 
                       ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT;
    v.push_back(r);
    if ( inputRelays_->CurrentState(r) == ON )
        inhSet = true; 

    if ( 
          alwaysReset_                                      ||
          inhSet                                            ||
          miscChange                                        ||
          inputRelays_->ChangedState(v)                     ||
          outputRelays_->ChangedState()                     ||
          switchMatrix_->ChangedState(SwitchMatrix::RF)     ||
          switchMatrix_->ChangedState(SwitchMatrix::FILTER) ||
          switchMatrix_->ChangedState(SwitchMatrix::DC)
       ) { 
        SafeInhibit(ON);
        inputRelays_->CustomReset();
        outputRelays_->CustomReset();
        switchMatrix_->CustomReset(SwitchMatrix::RF);
        switchMatrix_->CustomReset(SwitchMatrix::FILTER);
        switchMatrix_->CustomReset(SwitchMatrix::DC);
        if ( miscChange )
            miscLines_->CustomReset();
        SafeInhibit(OFF);
    }
    iinShunt_ = StationFile::SMALLOHM;    
}

//=============
// ResetSync()
//=============
void SPTS::ResetSync() {
    ControlMatrix::InputRelayControl::RelayContainer r;
    r.push_back(ControlMatrixTraits::RelayTypes::SYNCENABLE);
    r.push_back(ControlMatrixTraits::RelayTypes::SYNCIN);
    SafeInhibit(ON);    
    ResetPath(r);    
    funcGen_->OutputOff();
    SafeInhibit(OFF);    
}

//===============
// SafeInhibit()
//===============
void SPTS::SafeInhibit(Switch type) {
    typedef SingletonType<PauseStates> PS;
    std::vector<LoadTraits::Channels> v;
    switch(type) {        
        case OFF:
            v = getLoads(OFF);
            SetLoad(LoadTraits::ALL, OFF);
            inputRelays_->TurnOff(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);            
            Pause(PS::Instance()->GetPauseValue(PauseStates::SAFEINHIBITOFF));
            setLoadAllExcept(ON, v); // don't turn ON any that were originally OFF
            break;
        case ON:
            inputRelays_->TurnOn(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);
            temporaryPreloadDUT();
    };
}

//====================
// selectFilterPath()
//====================
std::pair<SwitchMatrixTraits::RelayTypes::FilterRelay, 
          SwitchMatrixTraits::RelayTypes::FilterRelay> 
    SPTS::selectFilterPath(FilterSelects::FilterType bw) {

    typedef SwitchMatrixTraits::RelayTypes Resolution;
	std::pair<Resolution::FilterRelay, Resolution::FilterRelay> toRtn;   

    switch(bw) {
        case TWOMHZ:          
            toRtn.first  = Resolution::TWOMHZIN;
            toRtn.second = Resolution::TWOMHZOUT;
            break;
        case TENMHZ:  
            toRtn.first  = Resolution::TENMHZIN;
            toRtn.second = Resolution::TENMHZOUT;
            break;
        case PASSTHRU: 
            toRtn.first  = Resolution::PASSTHRUIN;
            toRtn.second = Resolution::PASSTHRUOUT;
            break;
        default:
            throw(BadArg(name_));
    };  // Inner-Switch1    
	return(toRtn);
}

//====================
// SetAPS() Overload1
//====================
void SPTS::SetAPS(APS::Channel channel, APS::SetType type, const SetType& value) {
    bool worked = false;
    AuxSupply::Channel c = static_cast<AuxSupply::Channel>(channel);
    switch(type) {
        case APS::CURRENT:
            worked = auxSupply_->FindSetRange(c, auxSupply_->GetVolts(c), value);
            break;
        default: // APS::VOLTS:
            worked = auxSupply_->FindSetRange(c, value, auxSupply_->GetAmps(c));
    };
    // worked is true iff we found a range to support 'value'
    Assert<InstrumentError>(worked, name_);
}

//====================
// SetAPS() Overload2
//====================
void SPTS::SetAPS(APS::Channel channel, APS::StandardVoltValue v) {
    SetAPS(channel, APS::VOLTS, static_cast<SetType>(v)); // call overload
}

//====================
// SetAPS() Overload3
//====================
void SPTS::SetAPS(APS::Channel channel, APS::SetType type, APS::Max) {
    AuxSupplyTraits::Channels c = static_cast<AuxSupplyTraits::Channels>(channel);
    if ( type == APS::VOLTS ) 
        SetAPS(channel, type, auxSupply_->GetMaxVolts(c));
    else
        SetAPS(channel, type, auxSupply_->GetMaxAmps(c));
}

//====================
// SetAPS() Overload4
//====================
void SPTS::SetAPS(APS::Channel channel, Switch state) {
    AuxSupplyTraits::Channels c = static_cast<AuxSupplyTraits::Channels>(channel);
    if ( state == ON ) 
        Assert<InstrumentError>(auxSupply_->OutputOn(c), name_);
    else
        Assert<InstrumentError>(auxSupply_->OutputOff(c), name_);
}

//==========
// SetDMM()
//==========
void SPTS::SetDMM(const ProgramTypes::SetType& range, SPTSInstrument::DMM::Mode mode) {
    dMM_->SetMode(mode);
    dMM_->SetRange(range);
}

//===============
// SetIinLimit()
//===============
void SPTS::SetIinLimit(const SetType& limit) {
    mainSupply_->SetCurrent(limit);
}

//==================
// SetIinLimitMax()
//==================
void SPTS::SetIinLimitMax() {
    InstrumentFile* ifile = SingletonType<InstrumentFile>::Instance();
    mainSupply_->SetCurrent(ifile->MaxAmps(psIsolation_));
}

//=========================
// SetIinLimitProtection()
//=========================
void SPTS::SetIinLimitProtection(Switch state) {
    mainSupply_->SetCurrentProtection(state);
}

//===============
// SetIinShunt()
//===============
bool SPTS::SetIinShunt(StationFile::IinShunt newShunt) {
    // Warning SafeInhibit() is not used here --> set before
    //   calling this routine when desired.

    if ( newShunt == iinShunt_ ) // already there 
        return(false);
    
    typedef ControlMatrixTraits::RelayTypes CR;
    CR::InputRelay a = CR::IINDCA, b = CR::IINDCB;
    ControlMatrix::InputRelayControl::RelayContainer rc;
 
    switch(newShunt) {
        case StationFile::BIGOHM:
            rc.push_back(a);
            rc.push_back(b);
            inputRelays_->TurnOff(rc);            
            break;
        case StationFile::MIDOHM:
            inputRelays_->TurnOn(b);
            inputRelays_->TurnOff(a);                                    
            break;
        default: // StationFile::SMALLOHM
            rc.push_back(a);
            rc.push_back(b);
            inputRelays_->TurnOn(rc);
    };    
    iinShunt_ = newShunt;
    return(true);
}

//=====================
// SetLoad() Overload1
//=====================
void SPTS::SetLoad(LoadTraits::Channels chan, const SetType& loadValue) {
    static const SetType zero = 0;
	LoadChannels::const_iterator found;
    found = std::find(activeLoadChannels_.begin(), activeLoadChannels_.end(), chan);
    Assert<BadArg>(found != activeLoadChannels_.end(), name_);
    Assert<BadArg>(loadValue >= zero, name_);

    // If load value change is greater than DUT's half load, step in iterations
    //   toward final value; only applicable to CC mode and from lighter load
    //   to heavier load
    // If loadValue is 0 and current load value > 10%, then step to 10% load
    //   before going to a no load condition
    if ( load_->GetMode(chan) == LoadTraits::CC ) {        
        std::pair<Switch, SetType> currentValue = load_->GetLoadValue(chan);
        ConverterOutput::Output output = Convert2ConverterOutput(chan);
        SetType halfLoad = dut_->Iout(output) / SetType(2);
        SetType tenPercent = dut_->Iout(output) / SetType(10);
        SetType margin = 0.003; // to prevent roundoff errors == infinite recursion
        if ( (loadValue - currentValue.second) > halfLoad + margin )
            SetLoad(chan, loadValue - halfLoad); // recurse
        else if ( (currentValue.first == OFF) && (loadValue > halfLoad) )
            SetLoad(chan, halfLoad);  // recurse
        else if ( (loadValue == zero) && (currentValue.second > tenPercent) ) {
            if ( currentValue.first == ON )
                SetLoad(chan, tenPercent); // recurse
        }
    }
    load_->SetLoadValue(chan, loadValue);
}

//=====================
// SetLoad() Overload2
//=====================
void SPTS::SetLoad(const SetTypeContainer& loadValues) {
	SetTypeContainer::const_iterator start = loadValues.begin();
	SetTypeContainer::const_iterator stop  = loadValues.end();
	LoadChannels::const_iterator current   = activeLoadChannels_.begin();

    // Assert input argument is correct size and contains good values
	Assert<BadArg>(loadValues.size() == activeLoadChannels_.size(), name_);
	Assert<BadArg>(std::find_if(
                   start, 
		           stop, 
		           std::bind2nd(std::less<SetType>(), static_cast<SetType>(0))) == stop,
                   name_); 
    start = loadValues.begin();
    

    // If any load value change is greater than DUT's half load for that output, 
    //   step in iterations toward final values; only applicable to CC mode and from
    //   lighter load to heavier load
    // If going to no load on some channel, X, and the current value is > 10%, then
    //   step to 10% before going to zero
    if ( load_->GetMode(*current) == LoadTraits::CC ) {
        static const SetType zero = 0;
        SetTypeContainer iouts, tenPercents;
        MTypeContainer toCopy = dut_->Iouts();
        MTypeContainer::iterator i = toCopy.begin(), j = toCopy.end();
        while ( i != j ) {
            iouts.push_back(i->Value());
            ++i;
        }
        std::transform(iouts.begin(), iouts.end(), std::back_inserter(tenPercents),
                       std::bind2nd(std::divides<SetType>(), SetType(10)));
        std::transform(iouts.begin(), iouts.end(), iouts.begin(),     
                       std::bind2nd(std::divides<SetType>(), SetType(2)));
        start = loadValues.begin();
        SetTypeContainer::iterator halfLoad = iouts.begin(), 
                                   tenPercent = tenPercents.begin();

        bool recurse = false;
        SetTypeContainer recurseContainer;
        SetType margin = 0.003; // to prevent roundoff errors == infinite recursion
        while ( start != stop ) {            
            std::pair<Switch, SetType> currentValue = load_->GetLoadValue(*current);
            if ( (*start) - currentValue.second > (*halfLoad + margin) ) {
                recurseContainer.push_back((*start) - (*halfLoad));
                recurse = true;
            }
            else if ( (currentValue.first == OFF) && (*start > *halfLoad) ) {
                recurseContainer.push_back(*halfLoad);
                recurse = true;
            }
            else if ( (*start == zero) && (currentValue.first == ON) &&
                      (currentValue.second > *tenPercent) ) {
                recurseContainer.push_back(*tenPercent);
                recurse = true;
            }
            else
                recurseContainer.push_back(*start);
            ++current; ++start; ++halfLoad; ++tenPercent;
        }
        start = loadValues.begin();
        current = activeLoadChannels_.begin();
 
        if ( recurse ) // then recurse
            SetLoad(recurseContainer);
    } // if


    // Concatenate commands to the (load_)
    try {
	    load_->Concatenate(ON);
	    while ( start != stop ) {
		    load_->SetLoadValue(*current, *start); 
		    ++current;
		    ++start; 
	    } // while
        load_->Concatenate(OFF);
    } catch(...) {
	    load_->ImmediateMode();
        throw;
    }    
}

//=====================
// SetLoad() Overload3
//=====================
void SPTS::SetLoad(LoadTraits::Channels chan, Switch state) {       
	LoadChannels::iterator found;
    found = std::find(activeLoadChannels_.begin(), activeLoadChannels_.end(), chan);
    Assert<BadArg>(found != activeLoadChannels_.end(), name_);
    if ( state == ON ) {
        // If turing ON into CC mode, then call SetLoad() overload --> takes care
        //   of possibility of turning on into something >= Full Load
        if ( load_->GetMode(chan) == LoadTraits::CC )
            SetLoad(chan, load_->GetLoadValue(chan).second);
        else // CR mode
            load_->On(chan);
    }
    else // state == OFF
        load_->Off(chan);
}

//=====================
// SetLoad() Overload4
//=====================
void SPTS::SetLoad(LoadTraits::AllChannels, Switch state) {

    // If turning ON into CC mode, then call SetLoad() overload which takes care of
    //   possibility of turning on into something >= full load on any one or more 
    //   outputs.
    if ( state == ON ) {
        LoadChannels::iterator start = activeLoadChannels_.begin();
        if ( load_->GetMode(*start) == LoadTraits::CC ) {                
            SetTypeContainer currentValues;
            while ( start != activeLoadChannels_.end() ) {
                currentValues.push_back(load_->GetLoadValue(*start).second);
                ++start;
            }
            SetLoad(currentValues); // call overload
            return; 
        }
    }

    // recurse N times for all N outputs
    try {
        LoadChannels::const_iterator start = activeLoadChannels_.begin();
        LoadChannels::const_iterator stop  = activeLoadChannels_.end();
        load_->Concatenate(ON);
        while ( start != stop ) {
	        SetLoad(*start, state); // call overload
            ++start;
        }
        load_->Concatenate(OFF);
        return;
    } catch(...) {
        load_->ImmediateMode();
        throw;    
    }
}

//====================
// setLoadAllExcept()
//====================
void SPTS::setLoadAllExcept(Switch state, const std::vector<LoadTraits::Channels>& c) {
    // Set all load channels to (state) except for any in (c) 
 
    // Don't use concatenation in this routine --> will override safety setups inside
    //   of SetLoad()where we step to a pre-final load value if step value is too large
    //   in order to minimize load transient effects.   

    try {
        LoadChannels::const_iterator start = activeLoadChannels_.begin();
        LoadChannels::const_iterator stop  = activeLoadChannels_.end();
        while ( start != stop ) {
            if ( std::find(c.begin(), c.end(), *start) == c.end() ) // set (state)
	            SetLoad(*start, state);
            ++start;
        }
        return;
    } catch(...) {
        load_->ImmediateMode();
        throw;
    }
}

//===============
// setLoadMode() 
//===============
void SPTS::setLoadMode(LoadTraits::Channels chan, LoadTraits::Modes mode) {
    // Warning: It is best to have the load_ Concatenate(ON) set when
    //           using this function --> set load_->Concatenate(ON) prior
    //           to calling and load_->Concatenate(OFF) when done.

    // chan is a specific load channel
    LoadChannels::iterator found;
    found = std::find(activeLoadChannels_.begin(), activeLoadChannels_.end(), chan);
    Assert<BadArg>(found != activeLoadChannels_.end(), name_);
    if ( mode == load_->GetMode(*found) ) // already there
        return;
    else if ( load_->LoadType() == LoadTraits::PASSIVE ) // cannot change mode
        return;
   
    // Set new mode for chan : works for CC->CR and CR->CC
    std::pair<Switch, ProgramTypes::SetType> iout = load_->GetLoadValue(chan);
    ProgramTypes::MTypeContainer vouts;
    bool useDMM = false;
    MeasureVoutDC(vouts, Convert2ConverterOutput(chan), useDMM);
    Assert<UnexpectedState>(!vouts.empty(), name_);

    if ( mode == LoadTraits::CR ) { // CC->CR
        ProgramTypes::SetType max = load_->MinMaxOhms(chan).second;
        try {
            if ( iout.second == static_cast<ProgramTypes::SetType>(0) )
                throw(OutOfRange());
            ProgramTypes::SetType val;
            val = absolute(static_cast<ProgramTypes::SetType>(vouts[0]) / iout.second);
            Assert<OutOfRange>(val <= max);
            iout.second = val;
        } catch(OutOfRange&) {
            iout.second = max; // max ohms       
        }
    }
    else { // CR->CC
        ProgramTypes::MType minVolts = 0.2;
        try {
            iout.second = 
                 absolute(static_cast<ProgramTypes::SetType>(vouts[0]) / iout.second);
            Assert<OutOfRange>(iout.second <= load_->MaxCurrent(chan), name_);
        } catch(OutOfRange& oor) {
            VariablesFile* vf = SingletonType<VariablesFile>::Instance();
            if ( absolute(vouts[0]) >= minVolts || vf->DoDUTSanityChecks() )
                throw(oor);
            iout.second = 0; // iff skip sanity checks is true and vout is ~0V
                             //  likely only applicable to "configurable" units which
                             //  have some outputs OFF due to inhibit(s).
        } catch(...) {
            VariablesFile* vf = SingletonType<VariablesFile>::Instance();
            Assert<DivideByZero>(absolute(vouts[0]) < minVolts &&
                                 !vf->DoDUTSanityChecks(), name_);
            iout.second = 0; // iff skip sanity checks is true and vout is ~0V
                             //  likely only applicable to "configurable" units which
                             //  have some outputs OFF due to inhibit(s).
        }
    }

    load_->SetMode(chan, mode);
    load_->SetLoadValue(chan, iout.second);
    if ( iout.first == OFF ) // Load was off
        load_->Off(chan);
}

//================
// SetLoadModes() 
//================
void SPTS::SetLoadModes(LoadTraits::Modes mode) {
    // Switch all load modes
    LoadChannels::iterator i = activeLoadChannels_.begin();
    try {
        load_->Concatenate(ON);
        while ( i != activeLoadChannels_.end() ) {
            setLoadMode(*i, mode);
            ++i;
        } // while
    } catch(...) {
        load_->ImmediateMode();
        throw;
    }
    load_->Concatenate(OFF);
}

//====================
// SetLoadTransient()
//====================
void SPTS::SetLoadTransient(LoadTraits::Channels chan, const SetType& fromVal, 
                            const SetType& toVal, const SetType& slew) {
    slew > 0 ?
        load_->SetTransient(chan, fromVal, toVal, slew) :
        load_->SetTransient(chan, fromVal, toVal);
}

//=====================
// SetPath() Overload1
//=====================
void SPTS::SetPath(ACPathTypes::ImplicitPaths impPath, ConverterOutput::Output output, 
                                                       FilterSelects::FilterType bw) {
    Assert<BadArg>(output != ConverterOutput::ALL, name_);
    setPath(impPath, output, bw); // Call overloaded version
}

//=====================
// SetPath() Overload2
//=====================
void SPTS::SetPath(ACPathTypes::ExplicitPaths exPath, FilterSelects::FilterType bw) {

    // Local typedefs and variable declarations
	typedef SwitchMatrixTraits::RelayTypes  RFPath;
	typedef ControlMatrixTraits::RelayTypes InputPath;
	typedef ControlMatrixTraits::RelayTypes OutputPath;
	SwitchMatrix::RFRelayContainer  acRelays;
	SwitchMatrix::RFFilterContainer filtRelays;
	std::pair<SwitchMatrixTraits::RelayTypes::FilterRelay,
              SwitchMatrixTraits::RelayTypes::FilterRelay> filterPair;
	ControlMatrix::InputRelayControl::RelayContainer  inputBoxRelays;
	ControlMatrix::OutputRelayControl::RelayContainer outputBoxRelays;

    // Set or reset the explicit path
	switch(exPath) {
		case IINPARD:
            inputBoxRelays.push_back(InputPath::IINPARD);
            acRelays.push_back(RFPath::IINPARD);
            filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
            break;
		case LOADTRANS1: 
            acRelays.push_back(RFPath::LOADTRANSIENT1);
            outputBoxRelays.push_back(OutputPath::LOADTRANSIENT1);
            break;
        case LOADTRANS2:
            acRelays.push_back(RFPath::LOADTRANSIENT2);
            outputBoxRelays.push_back(OutputPath::LOADTRANSIENT2);
            break;
        case LOADTRANS3:
            acRelays.push_back(RFPath::LOADTRANSIENT3);
            outputBoxRelays.push_back(OutputPath::LOADTRANSIENT3);
            break;
		case LOADTRANS4:
            acRelays.push_back(RFPath::LOADTRANSIENT4);
            acRelays.push_back(RFPath::EXTENDEDTRANSIENT);
            outputBoxRelays.push_back(OutputPath::LOADTRANSIENT4);
            break;
		case LOADTRANS5:
			acRelays.push_back(RFPath::LOADTRANSIENT5);
            acRelays.push_back(RFPath::EXTENDEDTRANSIENT);
			outputBoxRelays.push_back(OutputPath::LOADTRANSIENT5);
			break;
        case LOADTRIGGER:
            acRelays.push_back(RFPath::LOADTRIGGER);
            outputBoxRelays.push_back(OutputPath::LOADTRIGGER);
            break;
		case PRIMARYINHIBIT:   
			acRelays.push_back(RFPath::PRIMARYINHIBITRISE);
			inputBoxRelays.push_back(InputPath::PRIMARYINHIBITRISE);
			break;
		case SYNCCHECK:  
			acRelays.push_back(RFPath::SYNCCHECK);
			inputBoxRelays.push_back(InputPath::SYNCCHECK);
			break;
		case SYNCIN:
            inputBoxRelays.push_back(InputPath::SYNCIN);
            inputBoxRelays.push_back(InputPath::SYNCENABLE);
            break;
		case SYNCOUT:                   
			acRelays.push_back(RFPath::SYNCOUT);
			break;
		case VINRISE:    
			acRelays.push_back(RFPath::VINRISE);
			inputBoxRelays.push_back(InputPath::VINRISE);
			break;
		case VOUTPARD1:
			acRelays.push_back(RFPath::VOUTPARD1);
			filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
			outputBoxRelays.push_back(OutputPath::VOUTPARD1);
			break;
		case VOUTPARD2:
			acRelays.push_back(RFPath::VOUTPARD2);
			filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
			outputBoxRelays.push_back(OutputPath::VOUTPARD2);
			break;
		case VOUTPARD3:
			acRelays.push_back(RFPath::VOUTPARD3);
			acRelays.push_back(RFPath::EXTENDEDPARD);
			filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
			outputBoxRelays.push_back(OutputPath::VOUTPARD3);
			break;
		case VOUTPARD4:
			acRelays.push_back(RFPath::VOUTPARD4);
			acRelays.push_back(RFPath::EXTENDEDPARD);
			filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
			outputBoxRelays.push_back(OutputPath::VOUTPARD4);
			break;
		case VOUTPARD5:
			acRelays.push_back(RFPath::VOUTPARD5);
			acRelays.push_back(RFPath::EXTENDEDPARD);
			filterPair = selectFilterPath(bw);
			filtRelays.push_back(filterPair.first);
			filtRelays.push_back(filterPair.second);
			outputBoxRelays.push_back(OutputPath::VOUTPARD5);
			break;
		default:
			throw(BadArg(name_));
	}; // Outer switch
	
	// Set or Reset System Matrix
    bool reset1 = false, reset2 = false, needReset = false;
	if ( pathOpen_ ) { // pathOpen_ set from inside ResetPath()
        if ( ! acRelays.empty() )
		    reset1 = switchMatrix_->Open(acRelays);   
        if ( ! filtRelays.empty() ) 
		    reset2 = switchMatrix_->Open(filtRelays);
	}
	else {
        if ( ! acRelays.empty() )
		    reset1 = switchMatrix_->Close(acRelays);   
        if ( ! filtRelays.empty() ) 
		    reset2 = switchMatrix_->Close(filtRelays);
	} // if-else
    if ( reset1 || reset2 )
        needReset = true;

	// Set or Reset SIM and fixture relays
    reset1 = false, reset2 = false;
	if ( pathOpen_ ) { // pathOpen_ set from inside ResetPath()
        if ( ! inputBoxRelays.empty() )
		    reset1 = inputRelays_->TurnOff(inputBoxRelays);
        if ( ! outputBoxRelays.empty() )
		    reset2 = outputRelays_->TurnOff(outputBoxRelays);
	}
	else {
        if ( ! inputBoxRelays.empty() )
            reset1 = inputRelays_->TurnOn(inputBoxRelays);
        if ( ! outputBoxRelays.empty() )
            reset2 = outputRelays_->TurnOn(outputBoxRelays);    
	} // if-else
    if ( reset1 || reset2 )
        needReset = true;

    if ( needReset )
        setPathPause();
}

//=====================
// SetPath() Overload3
//=====================
void SPTS::SetPath(ControlMatrixTraits::RelayTypes::MiscRelay relay) {
    Assert<UnexpectedState>(miscLines_.get() != 0, name_);
    bool reset = false;
    if ( pathOpen_ )
        reset = miscLines_->TurnOff(relay);       
    else
        reset = miscLines_->TurnOn(relay);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload4
//=====================
void SPTS::SetPath(ControlMatrixTraits::RelayTypes::InputRelay relay) {
    bool reset = false;
    if ( pathOpen_ )
        reset = inputRelays_->TurnOff(relay);
    else
        reset = inputRelays_->TurnOn(relay);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload5
//=====================
void SPTS::SetPath(ControlMatrixTraits::RelayTypes::OutputRelay relay) {
    bool reset = false;
    if ( pathOpen_ )
        reset = outputRelays_->TurnOff(relay);
    else
        reset = outputRelays_->TurnOn(relay);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload6
//=====================
void SPTS::SetPath(const std::vector<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                                                          relays) {
    if ( relays.empty() )
        return;
    Assert<UnexpectedState>(miscLines_.get() != 0, name_);
    ControlMatrix::Misc::MiscContainer r;
    std::copy(relays.begin(), relays.end(), std::back_inserter(r));
    bool reset = false;
    if ( pathOpen_ )
        reset = miscLines_->TurnOff(r);
    else
        reset = miscLines_->TurnOn(r);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload7
//=====================
void SPTS::SetPath(const std::vector<ControlMatrixTraits::RelayTypes::InputRelay> &relays)
{
    ControlMatrix::InputRelayControl::RelayContainer r;
    std::copy(relays.begin(), relays.end(), std::back_inserter(r));
    bool reset = false;
    if ( pathOpen_ )
        reset = inputRelays_->TurnOff(r);
    else
        reset = inputRelays_->TurnOn(r);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload8
//=====================
void SPTS::SetPath(const std::vector<ControlMatrixTraits::RelayTypes::OutputRelay>& 
                                                                            relays) {
    ControlMatrix::OutputRelayControl::RelayContainer r;
    std::copy(relays.begin(), relays.end(), std::back_inserter(r));
    bool reset = false;
    if ( pathOpen_ )
        reset = outputRelays_->TurnOff(r);
    else
        reset = outputRelays_->TurnOn(r);
    if ( reset )
        setPathPause();
}

//=====================
// SetPath() Overload9
//=====================
void SPTS::SetPath(SwitchMatrixTraits::RelayTypes::DCRelay relay) {
    bool reset = false;
    if ( pathOpen_ )
        reset = switchMatrix_->Open(relay);       
    else
        reset = switchMatrix_->Close(relay);
    if ( reset ) 
        setPathPause();
}

//======================
// SetPath() Overload10
//======================
void SPTS::SetPath(SwitchMatrixTraits::RelayTypes::RFRelay relay) {
    bool reset = false;
    if ( pathOpen_ )
        reset = switchMatrix_->Open(relay);       
    else
        reset = switchMatrix_->Close(relay);
    if ( reset ) 
        setPathPause();
}

//======================
// SetPath() Overload11
//======================
void SPTS::SetPath(SwitchMatrixTraits::RelayTypes::FilterRelay relay) {
    bool reset = false;
    if ( pathOpen_ )
        reset = switchMatrix_->Open(relay);       
    else
        reset = switchMatrix_->Close(relay);
    if ( reset ) 
        setPathPause();
}

//===========
// setPath() 
//===========
void SPTS::setPath(ACPathTypes::ImplicitPaths impPath, ConverterOutput::Output chan, 
                                                       FilterSelects::FilterType bw) {    
    // Call overloaded SetPath()
	SetPath(Convert2ExplicitPath(impPath, chan), bw); 
}

//================
// setPathPause()
//================
void SPTS::setPathPause() {    
    if ( !noReset_ ) {
        PauseStates* ps = SingletonType<PauseStates>::Instance();
        Pause(ps->GetPauseValue(PauseStates::RELAYSTATECHANGE));
    }
}

//======================
// SetScope() Overload1
//======================
void SPTS::SetScope(OScopeChannels::Channel chan, Switch state) {
    if ( chan == OScopeChannels::ALL ) {
        SetScope(OScopeChannels::ONE, state);
        SetScope(OScopeChannels::TWO, state);
        SetScope(OScopeChannels::THREE, state);
        SetScope(OScopeChannels::TRIGGER, state);
        return;
    }
    if ( state == ON ) 
        scope_->ChannelOn(chan);
    else
        scope_->ChannelOff(chan);
}

//======================
// SetScope() Overload2
//======================
std::set<OScopeSetupFile::Parameters> 
    SPTS::SetScope(const std::string& testName, OScopeChannels::Channel chan) {

    Assert<BadArg>(chan != OScopeChannels::ALL, name_);
    typedef OScopeSetupFile OSSF;
    typedef OScopeParameters OSP;
    typedef OScopeChannels::Channel OSC;

    // Set up oscilloscope for upcoming testName measurement
    std::set<OSSF::Parameters> toRtn;
    try {
        // Grab info from OScopeSetupFile
        OSSF* ssf = SingletonType<OSSF>::Instance();
        ssf->SetTestName(testName);
        scope_->Concatenate(ON);      

        // try-catch series to setup oscilloscope per OScopeSetupFile.  If any 
        //  parameter is "undefined", then record that setup parameter in the 
        //  returned value.  Non-Trigger Commands should precede Trigger Commands
        //  (e.g.; TRIGLEVEL depends on VERTSCALE sometimes, etc.)

        // Non-Trigger Commands
        try { // BANDWIDTH
            scope_->SetBandwidth(chan, ssf->GetValue<Switch, OSSF::BANDWIDTH>());
        } catch(Undefined) { toRtn.insert(OSSF::BANDWIDTH); }
        try { // COUPLING
            scope_->SetCoupling(chan, ssf->GetValue<OSP::CouplingType, 
                                                    OSSF::COUPLING>());
        } catch(Undefined) { toRtn.insert(OSSF::COUPLING); }
        try { // HORMEMORYSETTING
            scope_->SetHorizontalMemory(ssf->GetValue<OSP::MemorySetting, 
                                                      OSSF::HORMEMORYSETTING>());
        } catch(Undefined) { toRtn.insert(OSSF::HORMEMORYSETTING); }
        try { // HORSCALE
            scope_->SetHorizontalScale(ssf->GetValue<ProgramTypes::SetType, 
                                                     OSSF::HORSCALE>());
        } catch(Undefined) { toRtn.insert(OSSF::HORSCALE); }
        try { // VERTSCALE
            scope_->SetVerticalScale(chan, ssf->GetValue<ProgramTypes::SetType, 
                                                         OSSF::VERTSCALE>());            
        } catch(Undefined) { toRtn.insert(OSSF::VERTSCALE); }
        try { // OFFSET - must follow VERTSCALE
            scope_->SetOffset(chan, ssf->GetValue<ProgramTypes::SetType, 
                                                  OSSF::OFFSET>());
        } catch(Undefined) { toRtn.insert(OSSF::OFFSET); }


        // Trigger Commands - TRIGSOURCE must precede all other trigger commands
        // Also, TRIGLEVEL should follow TRIGVERTSCALE when TRIGVERTSCALE applies.
        OScopeChannels::Channel trigChan;
        try { // TRIGSOURCE
            trigChan = ssf->GetValue<OSC, OSSF::TRIGSOURCE>();
            scope_->SetTriggerSource(trigChan);
        } catch(Undefined) { toRtn.insert(OSSF::TRIGSOURCE); trigChan = chan; }
        try { // TRIGCHANCOUPLING
            // Since trigChan == chan is possible, a special case for trig chan
            //  coupling exists.  Not allowed to be undefined otherwise.
            if ( trigChan != chan ) {
                scope_->SetCoupling(trigChan, ssf->GetValue<OSP::CouplingType,
                                                          OSSF::TRIGCHANCOUPLING>());
            }
        } catch(Undefined) { 
            std::string err = "Must define trig chan coupling externally unless trig";
            err += " chan is the same as measurement channel";
            Assert<FileError>(trigChan == chan, name_ + ":\n" + err);
        }
        try { // TRIGCOUPLING
            scope_->SetTriggerCoupling(trigChan, 
                                       ssf->GetValue<OSSF::TriggerCouplingType,
                                                     OSSF::TRIGCOUPLING>());
        } catch(Undefined) { toRtn.insert(OSSF::TRIGCOUPLING); }
        try { // TRIGMODE
            scope_->SetTriggerMode(ssf->GetValue<OSSF::TriggerMode, 
                                                 OSSF::TRIGMODE>());
        } catch(Undefined) { toRtn.insert(OSSF::TRIGMODE); }
        try { // TRIGSLOPE
            OSSF::SlopeType s = ssf->GetValue<OSSF::SlopeType, OSSF::TRIGSLOPE>();
            scope_->SetTriggerSlope(trigChan, s);
        } catch(Undefined) { toRtn.insert(OSSF::TRIGSLOPE); }
        try { // TRIGVERTSCALE
            // Since trigChan == chan is possible, a special case for trig vertical
            //  scaling exists.  Not allowed to be undefined otherwise.
            if ( trigChan != chan ) {
                ProgramTypes::SetType vscale = ssf->GetValue<ProgramTypes::SetType,
                                                             OSSF::TRIGVERTSCALE>();
                scope_->SetVerticalScale(trigChan, vscale);
            }
        } catch(Undefined) {
            std::string err = "Must define trig chan vert scale externally unless trig";
            err += " chan is the same as measurement channel";
            Assert<FileError>(trigChan == chan, name_ + ":\n" + err);
        }
        try { // Trigger Offset
            // Since trigChan == chan is possible, a special case for trig offset
            //  exists.  Not configurable externally.
            if ( trigChan != chan ) // set to center of screen by default
                scope_->SetOffset(trigChan, 0);
        } catch(Undefined) {
            std::string err = "Must define trig chan vert scale externally unless trig";
            err += " chan is the same as measurement channel";
            Assert<FileError>(trigChan == chan, name_ + ":\n" + err);
        }
        try { // TRIGLEVEL
            scope_->SetTriggerLevel(trigChan, ssf->GetValue<ProgramTypes::SetType,
                                                                OSSF::TRIGLEVEL>());
        } catch(Undefined) { toRtn.insert(OSSF::TRIGLEVEL); }

    } catch(...) {
        scope_->ImmediateMode(); 
        throw;
    }
    scope_->Concatenate(OFF);
    return(toRtn);
}

//==============================
// SetScopeExplicit() Overload1
//==============================
void SPTS::SetScopeExplicit(OScopeChannels::Channel c, 
                            ExplicitScope::ExplicitParms parm) {

    Assert<BadArg>(c != OScopeChannels::ALL, name_);
    switch(parm) {
        case ExplicitScope::TRIGSOURCE:
            scope_->SetTriggerSource(c);
            break;
        default:
            throw(BadArg(name_));
    };
}

//==============================
// SetScopeExplicit() Overload2
//==============================
void SPTS::SetScopeExplicit(OScopeChannels::Channel chan, 
                            ExplicitScope::ExplicitParms parm, 
                            const SetType& value) {

    Assert<BadArg>(chan != OScopeChannels::ALL, name_);
    switch(parm) {
        case ExplicitScope::VERTSCALE:
            // If value is out of range, then use range's end value
            if ( value < scope_->GetVertRange(chan).first ) {
                scope_->SetVerticalScale(chan, scope_->GetVertRange(chan).first);
                return;
            }
            else if ( value > scope_->GetVertRange(chan).second ) {
                scope_->SetVerticalScale(chan, scope_->GetVertRange(chan).second);
                return;
            }
            scope_->SetVerticalScale(chan, value);
            break;
        case ExplicitScope::OFFSET:
            scope_->SetOffset(chan, value);
            break;
        default:
            throw(StationExceptionTypes::BadArg(Name()));
    };
}

//==============================
// SetScopeExplicit() Overload3
//==============================
void SPTS::SetScopeExplicit(ExplicitScope::ExplicitParms p, const SetType& value) {
    switch(p) {
        case ExplicitScope::HORZSCALE:
            scope_->SetHorizontalScale(value);
            break;
        default:
            throw(StationExceptionTypes::BadArg(Name()));
    };
}

//====================
// SetScopeExplicit()
//====================
void SPTS::SetScopeExplicit(OScopeParameters::TriggerMode tm) {
    scope_->SetTriggerMode(tm);
}

//===========
// SetSync()
//===========
void SPTS::SetSync(const ProgramTypes::SetType& freq, const ProgramTypes::SetType& ampl,
                   const ProgramTypes::PercentType& dc, 
                   const ProgramTypes::SetType& offset) {
    // Inhibiting DUT On/Off must be done outside of this routine
    ControlMatrix::InputRelayControl::RelayContainer r;
    r.push_back(ControlMatrixTraits::RelayTypes::SYNCENABLE);
    r.push_back(ControlMatrixTraits::RelayTypes::SYNCIN);
    SetPath(r);
    funcGen_->SetAmplitude(ampl);
    funcGen_->SetDutyCycle(dc);
    funcGen_->SetOffset(offset);
    funcGen_->SetFrequency(freq);
}

//======================
// SetTemperatureBase()
//======================
void SPTS::SetTemperatureBase(const SetType& value) {
    tempControl_->SetTemperature(value);
}

//============================
// SetTemperatureBaseLimits()
//============================
void SPTS::setTemperatureBaseLimits() {
    InstrumentFile* ifile = SingletonType<InstrumentFile>::Instance();
    SetType min = ifile->MinimumTemperature();
    SetType max = ifile->MaximumTemperature();    
    tempControl_->SetTemperatureLimits(min, max);
}

//==========
// SetVin() 
//==========
void SPTS::SetVin(const SetType& vinValue, bool canCheckWithDMM) {
    /*
       If canCheckDMM is true, then we are given authority here to measure Vin via a
        relay change + DMM.  We may instead choose to use the P/S's voltage measurement
        value, which is dependent upon mainSupply_'s CanTrustVoltsMeasure() value.
       If canCheckDMM is false, then we are not allowed to change relay states here.
    */

    static SetType zero = static_cast<SetType>(0);

    // Check vinValue
    Assert<BadArg>(vinValue >= zero, name_);

    ProgramTypes::SetType current = mainSupply_->GetVolts();    
    if ( vinValue == zero ) { // Turn supply off if vinValue == 0
        Assert<InstrumentError>(mainSupply_->OutputOff(), name_);
        Assert<InstrumentError>(mainSupply_->SetVolts(vinValue), name_);
    }
    else {
        poweredDown_ = false;

        // If vinValue <= Low Line, then step to vinValue in increments.
        //   This is to minimize undershoot which can be significant on
        //   High Voltage Power Supplies.
        if ( vinValue < current && (vinValue <= dut_->LowLine()) &&
                                        (current > dut_->LowLine()) ) {
            ProgramTypes::SetType delta = current - vinValue;
            ProgramTypes::SetType maxDelta = (0.05 * dut_->HighLine()); // Arbitrary #
            if ( delta > maxDelta ) {
                ProgramTypes::SetType nextVolts = vinValue + maxDelta;
                SetVin(nextVolts, canCheckWithDMM);
                bool noDMMCheck = false; // Only small iterations remain
                SetVin(nextVolts -= (maxDelta.Value() / 2), noDMMCheck);
                SetVin(nextVolts -= (maxDelta.Value() / 4), noDMMCheck); 
                SetVin(nextVolts -= (maxDelta.Value() / 8), noDMMCheck);
            }
        }

        // Set voltage.  If supply is off, then turn on.
        Assert<InstrumentError>(mainSupply_->SetVolts(vinValue), name_);
        if ( !mainSupply_->IsOn() )
            Assert<InstrumentError>(mainSupply_->OutputOn(), name_);
        else if ( vinValue == current )
            return; // already there
    }

    // Ensure Vin is where we want it to be before leaving this routine
    //  This allows the station's algorithms to be less dependent upon 
    //  the speed of any power supply going from X volts to Y volts.
    bool canTrustSupply = mainSupply_->CanTrustVoltsMeasure() || !canCheckWithDMM;
    long counter = -1, maxCounter = 50;
    MType voltValue = 0;
    const MType n = mainSupply_->GetAccuracy();
    MType m = n + MType(1);
    while ( m > n ) {
        MType voltValue = canTrustSupply ?
                             mainSupply_->MeasureVolts() :
                             MeasureVinDC();
        m = absolute(MType(voltValue.Value() - vinValue.Value()));
        m -= canTrustSupply ? 0 : 10e-3; /* conservative DMM acc. estimate */
        Assert<MainSupplyTimeout>(++counter < maxCounter, Name());
    } // while

    // Grab Pause Value for power supply changes
    static PauseStates* ps = SingletonType<PauseStates>::Instance();
    static SetType pauseValue = ps->GetPauseValue(PauseStates::POWERSUPPLYCHANGE);
    Pause(pauseValue);
}

//==============
// StartScope()
//==============
void SPTS::StartScope() {
    scope_->Start();
}

//=============
// StopScope()
//=============
void SPTS::StopScope() {
    WaitOnScope();
    scope_->Stop();
}

//=================
// StrongInhibit()
//=================
void SPTS::StrongInhibit(Switch type) {
    switch(type) {
        case OFF:           
            inputRelays_->TurnOff(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);
            break;
        case ON:
            inputRelays_->TurnOn(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);            
    };
}

//=======================
// temporaryPreloadDUT()
//=======================
void SPTS::temporaryPreloadDUT() {
    // DUT *must* be inhibited to use this routine; otherwise
    //  concatenation of load below will override safety setups
    //  inside of SetLoad() when we step to a pre-final load value
    //  prior to the final load value to minimize transients

    // Locals
    ProgramTypes::MTypeContainer m = dut_->Iouts();
    ProgramTypes::SetTypeContainer values;
    ProgramTypes::MTypeContainer::iterator a = m.begin();
    while ( a != m.end() ) {
        values.push_back(a->Value());
        ++a;
    }
    Assert<FileError>(values.size() == activeLoadChannels_.size(), name_);
    
    // Store current load states
    typedef std::vector< std::pair< std::pair<Switch, SetType>, LoadTraits::Modes > >
                                                                           LoadStates;

    LoadChannels::iterator i = activeLoadChannels_.begin();
    LoadStates loadStates;
    while ( i != activeLoadChannels_.end() ) {
        std::pair<Switch, SetType> s = load_->GetLoadValue(*i); 
        LoadTraits::Modes m = load_->GetMode(*i);
        loadStates.push_back(std::make_pair(s, m));        
        ++i;
    }

    // Change load mode(s) and value(s)    
    SetLoad(values);
    SetLoadModes(LoadTraits::CC);

    // Pause to let DUT bleed its energy
    typedef SingletonType<PauseStates> PF;
    Pause(PF::Instance()->GetPauseValue(PauseStates::SAFEINHIBITON));

    // Go back to original load configuration(s)
    load_->Concatenate(ON);
    try {
        i = activeLoadChannels_.begin();
        LoadStates::iterator j = loadStates.begin();
        while ( j != loadStates.end() ) {
            load_->SetMode(*i, j->second); // Old mode
            load_->SetLoadValue(*i, j->first.second); // Old load value
            if ( j->first.first == OFF ) // Was off before?
                load_->Off(*i); // Turn off again
            ++i; ++j;
        }
    } catch(...) {
        load_->ImmediateMode();
        throw;
    }
    load_->Concatenate(OFF);
}

//===============
// WaitOnScope()
//===============
void SPTS::WaitOnScope() {
    long cntr = 0, maxLoop = 100; // arbitrary
    while ( ! scope_->OperationComplete() ) // wait
        Assert<OScopeTimeout>(++cntr < maxLoop, name_);

    // complete an aquisition
    ProgramTypes::SetType horz = scope_->GetHorzScale();
    horz *= (scope_->NumberHorizontalDivisions() + 1);
    Pause(horz);
}

//=============
// WhatError()
//=============
std::pair<SPTSInstrument::InstrumentTypes::Types, std::string> SPTS::WhatError() {
    std::string toRtn = whatError_;
    whatError_ = "";
    return(std::make_pair(errorInstr_, toRtn));
}

//===============
// WhichSupply()
//===============
MainSupplyTraits::Supply SPTS::WhichSupply() {
    return(mainSupply_->WhichSupply());
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
