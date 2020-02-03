// Macro Guard
#ifndef SPTS_INSTRUMENTFILE_H
#define SPTS_INSTRUMENTFILE_H

// Files included
#include "Assertion.h"
#include "AuxSupplyTraits.h"
#include "InstrumentTypes.h"
#include "LoadTraits.h"
#include "MainSupplyTraits.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/14/05, sjn,
   ==============
     Added member function: SupplyCCModeChangePause() to reflect changes to actual
       instrument configuration file.  This gathers a pause value depending on which
       supply is in use.

   ==============
   10/07/05, sjn,
   ==============
     Added member function: SetMainSupplyVoltsWithDMM() to reflect changes to actual
       instrument configuration file.

   ==============
   06/23/05, sjn,
   ==============
     Added member funcs for Function Generator use: MaxAmplitude(), MaxDutyCycle(),
       MaxFrequency(), MaxOffset(), MinAmplitude(), MinDutyCycle() and
       SetFunctionGeneratorType().  Added fgType_ member variable.  Added typedef for
       PercentType.

   ==============
   03/02/05, sjn,
   ==============
     Removed CurrentProbeScales(Types which) to reflect changes to current probe system.

   ==============  
   12/20/04, sjn,
   ==============
     Added SetScopeType() - allow dynamic loading of oscilloscope types.
     Added member variable: scopeType_
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct InstrumentFile : public SPTSInstrument::InstrumentTypes, NoCopy {

    // Public Typedefs
    typedef ProgramTypes::SetType MinType;
    typedef ProgramTypes::SetType MaxType;
    typedef ProgramTypes::PercentType PercentType;
    typedef ProgramTypes::SetType SetType;

    //========================
    // Start Public Interface
    //========================
    long GetAddress(Types type);    
    std::string GetModelType(Types type);
    static std::string GetName(Types type);
    SetType LoadAccuracy(LoadTraits::Channels chan, const SetType& val);
    SetType LoadResolution(LoadTraits::Channels chan, const SetType& val);
    MaxType MaxAmplitude(Types funcGen = FUNCTIONGENERATOR);
    MaxType MaxAmps(LoadTraits::Channels chan);
    MaxType MaxAmps(MainSupplyTraits::Supply sup);
    MaxType MaxAmps(AuxSupplyTraits::Channels chan, AuxSupplyTraits::Range r);
    MaxType MaxAmps(Types currentProbeType);
    PercentType MaxDutyCycle(Types funcGen = FUNCTIONGENERATOR);
    MaxType MaxFrequency(Types funcGen = FUNCTIONGENERATOR);
    MaxType MaximumScale(Types type);
    MaxType MaximumTemperature();
    MaxType MaxOffset(Types funcGen = FUNCTIONGENERATOR);
    MaxType MaxOhms(LoadTraits::Channels chanz);
    MaxType MaxVolts(LoadTraits::Channels chan);    
    MaxType MaxVolts(MainSupplyTraits::Supply sup);    
    MaxType MaxVolts(AuxSupplyTraits::Channels chan, AuxSupplyTraits::Range r);
    MaxType MaxVolts(Types type = DMM);
    MinType MinAmplitude(Types funcGen = FUNCTIONGENERATOR);
    PercentType MinDutyCycle(Types funcGen = FUNCTIONGENERATOR);
    MinType MinimumScale(Types type);
    MinType MinimumTemperature();
    MinType MinOhms(LoadTraits::Channels chan);
    static std::string Name();
    std::pair<MinType, MaxType> ScopeExtTrigLevelRange();
    ProgramTypes::SetTypeContainer ScopeHorizontalScales();
    std::pair<MinType, MaxType> ScopeHorzScaleRange();
    std::pair<MinType, MaxType> ScopeOffsetRange(); 
    ProgramTypes::SetTypeContainer ScopeVerticalScales();
    std::pair<MinType, MaxType> ScopeVertScaleRange();
    void SetFunctionGeneratorType(const std::string& fgType);
    bool SetMainSupplyVoltsWithDMM(MainSupplyTraits::Supply sup);
    void SetScopeType(const std::string& scopeType);
    SetType SupplyCCModeChangePause(MainSupplyTraits::Supply sup);
    SetType VoltageAccuracy(MainSupplyTraits::Supply sup);
    SetType VoltageResolution(MainSupplyTraits::Supply sup);    
    //======================
    // End Public Interface
    //======================

private:
    friend class SingletonType<InstrumentFile>;
    InstrumentFile()
    { /* */ }
    ~InstrumentFile()
    { /* */ }

private:
    static std::string name();
    std::pair<MinType, MaxType> getScopeRange(const std::string& minVal, 
                                              const std::string& maxVal);

private:
    FileTypes::InstrumentFileType if_;
    std::string scopeType_;
    std::string fgType_;
};

#endif // SPTS_INSTRUMENTFILE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
