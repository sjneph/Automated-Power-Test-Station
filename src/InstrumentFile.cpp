// Files included
#include "Assertion.h"
#include "GenericAlgorithms.h"
#include "InstrumentFile.h"
#include "MainSupplyTraits.h"
#include "ScaleUnits.h"
#include "SPTSException.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/14/05, sjn,
   ==============
     Added member function: SupplyCCModeChangePause() to reflect changes to actual
       instrument configuration file.  When we force a supply into CC mode by current
       limiting & shorting, it takes time for the supply to actually go into overcurrent,
       with some supplies taking a second or more.  This cannot be captured well with
       one pause value for all supplies, especially since so many different main supplies
       are supported with radically different design characteristics.

   ==============
   10/07/05, sjn,
   ==============
     Modified layout of actual instrument file in regards to main power supplies to make
       it simpler to configure station supplies.  All member functions associated with
       main power supplies were updated to reflect this.  Additionally, getBoolean<>()
       and convertSupplyTypes() were addeded (helper functions).

   ==============
   06/23/05, sjn,
   ==============
     Added implementations for MaxAmplitude(), MaxDutyCycle(), MaxFrequency(),
       MaxOffset(), MinAmplitude(), MinDutyCycle() and SetFunctionGeneratorType().  All
       for Function Generator.  Modified GetModelType() --> special case for func gen.

   ==============
   03/14/05, sjn,
   ==============
     Removed CurrentProbeScales(Types which) to reflect changes to current probe system.
     Removed References to: InstrumentFile::Types::DCPROBELOWCURRENT and
       InstrumentFile::Types::DCPROBEHIGHCURRENT.  Replaced both with new
       InstrumentFile::Types::CURRENTPROBE
     Modified all member functions taking a single LoadTraits::Channels argument which
       retrieve load module information to reflect changes to layout of external
       instrument file.

   ==============
   12/20/04, sjn,
   ==============
     Added SetScopeType() implementation.
     Modified GetModelType(), getScopeRange(), ScopeHorizontalScales(), and 
      ScopeVerticalScales() to reflect changes to layout of external instrument file
      (to support dynamic identification of oscilloscopes).
     Removed #include "ElectronicLoad.h" --> not used here
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef InstrumentFile::MaxType MaxType;
    typedef InstrumentFile::MinType MinType;
    typedef InstrumentFile::PercentType PercentType;

    typedef StationExceptionTypes::BadArg    BadArg;
    typedef StationExceptionTypes::FileError FileError;

    // convertSupplyTypes()
    SPTSInstrument::InstrumentTypes::Types
                              convertSupplyTypes(MainSupplyTraits::Supply sup) {
        if ( sup == MainSupplyTraits::PS1 )
            return(SPTSInstrument::InstrumentTypes::PS1);
        else if ( sup == MainSupplyTraits::PS2 )
            return(SPTSInstrument::InstrumentTypes::PS2);
        return(SPTSInstrument::InstrumentTypes::PS3);
    }

    // getBoolean<ErrorType> class
    template <typename ErrorType>
    bool getBoolean(const std::string& s) {
        if ( ("1" == s) || ("TRUE" == Uppercase(s)) )
            return(true);
        if ( ("0" == s) || ("FALSE" == Uppercase(s)) )
            return(false);
        throw(ErrorType(InstrumentFile::Name()));
    }
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//==============
// GetAddress() 
//==============
long InstrumentFile::GetAddress(Types type) {
    std::string add = "GPIB Address";
    if ( type == InstrumentTypes::PS1 )
        add = "PS1 GPIB Address";
    else if ( type == InstrumentTypes::PS2 )
        add = "PS2 GPIB Address";
    else if ( type == InstrumentTypes::PS3 )
        add = "PS3 GPIB Address";

    add = if_.GetVariableValue(GetName(type), add);
    Assert<FileError>(!add.empty(), name());
    GetNumericInteger(add);
    return(convert<long>(add));
}    

