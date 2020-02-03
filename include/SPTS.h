// Macro Guard
#ifndef	SPTS_InstrumentRack_H
#define SPTS_InstrumentRack_H

// Files included
#include "Converter.h"
#include "ConverterOutput.h"
#include "InstrumentTypes.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "StandardFiles.h"
#include "StandardStationFiles.h"
#include "Switch.h"

// Instrument-related files
#include "AuxSupply.h"
#include "AuxSupplyTraits.h"
#include "ControlMatrix.h"
#include "CurrentProbe.h"
#include "DMM.h"
#include "FilterSelects.h"
#include "FunctionGenerator.h"
#include "Load.h"
#include "MainSupply.h"
#include "Oscilloscope.h"
#include "OScopeParameters.h"
#include "SwitchMatrix.h"
#include "TemperatureController.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/14/05, sjn,
   ==============
     Added SetIinLimitProtection(Switch = ON/OFF) for the main power supply.

   ==============
   10/26/05, sjn,
   ==============
     Added GetScopeVertScale().

   ==============
   05/10/05, sjn,
   ==============
     Added ResetAllMiscIO() to public interface.
     Removed CurrentProbeAutoRescale(), SetProbeScale(), rescaler() and
       OScopeAutoRescale().  Removed member variables: powerPath_, scopeRescale_ and
       cpRescale_.  This is in relation to station re-layout and current probe
       instrument change.  Removed enumerations: PowerPath and RescaleType. 
       See SPTS.cpp for more details.

   ==============
   10/05/04, sjn,
   ==============
     Added private member function, newDUTSetup().
     Added #include for Switch.h.
     Made all member function formal parameters start with a lowercase letter.
     Added public member function, GetTemperatureSetpoint().
     Added second parameter (w/default value) to EmergencyShutdown().
     Removed ConflictingPaths() --> not needed with new layout.
     Removed FilterBanks structure --> not neede with new layout.
     Removed FilterBanks::Banks parameter from selectFilterPath().
     Removed OScopeChannels::Channel Convert2ScopeChannel(LoadTraits::Channels);
     Removed LOWIMPEDANCE and HIGHIMPEDANCE from ExplicitScope::ExplicitParms --> not
       applicable to new station layout.
     Added StartScope() and StopScope() to the public interface.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SpacePowerTestStation {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct APS {
	enum Channel {
		PRIMARY   = AuxSupplyTraits::OUTPUT3,
	    SECONDARY = AuxSupplyTraits::OUTPUT4,
		SYSTEM12V = AuxSupplyTraits::OUTPUT2,
		SYSTEM5V  = AuxSupplyTraits::OUTPUT1
	};
    enum StandardVoltValue {
        FIVE = 5,
        TWELVE = 12
    };
    enum SetType {
        CURRENT,
        VOLTS
    };
    enum Max {
        MAX
    };
protected:
	~APS() {}
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ACPathTypes {
	enum ExplicitPaths {
		IINPARD,             
		LOADTRANS1,
		LOADTRANS2,
		LOADTRANS3,
		LOADTRANS4,
		LOADTRANS5,
        LOADTRIGGER,
		PRIMARYINHIBIT,
		SYNCCHECK,    
		SYNCIN,  
		SYNCOUT,
		VINRISE,
		VOUTPARD1,
		VOUTPARD2,
		VOUTPARD3,
		VOUTPARD4,
		VOUTPARD5
	}; // ExplicitPaths

	enum ImplicitPaths {
		LOADTRANSIENT = VOUTPARD5 + 1,	
		SHORTCKTOVERSHOOT,
		STARTUPDELAY,
		STARTUPOVERSHOOT,
		VOUTPARD
	}; // ImplicitPaths

protected:
	~ACPathTypes() {}
};


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ExplicitScope {
    enum ExplicitParms {
        HORZMEMORY,
        HORZSCALE,
        OFFSET,
        TRIGSOURCE,
        VERTSCALE
    };
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/
    
struct SPTS : public FilterSelects, public ACPathTypes, private NoCopy {

    //=================
    // Public Typedefs
    //=================
	typedef ProgramTypes::MType               MType;
	typedef ProgramTypes::SetType             SetType;
	typedef ProgramTypes::MTypeContainer      MTypeContainer;
	typedef ProgramTypes::SetTypeContainer    SetTypeContainer;
    typedef std::vector<LoadTraits::Channels> LoadChannels;

    //========================
    // Start Public Interface
    //========================
    ConverterOutput::Output Convert2ConverterOutput(LoadTraits::Channels fromChannel);
    ACPathTypes::ExplicitPaths Convert2ExplicitPath(ACPathTypes::ImplicitPaths imp, 
                                                    ConverterOutput::Output output);
    LoadTraits::Channels Convert2LoadChannel(ConverterOutput::Output fromChannel);
    void EmergencyShutdown(bool resetTemp = true, const SetType& = 0);
    MType GetCurrentProbeScale();
    std::pair< SwitchMatrixTraits::RelayTypes::DCRelay, 
               std::pair<StationFile::IinDCBoard, StationFile::IinShunt> 
             > GetInputShuntInfo();
    const LoadChannels& GetLoadChannels() const;
    std::vector< std::pair<Switch, SetType> > GetLoadValues() const;
    OScopeChannels::Channel GetScopeChannel(ACPathTypes::ImplicitPaths impPath, 
			                               LoadTraits::Channels chan) const;
    OScopeChannels::Channel GetScopeChannel(ACPathTypes::ExplicitPaths path) const;
    SetType GetScopeVertScale(OScopeChannels::Channel chan) const;
    SetType GetTemperatureSetpoint() const;
    SetType GetVin() const;
    void Initialize(bool resetTemp = true);
    bool IsError();
    void LoadTransientOff();
    void LoadTransientTrigger();
    LoadTraits::Types LoadType();
    MType MeasureAPSVolts(APS::Channel channel);
    MType MeasureBaseTemp();
    MType MeasureDCV();
    MType MeasureDUTTemperature();
    MType MeasureLoadCurrent(LoadTraits::Channels chan);
    MType MeasureLoadVolts(LoadTraits::Channels chan);
    MType MeasureOhms();
    MType MeasureScope(OScopeMeasurements::MeasurementType mType, 
                       OScopeChannels::Channel chan, bool toPause = true, 
                       bool toRescale = true);
    MType MeasureScope(OScopeMeasurements::MeasurementType type, 
                       OScopeChannels::Channel chan1, OScopeChannels::Channel chan2,
                       const ProgramTypes::PlusMinusPercentType& percChan1, 
                       const ProgramTypes::PlusMinusPercentType& percChan2,
                       OScopeParameters::SlopeType slope1,
                       OScopeParameters::SlopeType slope2,
                       bool toPause);
    MType MeasureScope(OScopeMeasurements::MeasurementType type, 
                       OScopeChannels::Channel chan1, OScopeChannels::Channel chan2,
                       const ProgramTypes::SetType& level1,
                       const ProgramTypes::SetType& level2,
                       OScopeParameters::SlopeType slope1,
                       OScopeParameters::SlopeType slope2,
                       bool toPause);
    std::string Name() const;
    bool NeedsDegauss();
    void NewDUTSetup();
    long NumberScopeVertDvns();
    void PowerDown();
    void PowerUp();
    void ProbeDegauss();
    void RescaleScope(OScopeChannels::Channel chan);
    bool Reset(bool resetController = true);
    void ResetAllMiscIO();
    void ResetPath(ACPathTypes::ImplicitPaths impPath, ConverterOutput::Output output, 
                   FilterSelects::FilterType bw);
    void ResetPath(ACPathTypes::ExplicitPaths exPath, 
                   FilterSelects::FilterType bw);    
    void ResetPath(ControlMatrixTraits::RelayTypes::MiscRelay relay);
    void ResetPath(ControlMatrixTraits::RelayTypes::InputRelay relay);
    void ResetPath(ControlMatrixTraits::RelayTypes::OutputRelay relay);
    void ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                                                         relays);
    void ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::InputRelay>& 
                                                                           relays);
    void ResetPath(const std::vector<ControlMatrixTraits::RelayTypes::OutputRelay>& 
                                                                           relays);
    void ResetPath(SwitchMatrixTraits::RelayTypes::DCRelay relay);
    void ResetPath(SwitchMatrixTraits::RelayTypes::FilterRelay relay);
    void ResetPath(SwitchMatrixTraits::RelayTypes::RFRelay relay);
    void ResetPath();
    void ResetSync();
    void SafeInhibit(Switch type);
    void SetAPS(APS::Channel channel, APS::SetType type, const SetType& value);
    void SetAPS(APS::Channel channel, APS::StandardVoltValue v);
    void SetAPS(APS::Channel channel, APS::SetType type, APS::Max max);
    void SetAPS(APS::Channel channel, Switch state);
    void SetDMM(const ProgramTypes::SetType& range = SPTSInstrument::DMM::AUTO,
                SPTSInstrument::DMM::Mode mode = SPTSInstrument::DMM::DCV);
    void SetIinLimit(const SetType& limit);
    void SetIinLimitMax();
    void SetIinLimitProtection(Switch state);
    bool SetIinShunt(StationFile::IinShunt newShunt);
    void SetLoad(LoadTraits::Channels chan, const SetType& loadValue);
    void SetLoad(const SetTypeContainer& loadValues);
    void SetLoad(LoadTraits::Channels chan, Switch state);
    void SetLoad(LoadTraits::AllChannels all, Switch state);
    void SetLoadModes(LoadTraits::Modes mode);
    void SetLoadTransient(LoadTraits::Channels chan, const SetType& fromVal, 
                          const SetType& toVal, 
                          const SetType& slew = -1);
    void SetPath(ACPathTypes::ImplicitPaths impPath, 
                 ConverterOutput::Output output, 
                 FilterSelects::FilterType bw = FilterSelects::PASSTHRU);
    void SetPath(ACPathTypes::ExplicitPaths exPath, 
                 FilterSelects::FilterType bw = FilterSelects::PASSTHRU);
    void SetPath(ControlMatrixTraits::RelayTypes::MiscRelay relay);    
    void SetPath(ControlMatrixTraits::RelayTypes::InputRelay relay);
    void SetPath(ControlMatrixTraits::RelayTypes::OutputRelay relay);
    void SetPath(const std::vector<ControlMatrixTraits::RelayTypes::InputRelay>& 
                                                                          relays);
    void SetPath(const std::vector<ControlMatrixTraits::RelayTypes::OutputRelay>& 
                                                                          relays);
    void SetPath(const std::vector<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                                                         relays);
    void SetPath(SwitchMatrixTraits::RelayTypes::DCRelay relay);
    void SetPath(SwitchMatrixTraits::RelayTypes::RFRelay relay);
    void SetPath(SwitchMatrixTraits::RelayTypes::FilterRelay relay);
    void SetScope(OScopeChannels::Channel chan, Switch state);
    std::set<OScopeSetupFile::Parameters> 
                    SetScope(const std::string& testName, OScopeChannels::Channel chan);
    void SetScopeExplicit(OScopeChannels::Channel c, ExplicitScope::ExplicitParms parm);
    void SetScopeExplicit(OScopeChannels::Channel chan,
                          ExplicitScope::ExplicitParms parm, const SetType& value);
    void SetScopeExplicit(ExplicitScope::ExplicitParms p, const SetType& value);
    void SetScopeExplicit(OScopeParameters::TriggerMode tm);
    void SetSync(const ProgramTypes::SetType& freq, 
                 const ProgramTypes::SetType& ampl, 
                 const ProgramTypes::PercentType& dc, 
                 const ProgramTypes::SetType& offset);
    void SetTemperatureBase(const SetType& value);    
    void SetVin(const SetType& vinValue, bool canCheckWithDMM = true);
    void StartScope();
    void StopScope();
    void StrongInhibit(Switch type);
    void WaitOnScope();
    std::pair<SPTSInstrument::InstrumentTypes::Types, std::string> WhatError();
    MainSupplyTraits::Supply WhichSupply();
    //======================
    // End Public Interface
    //======================

private:
    // Construction and destruction
	friend class SingletonType<SPTS>;
	SPTS(); 
    ~SPTS();    

private:
    // Private Helpers
    void customResets();
    bool dmmMeasurementCounter();
    LoadChannels getLoads(Switch state);
    void measureScopePause();
    void newDUTSetup();
    void partSpecific();
    void resetMemberVariables();    
    std::pair<SwitchMatrixTraits::RelayTypes::FilterRelay, 
              SwitchMatrixTraits::RelayTypes::FilterRelay>
	              selectFilterPath(FilterSelects::FilterType bw);
    void setLoadAllExcept(Switch state, const std::vector<LoadTraits::Channels>& c);
    void setLoadMode(LoadTraits::Channels chan, LoadTraits::Modes mode);
    void setPath(ACPathTypes::ImplicitPaths impPath, 
                 ConverterOutput::Output chan, 
                 FilterSelects::FilterType bw);
    void setPathPause();
    void setTemperatureBaseLimits();
    void temporaryPreloadDUT();

private:
    typedef VariablesFile::MapDut2Load MapDut2Load;
    

private:
	// Instrument Member Variables 
	std::auto_ptr<SPTSInstrument::AuxSupply> auxSupply_;
	std::auto_ptr<SPTSInstrument::CurrentProbe> currentProbe_;
	std::auto_ptr<SPTSInstrument::DMM> dMM_;
	std::auto_ptr<SPTSInstrument::FunctionGenerator> funcGen_;
	std::auto_ptr<SPTSInstrument::ControlMatrix::InputRelayControl> inputRelays_;
	std::auto_ptr<SPTSInstrument::Load> load_;	
	std::auto_ptr<SPTSInstrument::MainSupply> mainSupply_;
	std::auto_ptr<SPTSInstrument::ControlMatrix::Misc> miscLines_;
	std::auto_ptr<SPTSInstrument::ControlMatrix::OutputRelayControl> outputRelays_;
	std::auto_ptr<SPTSInstrument::Oscilloscope> scope_;
	std::auto_ptr<SPTSInstrument::SwitchMatrix> switchMatrix_;
	std::auto_ptr<SPTSInstrument::TemperatureController> tempControl_;

	LoadChannels activeLoadChannels_;
	MapDut2Load dut2Load_; 
	bool pathOpen_; 
	bool setShort_;
	bool locked_;
    bool customReset_;
    bool pSpec_;
    bool poweredDown_;
    bool noReset_;
    bool alwaysReset_;
    StationFile::IinShunt iinShunt_;
    MainSupplyTraits::Supply psIsolation_;
    SetType lastVin_;
    std::string whatError_;
    Converter* dut_;
    SPTSInstrument::InstrumentTypes::Types errorInstr_;
    std::string name_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

} // namespace SpacePowerTestStation

#endif  //  SPTS_InstrumentRack_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
