// Macro Guard
#ifndef SPTS_CONVERTER_H
#define SPTS_CONVERTER_H

// Files included
#include "ControlMatrixTraits.h"
#include "ConverterOutput.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "StandardFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
	05/16/06, mrb
		Added useLoadMeter() callout 
  ==============
  07/16/03, sjn,
  ==============
      Changed GetInfo() to Initialize().
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Converter {
	
    //========================
	// Start Public Interface
    //========================
    std::string DashNumber() const;
    std::string DashNumberNoAlpha() const;
    std::vector<ControlMatrixTraits::RelayTypes::InputRelay> EnabledInputRelays();
    std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> EnabledMiscRelays();
    std::vector<ControlMatrixTraits::RelayTypes::OutputRelay> EnabledOutputRelays();
    std::string FamilyNumber() const;
    bool  FrequencySplitter() const;
    bool  GroundedSync() const;            
    bool  HasInhibit() const;    
    bool  HasSyncOut() const;
    void  Initialize();
    const ProgramTypes::SetType& HighestIinSeen() const;
    const ProgramTypes::SetType& HighestVinSeen() const;
    const ProgramTypes::SetType& HighLine() const;
    const ProgramTypes::SetType Iout(ConverterOutput::Output channel) const;
    const ProgramTypes::MTypeContainer& Iouts() const;
    bool  JumperPullIout() const;
    bool  JumperPullVout() const;
    const ProgramTypes::SetType& LowLine() const;
    const ProgramTypes::SetType& MaxIinNoLoad() const;
    const ProgramTypes::SetType& NominalLine() const;
    ConverterOutput::Output NumberOutputs() const;
    std::vector<ConverterOutput::Output> Outputs() const;
    std::string SerialNumber() const;
	bool SkipIinPard() const;
    const ProgramTypes::SetType& SyncAmplitude() const;
    const ProgramTypes::PercentType& SyncDutyCycle() const;
    const ProgramTypes::SetType& SyncOffset() const;
	bool UseLoadMeter() const;
    const ProgramTypes::SetType Vout(ConverterOutput::Output channel) const;
	const ProgramTypes::MTypeContainer& Vouts() const;
	long InhibitLifeCycle() const;
    //======================
    // End Public Interface
    //======================

private:
    std::string name() const;
    void setInfo();
	
private:
    // Forward declaration
    class Pimpl;
    friend class SingletonType<Converter>;
    Converter();
	~Converter();	

private:  // Private members for Converter technical info		
    std::auto_ptr<Pimpl> pimpl_;       
};

#endif // SPTS_CONVERTER_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