//================
// GetModelType()
//================
std::string InstrumentFile::GetModelType(Types type) {
    std::string model;
    if ( OSCOPE == type )
        model = if_.GetVariableValue(GetName(type), scopeType_, "Model Number");
    else if ( FUNCTIONGENERATOR == type )
        model = if_.GetVariableValue(GetName(type), fgType_, "Model Number");
    else if ( PS1 == type ) {
        model = if_.GetVariableValue(GetName(type), "PS1");
        model = if_.GetVariableValue(GetName(type), model, "Model Number");
    }
    else if ( PS2 == type ) {
        model = if_.GetVariableValue(GetName(type), "PS2");
        model = if_.GetVariableValue(GetName(type), model, "Model Number");
    }
    else if ( PS3 == type ) {
        model = if_.GetVariableValue(GetName(type), "PS3");
        model = if_.GetVariableValue(GetName(type), model, "Model Number");
    }
    else
        model = if_.GetVariableValue(GetName(type), "Model Number");
    Assert<FileError>(!model.empty(), name());
    return(model);
}

//===========
// GetName()
//===========
std::string InstrumentFile::GetName(Types type) {
    switch(type) {
        case InstrumentFile::Types::APS:
            return("AUXILIARY POWER SUPPLY");
        case InstrumentFile::Types::CURRENTPROBE:
            return("DC CURRENT PROBE");
        case InstrumentFile::Types::DMM:
            return("DMM");
        case InstrumentFile::Types::ELECTRONICLOAD:
            return("ACTIVE LOAD");
        case InstrumentFile::Types::FUNCTIONGENERATOR:
            return("FUNCTION GENERATOR");
        case InstrumentFile::Types::INPUTRELAYCONTROL:
            return("CONTROLMATRIX::INPUTRELAYCONTROL");
        case InstrumentFile::Types::MISC:
            return("CONTROLMATRIX::MISC");
        case InstrumentFile::Types::OUTPUTRELAYCONTROL:
            return("CONTROLMATRIX::OUTPUTRELAYCONTROL");
        case InstrumentFile::Types::OSCOPE: 
            return("OSCILLOSCOPE");
        case InstrumentFile::Types::PS1:
            return("MAIN POWER SUPPLY"); // same as other mains
        case InstrumentFile::Types::PS2: 
            return("MAIN POWER SUPPLY"); // same as other mains
        case InstrumentFile::Types::PS3:
            return("MAIN POWER SUPPLY"); // same as other mains
        case InstrumentFile::Types::RLL:
            return("CONTROLMATRIX::RLL");
        case InstrumentFile::Types::SWITCHMATRIXDC:
            return("DC SWITCH MATRIX");
        case InstrumentFile::Types::SWITCHMATRIXFILTER:
            return("FILTER SWITCH MATRIX");
        case InstrumentFile::Types::SWITCHMATRIXRF:
            return("RF SWITCH MATRIX");
        case InstrumentFile::Types::TEMPCONTROLLER:
            return("TEMPERATURE CONTROLLER");
        default: // ?
            throw(BadArg(), name());
    };
}

//=================
// getScopeRange()
//=================
std::pair<MinType, MaxType> InstrumentFile::getScopeRange(const std::string& minVal, 
                                                          const std::string& maxVal) {
    std::string min, max; 
    min = if_.GetVariableValue(GetName(Types::OSCOPE), scopeType_, minVal);
    max = if_.GetVariableValue(GetName(Types::OSCOPE), scopeType_, maxVal);
    Assert<FileError>(!(max.empty() || min.empty()), name());
    std::pair<MinType, ScaleUnits<MinType>::Units> m;
    std::pair<MaxType, ScaleUnits<MaxType>::Units> n;
    m = ScaleUnits<MinType>::GetUnits(min);
    n = ScaleUnits<MaxType>::GetUnits(max);
    MinType s = ScaleUnits<MinType>::ScaleUp(m.first, m.second); // max
    MaxType t = ScaleUnits<MaxType>::ScaleUp(n.first, n.second); // min
    return(std::make_pair(s, t)); 
}

//================
// LoadAccuracy()
//================
ProgramTypes::SetType InstrumentFile::LoadAccuracy(LoadTraits::Channels chan, 
                                                   const SetType&) {
    std::string ch = "CH" + convert<std::string>(chan), acc;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    acc = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, "Accuracy");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(acc);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));   
}

//==================
// LoadResolution()
//==================
ProgramTypes::SetType InstrumentFile::LoadResolution(LoadTraits::Channels chan,
                                                     const SetType& val) {
    std::string ch = "CH" + convert<std::string>(chan), max;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    max = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model,
                               "Lo Range Max Amps");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;    
    m = ScaleUnits<MaxType>::GetUnits(max);
    SetType lowRange = ScaleUnits<MaxType>::ScaleUp(m.first, m.second);

    std::string resolution;
    if ( lowRange > val )
        resolution = "Lo Range Resolution";
    else 
        resolution = "Hi Range Resolution";
    std::string toRtn;
    toRtn = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, resolution); 
    m = ScaleUnits<MaxType>::GetUnits(toRtn);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));     
}

//================
// MaxAmplitude()
//================
MaxType InstrumentFile::MaxAmplitude(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Max Amplitude";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string max = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));  
}

//=====================
// MaxAmps() Overload1
//=====================
MaxType InstrumentFile::MaxAmps(LoadTraits::Channels chan) {
    std::string ch = "CH" + convert<std::string>(chan), ma;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    ma = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, "Max Amps");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(ma);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));   
}

//=====================
// MaxAmps() Overload2
//=====================
MaxType InstrumentFile::MaxAmps(MainSupplyTraits::Supply sup) {
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "Max Current";
    std::string max = if_.GetVariableValue(GetName(type), model, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));
}

//=====================
// MaxAmps() Overload3
//=====================
MaxType InstrumentFile::MaxAmps(AuxSupplyTraits::Channels chan, 
                                AuxSupplyTraits::Range r) {
    std::string label = "Chan" + convert<std::string>(chan) + " Max Current";
    label += " Range" + convert<std::string>(r);
    std::string max = if_.GetVariableValue(GetName(Types::APS), label);  
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second)); 
}

//=====================
// MaxAmps() Overload4
//=====================
MaxType InstrumentFile::MaxAmps(Types currentProbeType) {
    Assert<BadArg>(currentProbeType == Types::CURRENTPROBE, name());
    std::string label = "Maximum Current Rating";    
    std::string max = if_.GetVariableValue(GetName(currentProbeType), label);  
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second)); 
}

//================
// MaxDutyCycle() 
//================
PercentType InstrumentFile::MaxDutyCycle(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Max Duty Cycle";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string max = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    MaxType tmp = ScaleUnits<MaxType>::ScaleUp(m.first, m.second);
    PercentType pt(tmp.ValueStr());
    return(pt);
}

//================
// MaxFrequency() 
//================
MaxType InstrumentFile::MaxFrequency(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Max Frequency";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string max = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));
}

//================
// MaximumScale()
//================
MaxType InstrumentFile::MaximumScale(Types type) {
    std::string max = if_.GetVariableValue(GetName(type), "Maximum Scale"); 
    Assert<FileError>(!max.empty(), name());
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second)); 
}

//======================
// MaximumTemperature()
//======================
MaxType InstrumentFile::MaximumTemperature() {
    std::string maxHW, maxSW;
    maxHW = if_.GetVariableValue(GetName(TEMPCONTROLLER), "Max HW Temperature");
    maxSW = if_.GetVariableValue(GetName(TEMPCONTROLLER), "Max SW Temperature");
    Assert<FileError>(!(maxHW.empty() || maxSW.empty()), name());
    MaxType one = convert<MaxType>(maxSW), two = convert<MaxType>(maxHW);
    Assert<FileError>(one < two, name());
    return(one);    
}

//=============
// MaxOffset()
//=============
MaxType InstrumentFile::MaxOffset(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Max Offset";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string max = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));
}

//===========
// MaxOhms()
//===========
MaxType InstrumentFile::MaxOhms(LoadTraits::Channels chan) {
    std::string ch = "CH" + convert<std::string>(chan), mo;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    mo = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, "Max Ohms");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(mo);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));       
}

//======================
// MaxVolts() Overload1
//======================
MaxType InstrumentFile::MaxVolts(LoadTraits::Channels chan) {
    std::string ch = "CH" + convert<std::string>(chan), mv;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    mv = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, "Max Volts");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(mv);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second)); 
}

//======================
// MaxVolts() Overload2
//======================
MaxType InstrumentFile::MaxVolts(MainSupplyTraits::Supply sup) {
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "Max Voltage";
    std::string max = if_.GetVariableValue(GetName(type), model, label);
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));
}

//======================
// MaxVolts() Overload3
//======================
MaxType InstrumentFile::MaxVolts(AuxSupplyTraits::Channels chan, 
                                 AuxSupplyTraits::Range r) {
    std::string label = "Chan" + convert<std::string>(chan) + " Max Voltage";
    label += " Range" + convert<std::string>(r);
    std::string max = if_.GetVariableValue(GetName(Types::APS), label);  
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second)); 
}

//======================
// MaxVolts() Overload4
//======================
MaxType InstrumentFile::MaxVolts(Types dmm) {
    Assert<BadArg>(dmm == Types::DMM, name());
    std::string max = if_.GetVariableValue(GetName(dmm), "Max Voltage");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(max);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));
}

//================
// MinAmplitude()
//================
MinType InstrumentFile::MinAmplitude(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Min Amplitude";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string min = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MinType, ScaleUnits<MinType>::Units> m;
    m = ScaleUnits<MinType>::GetUnits(min);
    return(ScaleUnits<MinType>::ScaleUp(m.first, m.second));
}

//================
// MinDutyCycle()
//================
PercentType InstrumentFile::MinDutyCycle(Types funcGen) {
    Assert<BadArg>(funcGen == FUNCTIONGENERATOR, name());
    std::string label = "Min Duty Cycle";
    std::string fgName = GetName(FUNCTIONGENERATOR);
    std::string min = if_.GetVariableValue(fgName, fgType_, label);
    std::pair<MinType, ScaleUnits<MinType>::Units> m;
    m = ScaleUnits<MinType>::GetUnits(min);
    MinType tmp = ScaleUnits<MinType>::ScaleUp(m.first, m.second);
    PercentType pt(tmp.ValueStr());
    return(pt);
}

//================
// MinimumScale()
//================
MinType InstrumentFile::MinimumScale(Types type) {
    std::string min = if_.GetVariableValue(GetName(type), "Minimum Scale"); 
    Assert<FileError>(!min.empty(), name());
    std::pair<MinType, ScaleUnits<MinType>::Units> m;
    m = ScaleUnits<MinType>::GetUnits(min);
    return(ScaleUnits<MinType>::ScaleUp(m.first, m.second));
}

//======================
// MinimumTemperature()
//======================
MinType InstrumentFile::MinimumTemperature() {
    std::string minHW, minSW;
    minHW = if_.GetVariableValue(GetName(TEMPCONTROLLER), "Min HW Temperature");
    minSW = if_.GetVariableValue(GetName(TEMPCONTROLLER), "Min SW Temperature");
    Assert<FileError>(!(minHW.empty() || minSW.empty()), name());
    MaxType two = convert<MinType>(minSW), one = convert<MinType>(minHW);
    Assert<FileError>(one < two, name());
    return(two);       
}

//===========
// MinOhms()
//===========
MaxType InstrumentFile::MinOhms(LoadTraits::Channels chan) {
    std::string ch = "CH" + convert<std::string>(chan), mo;
    std::string model = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), ch);
    mo = if_.GetVariableValue(GetName(Types::ELECTRONICLOAD), model, "Min Ohms");
    std::pair<MaxType, ScaleUnits<MaxType>::Units> m;
    m = ScaleUnits<MaxType>::GetUnits(mo);
    return(ScaleUnits<MaxType>::ScaleUp(m.first, m.second));  
}

//========
// name() 
//========
std::string InstrumentFile::name() {
    return(Name());
}

//========
// Name()
//========
std::string InstrumentFile::Name() {
    return("InstrumentFile Class");
}

//==========================
// ScopeExtTrigLevelRange() 
//==========================
std::pair<MinType, MaxType> InstrumentFile::ScopeExtTrigLevelRange() {
    return(getScopeRange("Min Ext Trig Level", "Max Ext Trig Level"));    
}

//=========================
// ScopeHorizontalScales()
//=========================
ProgramTypes::SetTypeContainer InstrumentFile::ScopeHorizontalScales() {
    std::string toGet = "Horizontal Scales";
    toGet = if_.GetVariableValue(GetName(Types::OSCOPE), scopeType_, toGet);
    std::vector<std::string> scales = SplitString(toGet, ',');
    typedef ProgramTypes::SetType SetType;
    std::vector<SetType> toRtn;

    std::pair<SetType, ScaleUnits<SetType>::Units> m;
    std::vector<std::string>::iterator i = scales.begin();
    while ( i != scales.end() ) {        
        m = ScaleUnits<SetType>::GetUnits(*i); 
        toRtn.push_back(ScaleUnits<SetType>::ScaleUp(m.first, m.second));   
        ++i;
    }
    std::sort(toRtn.begin(), toRtn.end());
    return(toRtn);     
}

//=======================
// ScopeHorzScaleRange() 
//=======================
std::pair<MinType, MaxType> InstrumentFile::ScopeHorzScaleRange() {
    ProgramTypes::SetTypeContainer s = ScopeHorizontalScales();
    Assert<FileError>(static_cast<long>(s.size()) >= 2, name());
    return(std::make_pair(s[0], s[s.size()-1]));
}

//====================
// ScopeOffsetRange() 
//====================
std::pair<MinType, MaxType> InstrumentFile::ScopeOffsetRange() {
    return(getScopeRange("Min Offset", "Max Offset"));    
}

//=======================
// ScopeVerticalScales()
//=======================
ProgramTypes::SetTypeContainer InstrumentFile::ScopeVerticalScales() {    
    std::string toGet = "Vertical Scales";
    toGet = if_.GetVariableValue(GetName(Types::OSCOPE), scopeType_, toGet);
    std::vector<std::string> scales = SplitString(toGet, ',');
    typedef ProgramTypes::SetType SetType;
    std::vector<SetType> toRtn;

    std::pair<SetType, ScaleUnits<SetType>::Units> m;
    std::vector<std::string>::iterator i = scales.begin();
    while ( i != scales.end() ) {        
        m = ScaleUnits<SetType>::GetUnits(*i); 
        toRtn.push_back(ScaleUnits<SetType>::ScaleUp(m.first, m.second));   
        ++i;
    }
    return(toRtn);    
}

//=======================
// ScopeVertScaleRange() 
//=======================
std::pair<MinType, MaxType> InstrumentFile::ScopeVertScaleRange() {
    ProgramTypes::SetTypeContainer s = ScopeVerticalScales();
    Assert<FileError>(static_cast<long>(s.size()) >= 2, name());
    return(std::make_pair(s[0], s[s.size()-1]));
}

//============================
// SetFunctionGeneratorType()
//============================
void InstrumentFile::SetFunctionGeneratorType(const std::string& fgType) {
    fgType_ = fgType;
}

//=============================
// SetMainSupplyVoltsWithDMM()
//=============================
bool InstrumentFile::SetMainSupplyVoltsWithDMM(MainSupplyTraits::Supply sup) {
    typedef ProgramTypes::SetType SetType;
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "DMM Verify Change";
    std::string toGet = if_.GetVariableValue(GetName(type), model, label);
    return(getBoolean<FileError>(toGet));
}

//================
// SetScopeType()
//================
void InstrumentFile::SetScopeType(const std::string& scopeType) {
    scopeType_ = scopeType;
}

//===========================
// SupplyCCModeChangePause()
//===========================
ProgramTypes::SetType
     InstrumentFile::SupplyCCModeChangePause(MainSupplyTraits::Supply sup) {
    typedef ProgramTypes::SetType SetType;
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "CC Mode Pause Value";
    std::string toGet = if_.GetVariableValue(GetName(type), model, label);

    // Scale and return value
    std::pair<MaxType, ScaleUnits<SetType>::Units> m;
    m = ScaleUnits<SetType>::GetUnits(toGet);
    return(ScaleUnits<SetType>::ScaleUp(m.first, m.second));
}

//===================
// VoltageAccuracy() 
//===================
ProgramTypes::SetType InstrumentFile::VoltageAccuracy(MainSupplyTraits::Supply sup) {
    typedef ProgramTypes::SetType SetType;
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "Voltage Accuracy";
    std::string toGet = if_.GetVariableValue(GetName(type), model, label);

    // Scale and return value
    std::pair<MaxType, ScaleUnits<SetType>::Units> m;
    m = ScaleUnits<SetType>::GetUnits(toGet);
    return(ScaleUnits<SetType>::ScaleUp(m.first, m.second));
}

//=====================
// VoltageResolution() 
//=====================
ProgramTypes::SetType InstrumentFile::VoltageResolution(MainSupplyTraits::Supply sup) {
    typedef ProgramTypes::SetType SetType;
    InstrumentTypes::Types type = convertSupplyTypes(sup);
    std::string model = GetModelType(type);
    std::string label = "Voltage Resolution";
    std::string toGet = if_.GetVariableValue(GetName(type), model, label);

    // Scale and return value
    std::pair<MaxType, ScaleUnits<SetType>::Units> m;
    m = ScaleUnits<SetType>::GetUnits(toGet);
    return(ScaleUnits<SetType>::ScaleUp(m.first, m.second));
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
